import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.concurrent.Task;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Stage;
import javafx.util.converter.NumberStringConverter;

import java.io.FileNotFoundException;
import java.io.IOException;

public class Game {

    final String leftImage = "thumb_up_left.png";
    final String rightImage = "thumb_up_right.png";

    Button guessBttn = new Button("Tipuju");
    TextField guessTF = new TextField();

    boolean endMe = true;
    Button[] imageViewButtons;

    String incMessage = "";
    boolean notYourTurn = false;
    TextField fingers = new TextField();

    Thread thread = new Thread();
    Intro intro;
    Stage primStage;
    ImageLoader imageLoader = new ImageLoader();
    Message message;
    Gamer gamer = new Gamer();
    int remainingFingers;
    int[] numOfFingers = new int[8];

    int numberOfPlayers;
    String[] namesOfPlayers;
    String inputedName;

    final BooleanProperty booleanProperty = new SimpleBooleanProperty(true);

    boolean leftBttn = false;
    boolean rightBttn = false;

    int minHeight = 880;
    int minWidth = 650;

    public Game(Stage stage, int numOfPlayers,  String[] names, String inputName,Message message1) {
        primStage = stage;
        numberOfPlayers = numOfPlayers;
        namesOfPlayers = names;
        inputedName = inputName;
        message = message1;
        gamer.setName(inputName);
        imageViewButtons = new Button[numOfPlayers*2];
        remainingFingers = numOfPlayers*2;
    }

    public Game(Stage stage, int numOfPlayers, String[] names,int[] fingers,String inputName, Message message1){
        primStage = stage;
        numberOfPlayers = numOfPlayers;
        namesOfPlayers = names;
        numOfFingers = fingers;
        message = message1;
        inputedName = inputName;
        gamer.setName(inputName);
        imageViewButtons = new Button[numOfPlayers*2];
        for (int i = 0; i < fingers.length; i++) {
            remainingFingers += fingers[i];
        }

    }


    public Parent createRootPanee() throws FileNotFoundException {
        BorderPane rootPane = new BorderPane();

        ImageView[] imageViews = imageLoader.loadImages(numberOfPlayers);

        switch (numberOfPlayers){
            case 2:
                rootPane.setCenter(createTopCenter(imageViews[0],namesOfPlayers[0],numOfFingers[0],isActualGamer(inputedName,namesOfPlayers[0])));
                rootPane.setBottom(createBottom(imageViews[1],namesOfPlayers[1],numOfFingers[1],isActualGamer(inputedName,namesOfPlayers[1])));
                break;
            case 3:
                rootPane.setCenter(createTopCenter(imageViews[0],namesOfPlayers[0],numOfFingers[0],isActualGamer(inputedName,namesOfPlayers[0])));
                rootPane.setBottom(createBottom(imageViews[1],namesOfPlayers[1],numOfFingers[1],isActualGamer(inputedName,namesOfPlayers[1])));
                rootPane.setRight(createRight(imageViews[2],namesOfPlayers[2],numOfFingers[2],isActualGamer(inputedName,namesOfPlayers[2])));
                break;
            case 4:
                rootPane.setCenter(createTopCenter(imageViews[0],namesOfPlayers[0],numOfFingers[0],isActualGamer(inputedName,namesOfPlayers[0])));
                rootPane.setBottom(createBottom(imageViews[1],namesOfPlayers[1],numOfFingers[1],isActualGamer(inputedName,namesOfPlayers[1])));
                rootPane.setRight(createRight(imageViews[2],namesOfPlayers[2],numOfFingers[2],isActualGamer(inputedName,namesOfPlayers[2])));
                rootPane.setLeft(createLeft(imageViews[3],namesOfPlayers[3],numOfFingers[3],isActualGamer(inputedName,namesOfPlayers[3])));
                break;
        }



        return rootPane;
    }

    private ImageView formatThumbsUp(ImageView thumbsUp){
        thumbsUp.setFitHeight(40);
        thumbsUp.setFitWidth(40);
        return thumbsUp;
    }

    private Node createTopCenter(ImageView imageView, String name, int numOfFingers, boolean isActualGamer) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        VBox main = new VBox();

        Label nameOfPlayer = new Label(name);
        nameOfPlayer.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,13));

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage(leftImage));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage(rightImage));


        Button thumbUpLeftBttn = new Button();
        Button thumbUpRightBttn = new Button();

        addImageViews(thumbUpLeftBttn,thumbUpRightBttn,0);
        if (numOfFingers == 1){
            hideImageView(name);
        }

        thumbUpLeftBttn.setGraphic(thumbsUpLeft);
        thumbUpRightBttn.setGraphic(thumbsUpRight);


        if (isActualGamer){

            thumbUpLeftBttn.setOnAction(actionEvent -> {
                if (!leftBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    leftBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    leftBttn = false;
                }
            });

            thumbUpRightBttn.setOnAction(actionEvent -> {
                if (!rightBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    rightBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    rightBttn = false;
                }
            });


            fingers.setMaxWidth(50);
            fingers.textProperty().bindBidirectional(gamer.fingersSelectedProperty(), new NumberStringConverter());
            fingers.setDisable(true);
            fingers.setAlignment(Pos.CENTER);
            fingers.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,16));
            hBox1.setAlignment(Pos.TOP_CENTER);
            hBox1.getChildren().add(fingers);

        } else {
            thumbUpLeftBttn.setDisable(true);
            thumbUpRightBttn.setDisable(true);
        }


        hBox.getChildren().addAll(thumbUpLeftBttn,thumbUpRightBttn);

        hBox.setAlignment(Pos.TOP_CENTER);
        hBox.setSpacing(5);
        hBox.setPadding(new Insets(15,0,0,0));

        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.TOP_CENTER);
        vBox.setSpacing(10);



        VBox guessBox = new VBox();
        Label guess = new Label("Tvůj tip");



        //ABY TEXTFIELD NEMĚL FOCUS
        guessTF.focusedProperty().addListener((observable,  oldValue,  newValue) -> {
            if(newValue && booleanProperty.get()){
                vBox.requestFocus(); // Delegate the focus to container
                booleanProperty.setValue(false); // Variable value changed for future references
            }
        });

        guessTF.textProperty().addListener((observableValue, s, t1) -> {
            if (!t1.matches("\\d*")) {
                guessTF.setText(t1.replaceAll("[^\\d]", ""));
            }
            try {
                if(Integer.parseInt(t1) > remainingFingers){
                    guessTF.setText(s);
                }
            } catch (Exception ignored){
            }

        });
        guessTF.setText("0");

        guessBttnControl(isActualGamer);

        guessBttn.setOnMouseClicked(mouseEvent -> {
            if (thread.isAlive()){
                thread.interrupt();
            }
            if (guessTF.getText().equals("")){
                guessTF.setText("0");
            }
            try {
                message.writeSomething(CommandsClient.GUESS+"|"+guessTF.getText());
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                Thread.sleep(150);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });


        guess.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,17));
        guessTF.setMaxWidth(75);
        guessTF.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,15));
        guessTF.setAlignment(Pos.CENTER);
        guessBox.setPadding(new Insets(primStage.getHeight()/2-85,0,0,0));
        guessBox.setSpacing(10);
        guessBox.getChildren().addAll(guess,guessTF,guessBttn);
        guessBox.setAlignment(Pos.CENTER);

        main.getChildren().addAll(vBox,guessBox);

        return main;
    }

    private Node createBottom(ImageView imageView, String name, int numOfFingers, boolean isActualGamer) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label(name);

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage(leftImage));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage(rightImage));

        Button thumbUpLeftBttn = new Button();
        Button thumbUpRightBttn = new Button();

        addImageViews(thumbUpLeftBttn,thumbUpRightBttn,2);

        if (numOfFingers == 1){
            hideImageView(name);
        }

        thumbUpLeftBttn.setGraphic(thumbsUpLeft);
        thumbUpRightBttn.setGraphic(thumbsUpRight);

        if (isActualGamer){

            thumbUpLeftBttn.setOnAction(actionEvent -> {
                if (!leftBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    leftBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    leftBttn = false;
                }
            });

            thumbUpRightBttn.setOnAction(actionEvent -> {
                if (!rightBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    rightBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    rightBttn = false;
                }
            });


            fingers.setMaxWidth(50);
            fingers.textProperty().bindBidirectional(gamer.fingersSelectedProperty(), new NumberStringConverter());
            fingers.setDisable(true);
            fingers.setAlignment(Pos.CENTER);
            fingers.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,16));
            hBox1.setAlignment(Pos.BOTTOM_CENTER);
            hBox1.getChildren().add(fingers);

        } else {
            thumbUpLeftBttn.setDisable(true);
            thumbUpRightBttn.setDisable(true);
        }

        hBox.getChildren().addAll(thumbUpLeftBttn,thumbUpRightBttn);
        hBox.setAlignment(Pos.BOTTOM_CENTER);
        hBox.setSpacing(5);

        nameOfPlayer.setPadding(new Insets(0,0,35,0));
        nameOfPlayer.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,13));

        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.BOTTOM_CENTER);
        vBox.setSpacing(10);
        return vBox;
    }

    private Node createRight(ImageView imageView, String name, int numOfFingers, boolean isActualGamer) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label(name);

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage(leftImage));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage(rightImage));

        Button thumbUpLeftBttn = new Button();
        Button thumbUpRightBttn = new Button();

        addImageViews(thumbUpLeftBttn,thumbUpRightBttn,4);

        if (numOfFingers == 1){
            hideImageView(name);
        }

        thumbUpLeftBttn.setGraphic(thumbsUpLeft);
        thumbUpRightBttn.setGraphic(thumbsUpRight);

        if (isActualGamer){

            thumbUpLeftBttn.setOnAction(actionEvent -> {
                if (!leftBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    leftBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    leftBttn = false;
                }
            });

            thumbUpRightBttn.setOnAction(actionEvent -> {
                if (!rightBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    rightBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    rightBttn = false;
                }
            });

            fingers.setMaxWidth(50);
            fingers.setAlignment(Pos.CENTER);
            fingers.textProperty().bindBidirectional(gamer.fingersSelectedProperty(), new NumberStringConverter());
            fingers.setDisable(true);
            fingers.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,16));
            hBox1.getChildren().add(fingers);

        } else {
            thumbUpLeftBttn.setDisable(true);
            thumbUpRightBttn.setDisable(true);
        }

        hBox.getChildren().addAll(thumbUpLeftBttn,thumbUpRightBttn);
        hBox.setAlignment(Pos.CENTER_RIGHT);
        hBox.setSpacing(5);

        int padding = imageLoader.widths[0]/2-65/2;
        hBox.setPadding(new Insets(0,padding,0,0));
        nameOfPlayer.setPadding(new Insets(0,padding+15,0,0));
        nameOfPlayer.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,13));

        hBox1.setPadding(new Insets(0,0,0,55));

        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.CENTER_RIGHT);
        vBox.setPadding(new Insets(210,0,0,0));
        vBox.setSpacing(10);
        return vBox;
    }

    private Node createLeft(ImageView imageView, String name, int numOfFingers, boolean isActualGamer) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label(name);

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage(leftImage));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage(rightImage));

        Button thumbUpLeftBttn = new Button();
        Button thumbUpRightBttn = new Button();

        addImageViews(thumbUpLeftBttn,thumbUpRightBttn,6);

        if (numOfFingers == 1){
            hideImageView(name);
        }

        thumbUpLeftBttn.setGraphic(thumbsUpLeft);
        thumbUpRightBttn.setGraphic(thumbsUpRight);

        if (isActualGamer){

            thumbUpLeftBttn.setOnAction(actionEvent -> {
                if (!leftBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    leftBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    leftBttn = false;
                }
            });

            thumbUpRightBttn.setOnAction(actionEvent -> {
                if (!rightBttn){
                    gamer.setFingersSelected(gamer.getFingersSelected()+1);
                    rightBttn = true;
                } else {
                    gamer.setFingersSelected(gamer.getFingersSelected()-1);
                    rightBttn = false;
                }
            });

            fingers.setMaxWidth(50);
            fingers.setAlignment(Pos.CENTER);
            fingers.textProperty().bindBidirectional(gamer.fingersSelectedProperty(), new NumberStringConverter());
            fingers.setDisable(true);
            fingers.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,16));
            hBox1.getChildren().add(fingers);

        } else {
            thumbUpLeftBttn.setDisable(true);
            thumbUpRightBttn.setDisable(true);
        }




        hBox.getChildren().addAll(thumbUpLeftBttn,thumbUpRightBttn);
        hBox.setAlignment(Pos.CENTER_LEFT);
        hBox.setSpacing(5);

        int padding = imageLoader.widths[0]/2-65/2;
        hBox.setPadding(new Insets(0,0,0,padding));
        nameOfPlayer.setPadding(new Insets(0,0,0,padding+10));
        nameOfPlayer.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,13));

        hBox1.setPadding(new Insets(0,0,0,padding+10));

        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.CENTER_LEFT);
        vBox.setPadding(new Insets(210,0,0,0));
        vBox.setSpacing(10);
        return vBox;
    }

    /**
     * Ukončí danou hru a vypíše oznámení
     * @param name Jméno hráče, který vyhrál
     */
    public void EndOfGame(String name){
        Alert a = new Alert(Alert.AlertType.INFORMATION);
        a.setHeaderText("Konec hry");
        a.setContentText("Hru vyhrál "+name);
        a.showAndWait();
        endMe = false;
        thread.interrupt();
        goBackToIntro();
    }

    /**
     * Změní scénu zpět na úvodní
     */
    public void goBackToIntro(){
        endMe = false;
        thread.interrupt();
        intro = new Intro(primStage,message, null, 0);
        primStage.setScene(new Scene(intro.createRootPane(),intro.minWidth, intro.minHeight));
        primStage.sizeToScene();
        primStage.setMinHeight(intro.minHeight);
        primStage.setMinWidth(intro.minWidth);
    }

    /**
     * Zjistí, zda je daný hráč na tahu
     * @param inputName Jméno hráče, který začíná
     * @param name Jméno hráče
     * @return TRUE|FALSE
     */
    public boolean isActualGamer(String inputName, String name){
        return inputName.equals(name);
    }

    /**
     * Vytvoří nové vlákno, které čeká než přijde zpráva
     */
    public void listenAndWait(){
        Task<Void> sleeperGame = new Task<>() {
            @Override
            protected Void call() {
                try {
                    while (endMe && (incMessage = message.readSomething()).equals("")) {
                        Thread.sleep(150);
                    }

                } catch (InterruptedException | IOException ignored) {}
                return null;
            }
        };
        sleeperGame.setOnSucceeded(event -> {
            try {
                parse(incMessage);
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
        thread = new Thread(sleeperGame);
        thread.start();
    }

    /**
     * Metoda pro obsluhu tipovacího tlačíka
     * @param turn hráč na tahu nebo ne
     */
    public void guessBttnControl(boolean turn){
        notYourTurn = turn;

        listenAndWait();

        // https://stackoverflow.com/questions/16919727/javafx-how-to-disable-a-button-for-a-specific-amount-of-time
        // DISABLE BUTTON FOR SPECIFIC TIME
        new Thread(() -> {
            Platform.runLater(() -> guessBttn.setDisable(true));
            try {
                Thread.sleep(5000); //5 seconds
            }
            catch(InterruptedException ignored) {
            }
            if (notYourTurn && numOfFingers[0] == 0){
                Platform.runLater(() -> guessBttn.setDisable(false));

            }
        }).start();
    }

    /**
     * Přidá tlačítku obrázek palce
     * @param thumbsUpLeft Tlačítko levé
     * @param thumbsUpRight Tlačítko pravé
     * @param i Index do pole
     */
    private void addImageViews(Button thumbsUpLeft, Button thumbsUpRight, int i) {
        imageViewButtons[i] = thumbsUpLeft;
        imageViewButtons[i+1] = thumbsUpRight;
    }

    /**
     * Schová jeden palec
     * @param inputedName Jméno hráče, kterému se má schovat palec
     */
    public void hideImageView(String inputedName){
        for (int i = 0; i < namesOfPlayers.length; i++) {
            if (namesOfPlayers[i].equals(inputedName)){
                imageViewButtons[i*2].setVisible(false);
                break;
            }
        }
    }

    /**
     * Předanou zprávu zpracuje a podle obsahu vykoná něco
     * @param message1 Zpráva ke zpracování
     * @throws IOException Chyba
     */
    public void parse(String message1) throws IOException {
        String command = message1.split("\\|")[0];

        switch (command) {
            //NENASTANE
            case "START":
                break;
            case "GIVEGUESS":
                incMessage = "";
                message.writeSomething(CommandsClient.FINGERS+"|"+ gamer.getFingersSelected());
                if (numOfFingers[0] != 0){
                    numOfFingers[0] = 0;
                }
                listenAndWait();
                break;
            case "ENDOFROUND":
                guessBttnControl(isActualGamer(message1.split("\\|")[1], gamer.getName()));
                fingers.setText("0");
                rightBttn = false;
                leftBttn = false;

                guessTF.setText("0");
                Alert a = new Alert(Alert.AlertType.INFORMATION);

                if (message1.split("\\|")[2].equals("true")){
                    a.setHeaderText("Správně!");
                    a.setContentText("Hráč: "+message1.split("\\|")[3]+" -> ČIM ČONG "+ message1.split("\\|")[5]);
                    hideImageView(message1.split("\\|")[3]);
                    gamer.setNumOfFingers(gamer.getNumOfFingers()-1);
                    remainingFingers--;
                } else {
                    a.setHeaderText("Chybička!");
                    a.setContentText("Hráč: "+message1.split("\\|")[3]+" -> ČIM ČONG "+ message1.split("\\|")[5] +" v reálu se zvedlo "+message1.split("\\|")[4]+" palců.\nSadge");
                }
                a.show();
                break;
            case "ENDGAME":
                EndOfGame(message1.split("\\|")[1]);
                break;
            case "RECONNECT":
                incMessage = "";
                Alert alertReconnect = new Alert(Alert.AlertType.INFORMATION);
                alertReconnect.setContentText("Hráč "+message1.split("\\|")[1]+" se znovu připojil do hry.");
                alertReconnect.setHeaderText("Znovu připojení.");
                alertReconnect.show();
                listenAndWait();
                break;
            case "LOGOUT":
                incMessage = "";
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setContentText("Hráč "+message1.split("\\|")[1]+" se odpojil. Hrajeme bez něj!\nJeho prsty budou vždy nula.");
                alert.setHeaderText("Odpojení ze hry");
                alert.show();
                guessBttnControl(isActualGamer(message1.split("\\|")[2], gamer.getName()));
                break;
            default:
                Alert alertWTF = new Alert(Alert.AlertType.ERROR);
                alertWTF.setContentText("Upsík dupsík, server se zbláznil, tady to balíme!");
                alertWTF.setHeaderText("Odpojení ze hry");
                alertWTF.show();
                goBackToIntro();
                System.out.println("Chybný příkaz -> "+command);
        }

    }
}

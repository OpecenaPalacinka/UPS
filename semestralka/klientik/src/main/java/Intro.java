import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.concurrent.Task;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Stage;

import java.io.FileNotFoundException;
import java.io.IOException;

public class Intro {
    /**
     * Maximální délka jména
     */
    final int maxLength = 15;

    Game game;
    Message message;
    Stage primStage;
    String forParser = "nic";
    String[] parsedMessage;
    String[] names;
    int[] fingers;
    String help;
    Alert a;
    String ip;
    int p;
    Thread thread;

    String loginMessage;

    int minWidth =  550;
    int minHeight = 350;

    final BooleanProperty booleanProperty = new SimpleBooleanProperty(true);

    /**
     * Konstruktor
     * @param stage Aktuální stage
     * @param mess Již vytvořená zpráva
     * @param ipAddress IP adresa z parametru
     * @param port Port z parametru
     */
    public Intro(Stage stage, Message mess, String ipAddress, int port) {
        primStage = stage;
        message = mess;
        ip = ipAddress;
        p = port;
    }

    /**
     * Inicializuje objekt Message, aby se připojil k serveru
     */
    public void initMessage(){
        if (!message.connected) {
            try {
                message.init(ip,p);
                message.readSomething();
            } catch (IOException e) {
                System.err.println("Nepodařilo se navázat spojení se serverem! Končíme");
                System.exit(0);
            }
        }
    }

    /**
     * Vytvoří okno jako celek
     * @return Obsah okna
     */
    public Parent createRootPane(){
        BorderPane rootPane = new BorderPane();

        rootPane.setTop(createTop());
        rootPane.setCenter(createCenter());

        return rootPane;
    }

    /**
     * Vytvoří nadpis v úvodním okně
     * @return Nadpis
     */
    private Node createTop() {
        VBox welcoming = new VBox();
        Label welcome = new Label("Vítejte ve hře Čim Čong!");
        welcome.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,25));
        welcoming.setAlignment(Pos.TOP_CENTER);
        welcoming.setPadding(new Insets(25,0,25,0));

        welcoming.getChildren().add(welcome);
        return welcoming;
    }


    /**
     * Vytvoří střed okna + reakce na kliknutí
     * @return Obsah středu okna
     */
    private Node createCenter(){
        VBox vBox = new VBox();
        Button button = new Button("Připojit do hry!");
        button.setPrefSize(150,65);
        button.setStyle("-fx-font-size: 1.3em; ");

        TextField name = new TextField();
        name.setTooltip(new Tooltip("Zadejte vaše jméno"));
        name.setPromptText("Zadejte vaše jméno");
        name.setMaxWidth(176);

        // Aby TF ztratil focus a zobrazil se placeholder
        name.focusedProperty().addListener((observable,  oldValue,  newValue) -> {
            if(newValue && booleanProperty.get()){
                vBox.requestFocus(); // Delegate the focus to container
                booleanProperty.setValue(false); // Variable value changed for future references
            }
        });
        //Maximalni delka jmena
        name.textProperty().addListener((ov, oldValue, newValue) -> {
            if (name.getText().length() > maxLength) {
                String s = name.getText().substring(0, maxLength);
                name.setText(s);
            }
        });


        button.setOnAction(mouseEvent -> {

            initMessage();

            if (name.getCharacters().toString().equals("")){
                name.setText("Trouba");
            }

            loginMessage = CommandsClient.LOGIN + "|" + name.getCharacters().toString();

            try {
                message.writeSomething(loginMessage);
            } catch (IOException e) {
                e.printStackTrace();
            }


            try {

                forParser = message.readSomething();

                if (forParser.equals("ERROR")){
                    primStage.close();
                    return;
                } else if (forParser.split("\\|")[0].equals("RECONNECT")) {
                    names = new String[Integer.parseInt(forParser.split("\\|")[1])];
                    fingers = new int[Integer.parseInt(forParser.split("\\|")[1])];
                    for (int i = 0; i < Integer.parseInt(forParser.split("\\|")[1]); i++) {
                        help = forParser.split("\\|")[i+2];
                        names[i] = help.split("\\+")[0];
                        fingers[i] = Integer.parseInt(help.split("\\+")[1]);

                    }
                    game = new Game(primStage,Integer.parseInt(forParser.split("\\|")[1]),names,fingers,name.getCharacters().toString(),message);
                    switchScene();
                    return;
                } else {
                        a = new Alert(Alert.AlertType.INFORMATION);
                        a.setTitle("Waiting");
                        a.setHeaderText("Wait till game starts!");
                        a.setContentText(forParser);
                        a.getButtonTypes().clear();
                        a.show();
                    }
            } catch (IOException e) {
                e.printStackTrace();
            }

            //Čeká na zprávu od klienta, každých 150ms kontroluje, zda něco přišlo
            Task<Void> sleeper = new Task<>() {
                @Override
                protected Void call() {
                    try {
                        while ((forParser = message.readSomething()).equals("")) {
                            Thread.sleep(150);
                        }

                    } catch (InterruptedException | IOException ignored) {}
                    return null;
                }
            };

            sleeper.setOnSucceeded(event -> {

                a.setResult(ButtonType.CLOSE);
                a.close();

                parsedMessage = forParser.split("\\|");
                int numOfPlay = Integer.parseInt(parsedMessage[1]);
                for (int i = 2; i < numOfPlay+2; i++) {
                    parsedMessage[i-2] = parsedMessage[i];
                }

                game = new Game(primStage,numOfPlay, parsedMessage,name.getCharacters().toString(),message);

                switchScene();
            });
            thread = new Thread(sleeper);
            thread.start();

        });

        vBox.setSpacing(15);
        vBox.setPadding(new Insets(0,0,0,15));
        vBox.setAlignment(Pos.CENTER);
        vBox.getChildren().addAll(name,button);

        return vBox;
    }

    /**
     * Změní okno z intra na hru
     */
    private void switchScene() {
        try {
            primStage.setScene(new Scene(game.createRootPanee(),game.minHeight,game.minWidth));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        primStage.sizeToScene();
        primStage.setMinHeight(game.minHeight);
        primStage.setMinWidth(game.minWidth);
        primStage.setY(100);
    }
}

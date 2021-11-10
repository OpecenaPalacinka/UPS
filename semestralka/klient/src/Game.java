import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Stage;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

public class Game {

    Intro intro;
    Stage primStage;
    ImageLoader imageLoader = new ImageLoader();


    int numberOfPlayers;

    final BooleanProperty booleanProperty = new SimpleBooleanProperty(true);

    int minHeight = 880;
    int minWidth = 650;

    public Game(Stage stage, int numOfPlayers) throws FileNotFoundException {
        primStage = stage;
        numberOfPlayers = numOfPlayers;
    }


    public Parent createRootPanee() throws FileNotFoundException {
        BorderPane rootPane = new BorderPane();

        //rootPane.setTop(createTopName());

        ImageView[] imageViews = imageLoader.loadImages(numberOfPlayers);

        switch (numberOfPlayers){
            case 2:
                rootPane.setCenter(createTopCenter(imageViews[0]));
                rootPane.setBottom(createBottom(imageViews[1]));
                break;
            case 3:
                rootPane.setCenter(createTopCenter(imageViews[0]));
                rootPane.setBottom(createBottom(imageViews[1]));
                rootPane.setRight(createRight(imageViews[2]));
                break;
            case 4:
                rootPane.setCenter(createTopCenter(imageViews[0]));
                rootPane.setBottom(createBottom(imageViews[1]));
                rootPane.setRight(createRight(imageViews[2]));
                rootPane.setLeft(createLeft(imageViews[3]));
                break;
        }



        return rootPane;
    }

    private ImageView formatThumbsUp(ImageView thumbsUp){
        thumbsUp.setFitHeight(40);
        thumbsUp.setFitWidth(40);
        return thumbsUp;
    }

    private Node createTopName() {
        VBox welcoming = new VBox();
        Label welcome = new Label("Čiim Čoong!");
        welcome.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,25));
        welcoming.setAlignment(Pos.TOP_LEFT);
        welcoming.setPadding(new Insets(25,0,25,0));
        welcoming.getChildren().add(welcome);
        return welcoming;
    }

    private Node createTopCenter(ImageView imageView) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        VBox main = new VBox();
        Label nameOfPlayer = new Label("Hráč nahoře");
        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage("thumb_up_left.png"));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage("thumb_up_right.png"));
        TextField fingers = new TextField();
        fingers.setMaxWidth(50);
        hBox.getChildren().addAll(thumbsUpLeft,thumbsUpRight);
        hBox.setAlignment(Pos.TOP_CENTER);
        hBox.setSpacing(5);
        hBox.setPadding(new Insets(15,0,0,0));
        vBox.getChildren().addAll(hBox,imageView,fingers,nameOfPlayer);
        vBox.setAlignment(Pos.TOP_CENTER);
        vBox.setSpacing(10);



        VBox guessBox = new VBox();
        Label guess = new Label("Tvůj tip");
        TextField guessTF = new TextField();

        //ABY TEXTFIELD NEMĚL FOCUS
        guessTF.focusedProperty().addListener((observable,  oldValue,  newValue) -> {
            if(newValue && booleanProperty.get()){
                vBox.requestFocus(); // Delegate the focus to container
                booleanProperty.setValue(false); // Variable value changed for future references
            }
        });

        // NĚJAKÉ VLÁKNO Z STACKUOVERFLOW
        // ZDROJ MOJE SEMESTRÁLKA UUR (Nemám páru jak to funguje)
        guessTF.textProperty().addListener((observableValue, s, t1) -> {
            if (!t1.matches("\\d*")) {
                guessTF.setText(t1.replaceAll("[^\\d]", ""));
            }
            try {
                if(Integer.parseInt(t1) > numberOfPlayers * 2){
                    guessTF.setText(s);
                }
            } catch (Exception ignored){
            }

        });

        guess.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,17));
        guessTF.setMaxWidth(100);
        guessBox.setPadding(new Insets(130,0,0,0));
        guessBox.setSpacing(10);
        guessBox.getChildren().addAll(guess,guessTF);
        guessBox.setAlignment(Pos.CENTER);

        main.getChildren().addAll(vBox,guessBox);

        return main;
    }

    private Node createBottom(ImageView imageView) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label("Hráč dole");

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage("thumb_up_left.png"));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage("thumb_up_right.png"));
        TextField fingers = new TextField();
        fingers.setMaxWidth(50);
        hBox.getChildren().addAll(thumbsUpLeft,thumbsUpRight);
        hBox.setAlignment(Pos.BOTTOM_CENTER);
        hBox.setSpacing(5);
        nameOfPlayer.setPadding(new Insets(0,0,35,0));
        vBox.getChildren().addAll(hBox,imageView,fingers,nameOfPlayer);
        vBox.setAlignment(Pos.BOTTOM_CENTER);
        vBox.setSpacing(10);
        return vBox;
    }

    private Node createRight(ImageView imageView) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label("Hráč vpravo");
        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage("thumb_up_left.png"));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage("thumb_up_right.png"));
        TextField fingers = new TextField();
        fingers.setMaxWidth(50);
        hBox.getChildren().addAll(thumbsUpLeft,thumbsUpRight);
        hBox.setAlignment(Pos.CENTER_RIGHT);
        hBox.setSpacing(5);
        int padding = imageLoader.widths[0]/2-65/2;
        hBox.setPadding(new Insets(0,padding,0,0));
        nameOfPlayer.setPadding(new Insets(0,padding+10,0,0));
        hBox1.getChildren().add(fingers);
        hBox1.setPadding(new Insets(0,0,0,55));
        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.CENTER_RIGHT);
        vBox.setPadding(new Insets(210,0,0,0));
        vBox.setSpacing(10);
        return vBox;
    }

    private Node createLeft(ImageView imageView) throws FileNotFoundException {
        VBox vBox = new VBox();
        HBox hBox = new HBox();
        HBox hBox1 = new HBox();
        Label nameOfPlayer = new Label("Hráč vlevo");

        ImageView thumbsUpLeft = formatThumbsUp(imageLoader.loadImage("thumb_up_left.png"));
        ImageView thumbsUpRight = formatThumbsUp(imageLoader.loadImage("thumb_up_right.png"));
        TextField fingers = new TextField();
        fingers.setMaxWidth(50);
        hBox.getChildren().addAll(thumbsUpLeft,thumbsUpRight);
        hBox.setAlignment(Pos.CENTER_LEFT);
        hBox.setSpacing(5);
        int padding = imageLoader.widths[0]/2-65/2;
        hBox.setPadding(new Insets(0,0,0,padding));
        nameOfPlayer.setPadding(new Insets(0,0,0,padding+10));
        hBox1.getChildren().add(fingers);
        hBox1.setPadding(new Insets(0,0,0,padding+10));
        vBox.getChildren().addAll(hBox,imageView,hBox1,nameOfPlayer);
        vBox.setAlignment(Pos.CENTER_LEFT);
        vBox.setPadding(new Insets(210,0,0,0));
        vBox.setSpacing(10);
        return vBox;
    }

    public void EndOfGame(){
        intro = new Intro(primStage);
        primStage.setScene(new Scene(intro.createRootPane(),intro.minWidth, intro.minHeight));
        primStage.sizeToScene();
        primStage.setMinHeight(intro.minHeight);
        primStage.setMinWidth(intro.minWidth);
    }

}

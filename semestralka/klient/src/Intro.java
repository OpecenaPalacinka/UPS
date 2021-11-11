import javafx.application.Preloader;
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
import javafx.scene.control.Tooltip;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Stage;
import javafx.stage.Window;

import java.io.FileNotFoundException;
import java.io.IOException;

public class Intro {
    Game game;
    Gamer gamer;
    Message message = new Message();
    Stage primStage;
    Parser parser = new Parser();

    String loginMessage;

    int minWidth =  550;
    int minHeight = 350;

    final BooleanProperty booleanProperty = new SimpleBooleanProperty(true);

    public Intro(Stage stage) {
        primStage = stage;
    }

    public Parent createRootPane(){
        BorderPane rootPane = new BorderPane();

        rootPane.setTop(createTop());
        rootPane.setCenter(createCenter());

        return rootPane;
    }

    private Node createTop() {
        VBox welcoming = new VBox();
        Label welcome = new Label("Vítejte ve hře Čim Čong!");
        welcome.setFont(Font.font("Verdana", FontWeight.BOLD, FontPosture.REGULAR,25));
        welcoming.setAlignment(Pos.TOP_CENTER);
        welcoming.setPadding(new Insets(25,0,25,0));

        welcoming.getChildren().add(welcome);
        return welcoming;
    }



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

        button.setOnAction(mouseEvent -> {

            loginMessage = CommandsClient.LOGIN + "|" + name.getCharacters().toString();
            System.out.println(loginMessage);
            parser.parse("ENDGAME|ks");

            /*
            try {
                message.init();
            } catch (IOException e) {
                System.err.println("Nepodařilo se navázat spojení se serverem! Končíme");
                System.exit(0);
            }

             */

            gamer = new Gamer(name.getCharacters().toString());

            try {
                game = new Game(primStage,4,gamer);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            try {
                primStage.setScene(new Scene(game.createRootPanee(),game.minHeight,game.minWidth));
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            primStage.sizeToScene();
            primStage.setMinHeight(game.minHeight);
            primStage.setMinWidth(game.minWidth);
            primStage.setY(100);

        });


        vBox.setSpacing(15);
        vBox.setPadding(new Insets(0,0,0,15));
        vBox.setAlignment(Pos.CENTER);
        vBox.getChildren().addAll(name,button);

        return vBox;
    }
}

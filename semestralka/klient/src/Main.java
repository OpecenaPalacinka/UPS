import javafx.application.Application;
import javafx.application.Platform;
import javafx.application.Application;
import javafx.event.Event;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Stage;

import java.io.IOException;

public class Main extends Application {

    private Stage mainStage;
    Intro intro;

    @Override
    public void start(Stage primaryStage) throws Exception{

        mainStage = primaryStage;

        intro = new Intro(mainStage);

        Scene scene = new Scene(intro.createRootPane());

        mainStage.setTitle("Čim Čong");

        mainStage.setScene(scene);
        mainStage.setMinHeight(350);
        mainStage.setMinWidth(550);

        mainStage.show();

        mainStage.setOnCloseRequest(event -> {
            System.exit(0);
        });
    }

    public static void main(String[] args) {

        launch(args);
    }

    public Stage getMainStage() {
        return mainStage;
    }

    public void setMainStage(Stage mainStagee) {
        mainStage = mainStagee;
    }
}

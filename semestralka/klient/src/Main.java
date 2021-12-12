import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Main extends Application {

    private Stage mainStage;
    Intro intro;

    @Override
    public void start(Stage primaryStage) {

        mainStage = primaryStage;

        intro = new Intro(mainStage,new Message(false));


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

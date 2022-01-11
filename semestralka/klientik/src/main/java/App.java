import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class App extends Application {

    private Stage mainStage;
    Intro intro;
    static String[] arg;

    /**
     * Vytvoří úvodní okno hry
     * @param primaryStage Stage, do které se vykresluje
     */
    @Override
    public void start(Stage primaryStage) {

        mainStage = primaryStage;

        intro = new Intro(mainStage,new Message(false),arg[0], Integer.parseInt(arg[1]));

        Scene scene = new Scene(intro.createRootPane());

        mainStage.setTitle("Čim Čong");

        mainStage.setScene(scene);
        mainStage.setMinHeight(350);
        mainStage.setMinWidth(550);

        mainStage.show();

        mainStage.setOnCloseRequest(event -> System.exit(0));
    }

    /**
     * Zkontroluje parametry a pokud je vše v pořádku, spustí okno
     * @param args IP adresa a port
     */
    public static void main(String[] args) {
        if (args.length == 2){
            if (!isNumeric(args[1]) ||
                    Integer.parseInt(args[1]) > 65535 ||
                    Integer.parseInt(args[1]) < 0){
                System.err.println("Chybně zvolený port!");
                System.exit(1);
            }
            arg = args;
            launch(args);
        } else {
            System.err.println("Zadal jste příliš mnoho, nebo málo parametrů! Zadejte IP adresu a port!");
        }

    }

    /**
     * Kontroluje zda je string číslem
     * @param str Kontrolovaný string
     * @return True pokud string je číslo, false když není
     */
    public static boolean isNumeric(String str) {
        try {
            Integer.parseInt(str);
            return true;
        } catch(NumberFormatException e){
            return false;
        }
    }
}

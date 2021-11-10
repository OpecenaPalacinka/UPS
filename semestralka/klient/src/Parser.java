import java.util.Arrays;

public class Parser {

    public void parse(String message){
        String command = message.split("\\|")[0];

        switch (command) {
            case "START":
                System.out.println("Loginek");
                break;
            case "GUESS":
                System.out.println("GUESS");
                break;
            case "ENDOFROUND":
                System.out.println("ENDOFROUND");
                break;
            case "ENDGAME":
                System.out.println("ENDGAME");
                break;
            default:
                System.out.println("Nenene");
        }

    }
}

public enum CommandsServer {
    START ("START"),
    GUESS ("GIVEGUESS"),
    ENDOFROUND ("ENDOFROUND"),
    ENDGAME ("ENDGAME");

    private final String command;

    CommandsServer(String str){
        command = str;
    }

    @Override
    public String toString() {
        return this.command;
    }
}

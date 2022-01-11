public enum CommandsClient {
    LOGIN ("LOGIN"),
    GUESS ("GUESS"),
    FINGERS ("FINGERS");

    private final String command;

    CommandsClient(String str){
        command = str;
    }

    @Override
    public String toString() {
        return this.command;
    }
}

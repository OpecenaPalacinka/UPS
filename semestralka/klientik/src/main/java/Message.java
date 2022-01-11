import java.io.*;
import java.net.InetAddress;
import java.net.Socket;

public class Message {

    BufferedReader reader;
    PrintWriter writer;
    Socket socket;
    boolean connected;
    String sentence;

    public Message(boolean connect){
        connected = connect;
    }

    /**
     * Inicializuje spojení mezi serverem a klientem
     * @param ip IP adresa serveru
     * @param port port server
     * @throws IOException Nepodařilo se připojit
     */
    public void init(String ip, int port) throws IOException {
        socket = new Socket(ip,port);
        InetAddress adresa = socket.getInetAddress();
        System.out.print("Pripojuju se na : " + adresa.getHostAddress() + " se jmenem : " + adresa.getHostName() + "\n");
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
        connected = true;
    }

    /**
     * Odešle zprávu na server
     * @param sentences odesílaná zpráva
     * @throws IOException Nepodařilo se odeslat
     */
    public void writeSomething(String sentences) throws IOException {

        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
        writer.write(sentences + "\n");
        writer.flush();
    }

    /**
     * Čte zprávu ze input streamu
     * @return Vrací přečtenou zprávu
     * @throws IOException Nic nepřišlo
     */
    public String readSomething() throws IOException {
        sentence = "";

        try {
            sentence = reader.readLine();
            if (sentence == null){
                return "";
            }
            System.out.println("Přišla zpráva -> "+sentence);
            return sentence;

        } catch (Exception e){
            System.err.println("problémek bruh.");
            sentence = "Nic se neposlalo!!";
            return sentence;
        }
    }


}

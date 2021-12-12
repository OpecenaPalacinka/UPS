import java.io.*;
import java.net.InetAddress;
import java.net.Socket;

public class Message {

    BufferedReader reader;
    PrintWriter writer;
    Socket socket;
    boolean connected;

    public Message(boolean connect){
        connected = connect;
    }

    public void init() throws IOException {
        socket = new Socket("127.0.0.1",10000);
        InetAddress adresa = socket.getInetAddress();
        System.out.print("Pripojuju se na : " + adresa.getHostAddress() + " se jmenem : " + adresa.getHostName() + "\n");
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
        connected = true;
    }

    public void writeSomething(String sentence) throws IOException {

        System.out.println("Message for server -> "+sentence);
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
        writer.write(sentence + "\n");
        writer.flush();

    }

    public String readSomething() {
        String sentence = "";
        try {
            sentence = reader.readLine();
            return sentence;

        } catch (Exception e){
            sentence = "Nic se neposlalo!!";
        }
        return sentence;
    }


}

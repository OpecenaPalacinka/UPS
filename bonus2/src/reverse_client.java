import java.io.*;
import java.net.*;

public class reverse_client {

    public static void main(String[] args) throws Exception {
        String sentence;
        StringBuffer stringBuffer;

        BufferedReader reader;
        PrintWriter writer;

        Socket socket = new Socket("34.122.204.187", 2002);
        InetAddress adresa = socket.getInetAddress();
        System.out.print("Pripojuju se na : " + adresa.getHostAddress() + " se jmenem : " + adresa.getHostName() + "\n");

        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

        sentence = reader.readLine();
        System.out.println("Message Received: " + sentence);

        stringBuffer = new StringBuffer(sentence);
        stringBuffer.reverse();

        writer.write(stringBuffer + "\n");
        writer.flush();

        System.out.println("Reversed: "+stringBuffer);

        while ((sentence = reader.readLine()) != null) {
            System.out.println("Answer: " + sentence);
        }


        writer.close();
        reader.close();
        socket.close();
    }
}

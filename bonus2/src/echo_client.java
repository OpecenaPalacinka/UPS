import java.io.*;
import java.net.*;
import java.util.Scanner;

public class echo_client {

    public static void main(String[] args) throws Exception {

        Scanner scanner = new Scanner(System.in);
        String sentence;

        BufferedReader reader;
        PrintWriter writer;

        Socket socket = new Socket("34.122.204.187", 2001);
        InetAddress adresa = socket.getInetAddress();
        System.out.print("Pripojuju se na : "+adresa.getHostAddress()+" se jmenem : "+adresa.getHostName()+"\n" );

        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

        System.out.print("Write a message for server -> ");
        writer.write(scanner.nextLine()+"\n");
        writer.flush();

        while ((sentence = reader.readLine()) != null){
            System.out.println("Message Received: " + sentence);
        }

        writer.close();
        reader.close();
        socket.close();
    }
}

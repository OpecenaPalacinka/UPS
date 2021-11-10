import java.io.*;
import java.net.*;
import java.util.Scanner;

public class calc_client {

    public static void main(String[] args) throws Exception {

        String sentence;

        Scanner scanner = new Scanner(System.in);

        BufferedReader reader;
        PrintWriter writer;

        Socket socket = new Socket("34.122.204.187", 2000);
        InetAddress adresa = socket.getInetAddress();
        System.out.print("Pripojuju se na : " + adresa.getHostAddress() + " se jmenem : " + adresa.getHostName() + "\n");

        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

        for (int i = 0; i < 3; i++) {
            sentence = reader.readLine();
            System.out.println(sentence);
        }

        String command = scanner.nextLine();

        writer.write(command+"\n");
        writer.flush();

        String[] commands = command.split("\\|");
        String choose = commands[0];
        int num1 = Integer.parseInt(commands[1]);
        int num2 = Integer.parseInt(commands[2]);

        switch (choose) {
            case "plus" -> System.out.println("Můj výsledek: " + (num1 + num2));
            case "minus" -> System.out.println("Můj výsledek: " + (num1 - num2));
            case "division" -> System.out.println("Můj výsledek: " + (num1 / num2));
            case "multiply" -> System.out.println("Můj výsledek: " + (num1 * num2));
            default -> System.out.println("Illigal Operation");
        }

        while ((sentence = reader.readLine()) != null) {
            System.out.println("Answer: " + sentence);
        }

        writer.close();
        reader.close();
        socket.close();
    }
}

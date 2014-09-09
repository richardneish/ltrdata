import java.net.*;
import java.io.*;

public class SocketServer{
  static void handle(Socket client){
    try{
      BufferedReader recieve =
        new BufferedReader( new InputStreamReader ( client.getInputStream()));
        DataOutputStream send = new DataOutputStream( client.getOutputStream());
        while (true){
          String in = recieve.readLine();
          String out = in.toUpperCase() + "\n";
          send.writeBytes(out);
        }
    }
    catch (IOException e){
      // Avsluta vid förbindelseproblem (exempelvis att
      // klienten avslutar förbindelsen)
    }
  }

  public static void main(String args[]){
    ServerSocket serversocket;
    Socket client;
    try{
      serversocket = new ServerSocket(4001); //portnummer
      System.out.println("Server startad...");
      while (true){
        client = serversocket.accept();
        System.out.println("Klient mottagen!");
        handle(client);
        System.out.println("Klient hanterad!");
      }
    }
    catch (IOException e){
      System.out.println("Fel vid skapande av socket!");
    }
  }
}
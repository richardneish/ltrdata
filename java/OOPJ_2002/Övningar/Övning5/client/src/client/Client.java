package client;

import java.net.*;
import java.io.*;

public class Client {
  private Socket s;
  private DataOutputStream dos;

  Client(String host, int port){
    try{
      s = new Socket(host, port);
      dos = new DataOutputStream(s.getOutputStream());
      dos.writeUTF("Hello World!");
      if(disconnect)
        s.close();
    }

    catch(IOException e){
      System.out.println(e.getMessage());
    }
  }

  public static void main(String[] args) {
    new Client("127.0.0.1", 5000);
  }
}
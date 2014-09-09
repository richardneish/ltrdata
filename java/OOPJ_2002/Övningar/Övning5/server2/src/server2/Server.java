package server2;

import java.io.*;
import java.net.*;

public class Server {
  private ServerSocket ss;

  Server(int port){
    listen(port);
  }

  public void listen(int port){
    try {
      ss = new ServerSocket(5000);
    }
    catch (IOException ex) {}

    while(true)
    try{
      new ServerThread(ss.accept());
    }
    catch(IOException e){}
  }

  public static void main(String[] args) {
    new Server(5000);
  }
}

class ServerThread extends Thread{
  private Socket s;
  private DataInputStream di;

  ServerThread(Socket s){
    this.s = s;
    try{
      di = new DataInputStream(s.getInputStream());
    }
    catch(IOException e) {}
    start();
  }

  public void run(){
    while(true)
      try{
        System.out.println(di.readUTF());
      }
      catch(IOException e){
        System.out.println("Klient kopplade ifrån");
        break;
      }
      try {s.close();} catch(IOException e){}
  }
}
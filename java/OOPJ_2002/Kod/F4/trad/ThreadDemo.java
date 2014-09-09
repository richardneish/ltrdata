/* Exemplet visar vad som kan hända då två trådar använder samma metod.*/
import java.io.*;

class ThreadDemo implements Runnable{
  private String name;
  private CommunicationChannel cc;

  ThreadDemo(String n, CommunicationChannel channel){
    name = n;
    cc = channel;
  }

  public void run(){
    for (int i = 0; i < 10; i++){
      cc.send(name,"Hej "+name+"!");
    }
   }

  public static void main(String[] arg){
    CommunicationChannel cc = new CommunicationChannel();
      Thread t1 = new Thread(new ThreadDemo("A",cc));
      Thread t2 = new Thread(new ThreadDemo("B",cc));
      t1.start();
      t2.start();
      try{
        System.in.read();
      }
      catch (Exception e){}
      t1.stop();
      t2.stop();
   }
}

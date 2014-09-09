package thrconsole;

public class Main {
  public static void main(String[] args) {
    Räknare r1 = new Räknare();
    r1.aktivitet.start();
    Räknare r2 = new Räknare();
    r2.aktivitet.start();
    Räknare r3 = new Räknare();
    r3.aktivitet.start();
  }
}

class Räknare implements Runnable{
  public Thread aktivitet = new Thread(this);
  private int count;

  public void run(){
    while(true){
      try{
        Thread.sleep(1000);
      }catch(InterruptedException ie){}
      count++;
      System.out.println(count);
    }
  }
}
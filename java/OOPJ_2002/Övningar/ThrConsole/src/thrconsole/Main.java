package thrconsole;

public class Main {
  public static void main(String[] args) {
    R�knare r1 = new R�knare();
    r1.aktivitet.start();
    R�knare r2 = new R�knare();
    r2.aktivitet.start();
    R�knare r3 = new R�knare();
    r3.aktivitet.start();
  }
}

class R�knare implements Runnable{
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
/*Exemplet visar ett program som exekverar i två trådar.
  Kalle Anka och Fantomen är skyddshelgon för Elektro resp. Fysik
  på CTH (usch och fy)*/

class Trad extends Thread{

  public void run(){
    try{
      for(int i=0;i<15;i++){
        System.out.println("Banka Anka");
        sleep(100);
      }
    }
    catch(InterruptedException e){}
  }

  public static void main(String[] args)throws java.io.IOException{
    Trad t=new Trad();
    t.start();
    try{
      for(int i=0;i<25;i++){
        System.out.println("Fantis Klantis");
        sleep(100);
      }
    }
    catch(InterruptedException e){}
    System.in.read();
  }
}


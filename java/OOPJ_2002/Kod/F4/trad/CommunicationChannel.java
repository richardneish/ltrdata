public class CommunicationChannel extends Thread{
   // L�gg till synchronized f�re void och notera effekt!
  public void send(String rec, String mess){
    System.out.print("Skickar till:" + rec);
    System.out.println(" Meddelande:" + mess);
	}
}


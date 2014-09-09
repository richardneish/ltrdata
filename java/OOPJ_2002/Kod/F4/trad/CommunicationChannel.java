public class CommunicationChannel extends Thread{
   // Lägg till synchronized före void och notera effekt!
  public void send(String rec, String mess){
    System.out.print("Skickar till:" + rec);
    System.out.println(" Meddelande:" + mess);
	}
}


import java.awt.*;
import java.applet.*;
import java.net.*;
import java.io.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Socketklient extends Applet implements ActionListener {

   Button btnServerConnect= new Button("Kontakta Server");
   Button btnServerDisconnect = new Button("Avbryt Förbindelse");
   TextField port = new TextField("Skriv port-nummer");
   TextField ip = new TextField("Skriv IP-adress");
   Button btnSend= new Button("Sänd");
   TextField fldSend= new TextField();
   TextField fldRecieve= new TextField();
   Label lblConnectionStatus= new Label("Ingen förbindelse...");
   Socket socket;
   DataOutputStream send;
   BufferedReader recieve;

   public void init() {
      super.init();
      setLayout(new GridLayout(10,1));
      add(ip);
      add(port);
      add(btnServerConnect);
      add(lblConnectionStatus);
      Label label1 = new Label("Att sända:",Label.CENTER);
      add(label1);
      add(fldSend);
      add(btnSend);
      Label label2 = new Label("Meddelanden:",Label.CENTER);
      add(label2);
      add(fldRecieve);
      add(btnServerDisconnect);
      btnServerConnect.addActionListener(this);
      btnServerDisconnect.addActionListener(this);
      btnSend.addActionListener(this);
   }

  public void actionPerformed(ActionEvent event){
    if (event.getSource() == btnServerConnect){
      try{
        socket = new Socket(ip.getText(), Integer.parseInt(port.getText())); //host,port
        send = new DataOutputStream(socket.getOutputStream());
        recieve =  new BufferedReader( new InputStreamReader(
                    socket.getInputStream()));
        lblConnectionStatus.setText("Förbindelse öppen");
      }
      catch (UnknownHostException e){
        lblConnectionStatus.setText("Okänd host");
      }
      catch (IOException e){
        lblConnectionStatus.setText("Ingen kontakt med server");
      }
    }
    if (event.getSource() == btnServerDisconnect){
      try{
        socket.close();
		    send.close();
    		recieve.close();
    		fldSend.setText("");
    		fldRecieve.setText("");
		    lblConnectionStatus.setText("Ingen förbindelse");
      }
      catch (IOException e){}
    }
    if (event.getSource() == btnSend ){
      try{
        String sendString = fldSend.getText();
        send.writeBytes(sendString + "\n");
    		String replyString = recieve.readLine();
    		fldRecieve.setText(replyString);
      }
      catch (IOException e){
		    lblConnectionStatus.setText("Sändningsfel");
      }
    }
  }
}

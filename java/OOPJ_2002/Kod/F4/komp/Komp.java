/*Exemplet visar hur komponenter placeras på appletytan och hur appleten
  hanterar händelser. Notera att appleten här implementerar båda
  gränssnitten runnable (för tråden) och ActionListener (för att kunna
  lyssna på händelser). */

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.text.DateFormat;
import java.util.Date;


public class Komp extends Applet implements ActionListener,Runnable{
  private DateFormat fmt= DateFormat.getDateTimeInstance(
                          DateFormat.LONG,DateFormat.MEDIUM);
  private Thread t;
  private Label timeLabel;               //deklarera label
  private Button startButton,stopButton; //och knappar

  private String getTimeStamp(){ //privat metod
    Date current=new Date();
    String s=fmt.format(current);
    return "Tid: "+s;
  }

  public void init(){
    setLayout(new FlowLayout()); //default men ändå
    timeLabel=new Label(getTimeStamp()); //skapa label
    add(timeLabel);                      //lägg till (till appleten)
    startButton=new Button("Starta");
    startButton.addActionListener(this); //appleten lyssnar
    add(startButton);
    stopButton=new Button("Stanna");
    stopButton.addActionListener(this);
    add(stopButton);
  }

  public void start(){
    t=new Thread(this);
    t.start();
  }

  public void stop(){
    if(t!=null)
      t.stop();
    t=null;
  }

  public void run(){
    while(t!=null){
      try{
        Thread.sleep(100);
      }
      catch(InterruptedException e){}
      timeLabel.setText(getTimeStamp()); //uppdatera labeln
      repaint();
    }
  }

  public void actionPerformed(ActionEvent e){//appleten själv hanterar
    Object source=e.getSource();
    if(source==startButton)
      t.resume();
    else
      if(source==stopButton)
        t.suspend();
  }
}

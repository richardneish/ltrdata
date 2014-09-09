/*Exemplet visar dubbelbuffring, d.v.s. hur vi först tillverkar en
  bild utanför skärmen och sedan ritar hela bilden på en gång. */

import java.applet.Applet;
import java.awt.*;
import java.text.DateFormat;
import java.util.Date;

public class Dubbelbuff extends Applet implements Runnable{
  private DateFormat fmt=DateFormat.getDateTimeInstance(
                          DateFormat.LONG,DateFormat.MEDIUM);
  private Font font=new Font("SansSerif",Font.BOLD,40);
  private Thread t;
  private Image offscreenImage;          //för dubbelbuffring
  private Graphics offscreenGraphics;   //för dubbelbuffring

  public void init(){
    Dimension mySize=getSize();//Appletens storlek
    offscreenImage=createImage(mySize.width,mySize.height);
    offscreenGraphics=offscreenImage.getGraphics();
  }

  public void start(){
    t=new Thread(this);
    t.start();
  }

  public void update(Graphics g){//Omdefinierad för att hindra aut. radering
    paint(g);
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
      repaint();
    }
  }

  public void paint(Graphics g){  
    paintFrame(offscreenGraphics);  //Tillverkar först bilden...
    g.drawImage(offscreenImage,0,0,null);  //...ritar den sedan
  }

  public void paintFrame(Graphics g){ //g är kopplat till bilden offscreen
    Date current=new Date();
    String s=fmt.format(current);
    Dimension d=getSize();
    g.setColor(getBackground());
    g.fillRect(0,0,d.width,d.height); //fyll med bakgrundfärg=radering
    g.setColor(getForeground());
    g.setFont(font);
    g.drawString("tid: "+s,50,50);
  }
}
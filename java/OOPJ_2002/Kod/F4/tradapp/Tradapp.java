import java.applet.Applet;
import java.awt.*;
import java.text.DateFormat;
import java.util.Date;

public class Tradapp extends Applet implements Runnable{
  private DateFormat fmt=DateFormat.getDateTimeInstance(
                          DateFormat.LONG,DateFormat.MEDIUM);
  private Font font=new Font("SansSerif",Font.BOLD,40);
  private Thread t;


  public void init(){
  }

  public void start(){
    t=new Thread(this);
    t.start();
  }

  //Försök att minska flimret genom att omdefinera update
 /* public void update(Graphics g){
    paint(g);
  }            */

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
    Date current=new Date();
    String s=fmt.format(current);
    g.setFont(font);
    g.drawString("tid: "+s,50,50);
  }
}

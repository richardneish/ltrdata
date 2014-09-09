import java.awt.*;
import java.applet.*;
import java.util.*;

public class ThreadApplet extends Applet{
    private static Random r = new Random();
    private Thread t1;
    private Thread t2;
    private Thread t3;
    public int x,y;
    public Color c;

    private int getRandCoord(int maxInt){
      return (Math.abs(r.nextInt())% maxInt + 1);
    }

    public void update(Graphics gr){
      gr.setColor(c);
      gr.fillOval(x,y,10,10);
    }

    public void init(){
      super.init();
      t1 = new Thread( new GraphicThread( this, new Color(255,0,0),
          getRandCoord(250), getRandCoord(250)) );
        t2 = new Thread( new GraphicThread( this, new Color(0,255,0),
          getRandCoord(250), getRandCoord(250)) ) ;
        t3 = new Thread( new GraphicThread( this, new Color(0,0,255),
          getRandCoord(250), getRandCoord(250)) );
    }

    public void start(){
      if (t1.isAlive())
        t1.resume();
      else
        t1.start();
      if (t2.isAlive())
        t2.resume();
      else
        t2.start();
      if (t3.isAlive())
        t3.resume();
      else
        t3.start();
    }

    public void stop(){
        t1.suspend();
        t2.suspend();
        t3.suspend();
    }

    public void destroy(){
        t1.interrupt();
        t2.interrupt();
        t3.interrupt();
        t1 = null;
        t2 = null;
        t3 = null;
    }
}

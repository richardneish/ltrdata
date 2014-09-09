package thrapplet;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet {
  public void init(){
    setLayout(new GridLayout(2, 2));

    R�knare r1 = new R�knare(Color.green, 1);
    add(r1);
    r1.aktivitet.start();

    R�knare r2 = new R�knare(Color.blue, 2);
    add(r2);
    r2.aktivitet.start();

    R�knare r3 = new R�knare(Color.yellow, 5);
    add(r3);
    r3.aktivitet.start();

    R�knare r4 = new R�knare(Color.orange,3);
    add(r4);
    r4.aktivitet.start();
  }
}

class R�knare extends Canvas implements Runnable{
  public Thread aktivitet = new Thread(this);
  private int count, delay;

  R�knare(Color c, int delay){
    this.delay = delay * 0;
    setBackground(c);
    setVisible(true);
  }

  public void paint(Graphics g){
    g.setFont(new Font("SansSerif", Font.BOLD, 24));
    g.drawString(Integer.toString(count), getWidth()/2, getHeight()/2);
  }

  public void run(){
    while(true){
      try{
        Thread.sleep(delay);
      }catch (InterruptedException ie){}
      count++;
      repaint();
    }
  }
}
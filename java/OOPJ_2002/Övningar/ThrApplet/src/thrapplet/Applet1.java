package thrapplet;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet {
  public void init(){
    setLayout(new GridLayout(2, 2));

    Räknare r1 = new Räknare(Color.green, 1);
    add(r1);
    r1.aktivitet.start();

    Räknare r2 = new Räknare(Color.blue, 2);
    add(r2);
    r2.aktivitet.start();

    Räknare r3 = new Räknare(Color.yellow, 5);
    add(r3);
    r3.aktivitet.start();

    Räknare r4 = new Räknare(Color.orange,3);
    add(r4);
    r4.aktivitet.start();
  }
}

class Räknare extends Canvas implements Runnable{
  public Thread aktivitet = new Thread(this);
  private int count, delay;

  Räknare(Color c, int delay){
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
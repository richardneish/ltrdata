import java.awt.*;
import java.util.*;

public class GraphicThread implements Runnable{
  final int downRightDirection = 1;
  final int upRightDirection = 2;
  final int upLeftDirection = 3;
  final int downLeftDirection = 4;

  private ThreadApplet papa;
  private Dimension r;
  private Color cr;
  private int x;
  private int y;
  private int direction;
    
  GraphicThread(ThreadApplet parent, Color c,  int startx, int starty){
    papa=parent;
    cr = c;
    x = startx;
    y = starty;
    direction = ((int)(Math.random()*4)) + 1;
  }

  void sleep(int msec){
    try{
      Thread.sleep(msec);
    }
    catch (InterruptedException e){}
  }

   void handleCoord(){
    switch (direction){
      case downRightDirection :
        x++; y++;
        if (x >= r.width-10)
          direction = downLeftDirection;
        if (y >= r.height-10)
          direction = upRightDirection;
        break;
      case downLeftDirection :
        x--; y++;
        if (x <= 0)
          direction = downRightDirection;
        if (y >= r.height-10)
          direction = upLeftDirection;
        break;
      case upLeftDirection :
        x--; y--;
        if (x <= 0)
          direction = upRightDirection;
        if (y <= 0)
          direction = downLeftDirection;
        break;
      case upRightDirection :
        x++; y--;
        if (x >= r.width-10)
          direction = upLeftDirection;
        if (y <= 0)
          direction = downRightDirection;
        break;
      }
    }

    public void run(){
        while (true){
          r = papa.getSize();
          sleep(10);
          handleCoord();
          synchronized(papa){
            papa.x = x;
            papa.y = y;
            papa.c = cr;
            papa.repaint(x,y,20,20);
            sleep(5);
          }
          if (Thread.interrupted())
            break;
        }
    }
}
package click;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet{
  public void init(){
    ColorCanvas c = new ColorCanvas();
    add(c);
    setLayout(new GridLayout(1,1));
    c.setVisible(true);
  }
}

class ColorCanvas extends Canvas{
  ColorCanvas(){
    setBackground(Color.blue);
    addMouseListener(l);
  }

  MouseAdapter l = new MouseAdapter(){
    public void mouseClicked(MouseEvent e){
      System.out.println("Hej hej");
      if(getBackground() == Color.blue)
        setBackground(Color.yellow);
      else
        setBackground(Color.blue);
    }
  };
}
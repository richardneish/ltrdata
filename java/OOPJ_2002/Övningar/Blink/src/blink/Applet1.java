package blink;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import javax.swing.*;

public class Applet1 extends Applet implements ActionListener {

  public void init(){
    setBackground(Color.yellow);
    javax.swing.Timer t = new Timer(1000, this);
    t.start();
  }

  public void actionPerformed(ActionEvent e){
    if(getBackground() == Color.blue)
      setBackground(Color.yellow);
    else
      setBackground(Color.blue);
  }
}
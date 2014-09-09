package testapplet;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet {
  public void init(){
    Label l = new Label("Välkommen till Java Direkt", Label.CENTER);
    add(l);
    l.setBackground(Color.yellow);
    l.setForeground(Color.blue);
    l.setFont(new Font("SansSerif", Font.BOLD, 24));
    setLayout(new GridLayout(1, 1));
  }
}
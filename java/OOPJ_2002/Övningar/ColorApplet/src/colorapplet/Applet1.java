package colorapplet;            //Övning 3, uppgift 4

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet implements ActionListener{
  Button blå = new Button("Blå");
  Button gul = new Button("Gul");

  public void init(){
    setBackground(Color.blue);
    blå.addActionListener(this);
    add(blå);
    gul.addActionListener(this);
    add(gul);
  }

  public void actionPerformed(ActionEvent e){
    if(e.getSource() == blå)
      setBackground(Color.blue);
    else
      setBackground(Color.yellow);
  }
}
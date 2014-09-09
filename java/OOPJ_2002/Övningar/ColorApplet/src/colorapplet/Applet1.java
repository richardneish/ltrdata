package colorapplet;            //�vning 3, uppgift 4

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class Applet1 extends Applet implements ActionListener{
  Button bl� = new Button("Bl�");
  Button gul = new Button("Gul");

  public void init(){
    setBackground(Color.blue);
    bl�.addActionListener(this);
    add(bl�);
    gul.addActionListener(this);
    add(gul);
  }

  public void actionPerformed(ActionEvent e){
    if(e.getSource() == bl�)
      setBackground(Color.blue);
    else
      setBackground(Color.yellow);
  }
}
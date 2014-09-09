import java.applet.*;
import java.awt.*;
import java.awt.event.*;

public class Card extends Applet{
  CardLayout c1=new CardLayout();
  Button b;
  Applet me=this;

  public void init(){
    setLayout(c1);
    Panel p1=new Panel(new GridLayout(2,1));
    b=new Button("1");
    b.addActionListener(new AL());
    p1.add(b);
    b=new Button("2"); b.addActionListener(new AL());
    p1.add(b);
    add("A",p1);
    Panel p2=new Panel(new FlowLayout());
    b=new Button("1"); b.addActionListener(new AL());
    p2.add(b);
    b=new Button("2"); b.addActionListener(new AL());
    p2.add(b);
    add("B",p2);
  }

  class AL implements ActionListener{
    public void actionPerformed(ActionEvent e){
      c1.next(me);
    }
  }
}

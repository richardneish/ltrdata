import java.awt.*;
import java.io.*;
import java.awt.event.*;

public class Huvud{
  public static void main(String[] args){
    Frame f=new Frame();
    Xd x=new Xd(f);
    System.out.println("Användaren skrev "+x.val);
  }
}

class Xd extends Dialog implements ActionListener{
  public String val=null;
  Button ok=new Button("OK");
  TextArea t=new TextArea();
  Xd(Frame f){
    super(f,"Write something",true);
    setLayout(new BorderLayout());
    add(new Label("Hey yo",Label.CENTER),BorderLayout.NORTH);
    ok.addActionListener(this);
    add(ok,BorderLayout.SOUTH);
    add(t,BorderLayout.CENTER);
    setSize(200,300);
    setVisible(true);
  }
  public void actionPerformed(ActionEvent e){
    if(e.getSource()==ok){
      val=t.getText();
      dispose();
    }
  }
}




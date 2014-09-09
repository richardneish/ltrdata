import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class TestDialog{
  public static void main(String[] args){
    Frame f=new Frame();
    XDialog xd=new XDialog(f);
    if(xd.val==null)
      System.out.println("Användaren valde inget alternativ");
    else
      System.out.println("Användaren valde alternativ "+xd.val);
  }
}

class XDialog extends Dialog implements ActionListener{
  public String val=null;
  List list=new List();
  Button ok, cancel;
  XDialog(Frame f){
    super(f,"Välj alternativ",true);
    setLayout(new BorderLayout());
    add(new Label("Alternativ",Label.CENTER),BorderLayout.NORTH);
    for(int i=0;i<6;i++)
      list.add("Alt "+i);
    add(list,BorderLayout.CENTER);
    ok=new Button("OK");
    cancel=new Button("Avbryt");
    Panel bp=new Panel(new FlowLayout());
    bp.add(ok);
    bp.add(cancel);
    ok.addActionListener(this);
    cancel.addActionListener(this);
    add(bp,BorderLayout.SOUTH);
    setSize(200,300);
    setVisible(true);
  }
  public void actionPerformed(ActionEvent e){
    if(e.getSource()==ok){
      val=list.getSelectedItem();
      dispose();
    }
    if(e.getSource()==cancel){
      val=null;
      dispose();
    }
  }
}



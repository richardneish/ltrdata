import java.awt.*;
import java.awt.event.*;

public class Meny extends Frame implements ActionListener{
    MenuItem gul=new MenuItem("Gul");
    MenuItem gr�n=new MenuItem("Gr�n");
    MenuItem exit=new MenuItem("Avsluta");

  public Meny(){
    MenuBar mb=new MenuBar();
    Menu iMen= new Menu("Inst�llningar");
    setMenuBar(mb);
    mb.add(iMen);
    iMen.add(gul);
    iMen.add(gr�n);
    iMen.add(exit);
    gul.addActionListener(this);
    gr�n.addActionListener(this);
    exit.addActionListener(this);
    setSize(300,300);
    this.setVisible(true);
  }

  public void actionPerformed(ActionEvent e){
    if(e.getSource()==gul)
      setBackground(Color.yellow);
    if(e.getSource()==gr�n)
      setBackground(Color.green);
    if(e.getSource()==exit){
      dispose();
      System.exit(1);
    }
  }

  public static void main(String[] args){
    Meny m=new Meny();
  }
}







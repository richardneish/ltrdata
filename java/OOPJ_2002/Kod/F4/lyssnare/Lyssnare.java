/*Exemplet visar händelsehantering. En inre klass används som
  lyssnare. Om klasen implementerar gränssnittet MouseListener måste
  alla metoder implementeras. Alternativt används en anonym klass
  som ärver från MouseAdapter*/

import java.applet.Applet;
import java.awt.event.*;
import java.awt.*;

public class Lyssnare extends Applet{
  private int x;
  private int y;

  MouseAdapter MouseCatcher=new MouseAdapter(){ //anonym klass
    public void mouseClicked(MouseEvent e){
      x=e.getX();  //koordinaterna för händelsen
      y=e.getY();
      repaint();
    }
  };


  /*class MouseCatcher implements MouseListener{   //inre klass
    public void mouseClicked(MouseEvent e){
      x=e.getX();  //koordinaterna för händelsen
      y=e.getY();
      repaint();
    }
    public void mousePressed(MouseEvent e){}
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
  }  //slut MouseCatcher-klassem  */
  
  public void init(){
   // addMouseListener(new MouseCatcher()); //lägg till lyssnaren till appleten
    addMouseListener(MouseCatcher);
  }

  public void paint(Graphics g){
    g.drawString("Here you are",x,y);
  }
}



    
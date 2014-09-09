/*Exemplet visar h�ndelsehantering. En inre klass anv�nds som
  lyssnare. Om klasen implementerar gr�nssnittet MouseListener m�ste
  alla metoder implementeras. Alternativt anv�nds en anonym klass
  som �rver fr�n MouseAdapter*/

import java.applet.Applet;
import java.awt.event.*;
import java.awt.*;

public class Lyssnare extends Applet{
  private int x;
  private int y;

  MouseAdapter MouseCatcher=new MouseAdapter(){ //anonym klass
    public void mouseClicked(MouseEvent e){
      x=e.getX();  //koordinaterna f�r h�ndelsen
      y=e.getY();
      repaint();
    }
  };


  /*class MouseCatcher implements MouseListener{   //inre klass
    public void mouseClicked(MouseEvent e){
      x=e.getX();  //koordinaterna f�r h�ndelsen
      y=e.getY();
      repaint();
    }
    public void mousePressed(MouseEvent e){}
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
  }  //slut MouseCatcher-klassem  */
  
  public void init(){
   // addMouseListener(new MouseCatcher()); //l�gg till lyssnaren till appleten
    addMouseListener(MouseCatcher);
  }

  public void paint(Graphics g){
    g.drawString("Here you are",x,y);
  }
}



    
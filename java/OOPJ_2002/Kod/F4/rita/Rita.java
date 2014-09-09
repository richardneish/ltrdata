/*Exemplet visar hur h�ndelsehantering kan ske med inre klasser
  som �rver adapterklasserna.*/

import java.applet.*;
import java.awt.*;
import java.awt.event.*;

public class Rita extends Applet{
	boolean ritar = false;			// Sann d� anv�ndaren h�ller p� att rita
	boolean ritalinje = false;		// Sann d� anv ritar en linje
	Graphics rityta, bildyta;		// Enhetsbeskrivningar att rita p�
	Image bild;						// F�r att spara ritoperationer
	Point forsta, forra, senaste;	// H�ller reda p� var linjen ritas
  Color farg;						// Linjens f�rg
  int r,g,b;

  class MouseEventListener extends MouseAdapter{
    public void mousePressed(MouseEvent e){
      int x=e.getX();
      int y=e.getY();
      forsta = new Point(x, y);		// Anv�nds vid linjeritning
		  forra = new Point(x, y); 		// Anv�nds vid streckritning
		  senaste = new Point(x, y); 		// Anv�nds i b�gge fallen
		// F�rbered f�r ritoperationer
		  rityta = getGraphics();
		  if(e.isMetaDown()){   //h�gerknappen
			  rityta.setXORMode(farg);
			  ritalinje = true;
		  }
      else{
			  rityta.setColor(farg);
			  ritar = true;
		  }
    }

    public void mouseClicked(MouseEvent e){ //byt f�rg slumpat
      r=(int)(Math.random()*256);
      g=(int)(Math.random()*256);
      b=(int)(Math.random()*256);
      farg=new Color(r,g,b);
    }

    public void mouseReleased(MouseEvent e){
      int x=e.getX();
      int y=e.getY();
      if(ritar)
		   	ritar = false;
		  else
        if(ritalinje == true){
			  // Rita ut linjen slutgiltigt
			    rityta.setColor(farg);
		  	  rityta.setPaintMode();
			    rityta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
			  // Rita �ven p� bilden som sparas
          bildyta.setColor(farg);
			    bildyta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
			    ritalinje = false;
		    }
    }
  }

  class MouseMotionListener extends MouseMotionAdapter{
    public void mouseDragged(MouseEvent e) {
      int x=e.getX();
      int y=e.getY();
		  if(ritar == true){
        //Aktualisera ins�ttningspunkten
			  senaste.move(x,y);
			  // Rita linje fr�n f�reg�ende till nuvarande punkt
			  rityta.drawLine(forra.x, forra.y, senaste.x, senaste.y);
			  // Rita �ven p� bild (f�rgen �r satt sedan init( ))
			  bildyta.drawLine(forra.x, forra.y, senaste.x, senaste.y);
			  // Aktualisera startpunkten
			  forra.move(senaste.x, senaste.y);
			  showStatus("Musen befinner sig vid (" + x + ", " + y + ")"
        +" r:"+r+" g:"+g+" b:"+b);
		  }
      else
        if(ritalinje == true){
			    // Sudda ut f�reg�ende linje
			    rityta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
          // Aktualisera ins�ttningspunkten
			    senaste.move(x, y);
			    // Rita aktuell rektangel
			    rityta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
			    showStatus("Linje fr�n (" + forsta.x + ", " + forsta.y +
				  ") till (" + senaste.x + ", " + senaste.y + ")"+" r:"+r+" g:"
          +g+" b:"+b);
		    }
    }

    public void mouseMoved(MouseEvent e){
      int x=e.getX();
      int y=e.getY();
		  showStatus("Musen befinner sig vid (" + x + ", " + y + ")"+" r:"+r+" g:"
          +g+" b:"+b);
		}
  }

  public void init( ){
    addMouseListener(new MouseEventListener());  //appleten lyssnar
    addMouseMotionListener(new MouseMotionListener());
		setBackground(Color.white);//konstant i Color
    r=(int)(Math.random()*256);
    g=(int)(Math.random()*256);
    b=(int)(Math.random()*256);
		farg = new Color(r,g,b);
		// Tillverka en bild till vilken allt som anv�ndaren ritar ska kopieras
		bild = createImage(getSize().width, getSize().height);
		bildyta = bild.getGraphics();
		bildyta.setColor(farg);
	}

  public void paint(Graphics rityta) {
	  rityta.drawImage(bild, 0, 0, null);
  }
} //Slut p� applet-klassen Rita

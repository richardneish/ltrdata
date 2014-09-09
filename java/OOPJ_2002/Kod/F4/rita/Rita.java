/*Exemplet visar hur händelsehantering kan ske med inre klasser
  som ärver adapterklasserna.*/

import java.applet.*;
import java.awt.*;
import java.awt.event.*;

public class Rita extends Applet{
	boolean ritar = false;			// Sann då användaren håller på att rita
	boolean ritalinje = false;		// Sann då anv ritar en linje
	Graphics rityta, bildyta;		// Enhetsbeskrivningar att rita på
	Image bild;						// För att spara ritoperationer
	Point forsta, forra, senaste;	// Håller reda på var linjen ritas
  Color farg;						// Linjens färg
  int r,g,b;

  class MouseEventListener extends MouseAdapter{
    public void mousePressed(MouseEvent e){
      int x=e.getX();
      int y=e.getY();
      forsta = new Point(x, y);		// Används vid linjeritning
		  forra = new Point(x, y); 		// Används vid streckritning
		  senaste = new Point(x, y); 		// Används i bägge fallen
		// Förbered för ritoperationer
		  rityta = getGraphics();
		  if(e.isMetaDown()){   //högerknappen
			  rityta.setXORMode(farg);
			  ritalinje = true;
		  }
      else{
			  rityta.setColor(farg);
			  ritar = true;
		  }
    }

    public void mouseClicked(MouseEvent e){ //byt färg slumpat
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
			  // Rita även på bilden som sparas
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
        //Aktualisera insättningspunkten
			  senaste.move(x,y);
			  // Rita linje från föregående till nuvarande punkt
			  rityta.drawLine(forra.x, forra.y, senaste.x, senaste.y);
			  // Rita även på bild (färgen är satt sedan init( ))
			  bildyta.drawLine(forra.x, forra.y, senaste.x, senaste.y);
			  // Aktualisera startpunkten
			  forra.move(senaste.x, senaste.y);
			  showStatus("Musen befinner sig vid (" + x + ", " + y + ")"
        +" r:"+r+" g:"+g+" b:"+b);
		  }
      else
        if(ritalinje == true){
			    // Sudda ut föregående linje
			    rityta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
          // Aktualisera insättningspunkten
			    senaste.move(x, y);
			    // Rita aktuell rektangel
			    rityta.drawLine(forsta.x, forsta.y, senaste.x, senaste.y);
			    showStatus("Linje från (" + forsta.x + ", " + forsta.y +
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
		// Tillverka en bild till vilken allt som användaren ritar ska kopieras
		bild = createImage(getSize().width, getSize().height);
		bildyta = bild.getGraphics();
		bildyta.setColor(farg);
	}

  public void paint(Graphics rityta) {
	  rityta.drawImage(bild, 0, 0, null);
  }
} //Slut på applet-klassen Rita

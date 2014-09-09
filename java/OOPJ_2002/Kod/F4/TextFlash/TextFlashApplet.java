/*Exemplet visar hur man ritar p� appletytan. Att anropa repaint innifr�n paint
  �r inte riktigt snyggt, men det f�r vara s� innan vi disskuterat tr�dar...*/

import java.awt.*;
import java.applet.*;
import java.util.*;

public class TextFlashApplet extends Applet{
   private String showString;
   private int x;
   private int y;
   private int redc,greenc,bluec;
   private int fontsize;

   static private Random r = new Random();
   static private AudioClip sound;

  public void init(){
    super.init();
    String text = getParameter("TEXT");
    if (text == null)
      text = "<Ingen Text>";
    showString = text;
    sound = getAudioClip(getCodeBase(),"drumrol.au");
	}

   public void paint(Graphics g){
      x = Math.abs(r.nextInt()) % 200;
	    y = Math.abs(r.nextInt()) % 200;
      fontsize = Math.abs(r.nextInt()) % 100 + 12;
	    redc = Math.abs(r.nextInt()) % 256;
	    greenc = Math.abs(r.nextInt()) % 256;  // f�rgerna har
	    bluec = Math.abs(r.nextInt()) % 256;   // v�rde mella 0-255
      Color c = new Color(redc,greenc,bluec);
      g.setColor(c);
      Font f=new Font("Serif",3,fontsize);
      g.setFont(f);
	 	  g.drawString(showString, x, y);
		  sound.play();
      try{
        Thread.sleep(1500); //v�nta 1,5 s
        repaint(); //Rensa ytan i update som sedan anropar paint igen
      }
	    catch (InterruptedException ie){}
   }
}

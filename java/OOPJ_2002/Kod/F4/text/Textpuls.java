/*Exemplet visar hur man kan anv�nda lite grafik i paintmetoden.
  Notera hur objekt av typerna Color, Font och FontMetrics anv�nds*/

import java.applet.*;
import java.awt.*;

public class Textpuls extends Applet {
	String text, hast;
  Font typsnitt;
  FontMetrics typstorl;

	// Anropas av bl�ddraren d� n�gon bes�ker HTML-sidan
	public void init() {
		// H�mta parametrar fr�n HTML-sidan
		text = getParameter("TEXT");
		hast = getParameter("FART");
		// S�tt f�rvalt v�rde om anv�ndaren skulle gl�mma FART
		if(hast == null) hast = "6";
	}

	public void paint(Graphics rita) {
		// Typsnittets storlek f�r ing�ngsv�rdet 4 punkter
		int storlek = 4;
		// Variabler f�r str�ngens position och hela bredd
		int x, y, textbredd;
		int fart = Integer.parseInt(hast);	// Omvandla variabeln hast till int
		int appbredd = getSize().width;		// Ta reda p� �pplets bredd
		int apphojd = getSize().height;		// Ta reda p� �pplets h�jd
    int r=100,g=100,b=100;

		while(true) {
        r=(int)(Math.random()*256);  //slumpf�rg varje varv
        g=(int)(Math.random()*256);
        b=(int)(Math.random()*256);
        do {
        typsnitt = new Font("Helvetica", Font.PLAIN, storlek);
				typstorl = getFontMetrics(typsnitt);
				x = typstorl.stringWidth(text);
				y = typstorl.getHeight();
				rita.setFont(typsnitt);
        rita.setColor(new Color(r,g,b));
        rita.drawString(text, (appbredd - x) / 2, (150 + y) / 2);
        try{Thread.sleep(100);}catch(Exception e){} //lite paus
        rita.setColor(getBackground());  //m�la �ver gamla
        rita.drawString(text, (appbredd - x) / 2, (150 + y) / 2);
        storlek += fart;
			  } while(x < appbredd && storlek > 3);
			// Byt riktning s� att texten krymper varannan v�nda
			fart = -fart;
			// S�tt storleken till n�st st�rsta (minsta) inf�r n�sta v�nda
			storlek += (fart * 2);
		}
	}
}

/*Exemplet visar hur man kan använda lite grafik i paintmetoden.
  Notera hur objekt av typerna Color, Font och FontMetrics används*/

import java.applet.*;
import java.awt.*;

public class Textpuls extends Applet {
	String text, hast;
  Font typsnitt;
  FontMetrics typstorl;

	// Anropas av bläddraren då någon besöker HTML-sidan
	public void init() {
		// Hämta parametrar från HTML-sidan
		text = getParameter("TEXT");
		hast = getParameter("FART");
		// Sätt förvalt värde om användaren skulle glömma FART
		if(hast == null) hast = "6";
	}

	public void paint(Graphics rita) {
		// Typsnittets storlek får ingångsvärdet 4 punkter
		int storlek = 4;
		// Variabler för strängens position och hela bredd
		int x, y, textbredd;
		int fart = Integer.parseInt(hast);	// Omvandla variabeln hast till int
		int appbredd = getSize().width;		// Ta reda på äpplets bredd
		int apphojd = getSize().height;		// Ta reda på äpplets höjd
    int r=100,g=100,b=100;

		while(true) {
        r=(int)(Math.random()*256);  //slumpfärg varje varv
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
        rita.setColor(getBackground());  //måla över gamla
        rita.drawString(text, (appbredd - x) / 2, (150 + y) / 2);
        storlek += fart;
			  } while(x < appbredd && storlek > 3);
			// Byt riktning så att texten krymper varannan vända
			fart = -fart;
			// Sätt storleken till näst största (minsta) inför nästa vända
			storlek += (fart * 2);
		}
	}
}

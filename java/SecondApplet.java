
       // En mer avancerad applet i JAVA 

       // Importera klass f�r grafik
       import java.awt.*;

       // Importera klass f�r applets 
       import java.applet.*;

       public class SecondApplet extends Applet {

       private Font font;

       // Vid initialisering
       public void init() {
       font=new Font("Helvetica", Font.BOLD, 48);
       }

       // Ritar om appleten n�rhelst det beh�vs
       public void paint(Graphics g) {
       // Rita en oval fylld ring
       g.setColor(Color.pink);
       g.fillOval(10, 10, 330, 100);


       // Rita ut en str�ng p� sk�rmen 
       g.setColor(Color.black);
       g.setFont(font);
       g.drawString("Superenkelt!", 40, 75);
       }
       }

/*
       F�ljande HTML-kod medf�r att appleten kan visas via webl�sare:

       <HTML>
       <HEAD>
       <TITLE>En "andra" enkel JAVA-applikation</TITLE>
       </HEAD>
       <BODY>
       <H1>Java Applikation</H1><P>
       Nedanst�ende f�nster inneh�ller en JAVA-Applet.<P>
       <APPLET code=SecondApplet.class width=350 height=120>
       </APPLET>
       <P>
       Slut
       </BODY>
       </HTML>
*/


       // En mer avancerad applet i JAVA 

       // Importera klass för grafik
       import java.awt.*;

       // Importera klass för applets 
       import java.applet.*;

       public class SecondApplet extends Applet {

       private Font font;

       // Vid initialisering
       public void init() {
       font=new Font("Helvetica", Font.BOLD, 48);
       }

       // Ritar om appleten närhelst det behövs
       public void paint(Graphics g) {
       // Rita en oval fylld ring
       g.setColor(Color.pink);
       g.fillOval(10, 10, 330, 100);


       // Rita ut en sträng på skärmen 
       g.setColor(Color.black);
       g.setFont(font);
       g.drawString("Superenkelt!", 40, 75);
       }
       }

/*
       Följande HTML-kod medför att appleten kan visas via webläsare:

       <HTML>
       <HEAD>
       <TITLE>En "andra" enkel JAVA-applikation</TITLE>
       </HEAD>
       <BODY>
       <H1>Java Applikation</H1><P>
       Nedanstående fönster innehåller en JAVA-Applet.<P>
       <APPLET code=SecondApplet.class width=350 height=120>
       </APPLET>
       <P>
       Slut
       </BODY>
       </HTML>
*/

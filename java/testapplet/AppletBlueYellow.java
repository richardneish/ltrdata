import java.awt.*;
import java.awt.event.*;

public class AppletBlueYellow extends java.applet.Applet
{
   public void init()
   {
      Button b;
      b = new Button("Blå");
      b.addActionListener(new ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            setBackground(Color.blue);
         }
      });
      add(b);
      b = new Button("Gul");
      b.addActionListener(new ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            setBackground(Color.yellow);
         }
      });
      add(b);
   }
}



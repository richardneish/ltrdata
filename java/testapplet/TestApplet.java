import java.awt.*;
import java.awt.event.*;

public class TestApplet extends java.applet.Applet
{
   public void init()
   {
      addMouseListener(new MouseAdapter()
      {
         public void mouseClicked(MouseEvent e)
         {
            setBackground(Color.red);
         }
      });

      Button b = new Button("Gul");
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



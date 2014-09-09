import java.awt.*;
import java.awt.event.*;

public class DirectEventApplet extends java.applet.Applet
{
   public void setAppletBackground(Color color)
   {
      setBackground(color);
   }

   public void processMouseEvent(MouseEvent e)
   {
      if (e.getID() == MouseEvent.MOUSE_CLICKED)
         setBackground(Color.red);
   }

   public void init()
   {
      enableEvents(AWTEvent.MOUSE_EVENT_MASK);
      add(new Button("Gul")
      {
         {
            enableEvents(AWTEvent.ACTION_EVENT_MASK);
         }

         public void processActionEvent(ActionEvent e)
         {
            setAppletBackground(Color.yellow);
         }
      });
   }
}


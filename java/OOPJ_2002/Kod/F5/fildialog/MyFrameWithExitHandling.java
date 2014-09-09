import java.awt.*;
import java.awt.event.*;

public class MyFrameWithExitHandling extends Frame
  implements WindowListener
{

  public MyFrameWithExitHandling(String str)
  {
    super(str);
    addWindowListener(this);
  }

  public MyFrameWithExitHandling()
  {
    super();
    addWindowListener(this);
  }

  public void windowClosed(WindowEvent event)
  {
  }

  public void windowDeiconified(WindowEvent event)
  {
  }

  public void windowIconified(WindowEvent event)
  {
  }

  public void windowActivated(WindowEvent event)
  {
  }

  public void windowDeactivated(WindowEvent event)
  {
  }

  public void windowOpened(WindowEvent event)
  {
  }

  public void windowClosing(WindowEvent event)
  {
    dispose();
    System.exit(1);
  }
}



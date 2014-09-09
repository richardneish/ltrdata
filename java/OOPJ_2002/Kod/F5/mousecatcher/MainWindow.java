import java.awt.*;
import java.util.*;
import java.awt.event.*;

public class MainWindow extends Frame{
  private final int width = 75;
  private final int height = 25;
  private int xPos;
  private int yPos;
  private static Random r = new Random();
  private Button catchButton;
  private MenuBar menuBar1;
  private Menu helpMenu;
  private MenuItem aboutMenu;
  private MenuItem exitMenu;

  MainWindow(String title){
    super(title);
    toss();
    setLayout(null);
    setSize(300,300);
    catchButton = new Button("Tryck Här!");
    add(catchButton);
    menuBar1 = new MenuBar();
    helpMenu = new Menu("Hjälp");
    aboutMenu = new MenuItem("Om...");
    helpMenu.add(aboutMenu);
    exitMenu = new MenuItem("Avsluta!");
    helpMenu.add(exitMenu);
    menuBar1.add(helpMenu);
    setMenuBar(menuBar1);
    catchButton.setSize(width,height);
    catchButton.setLocation(xPos,yPos);
    WindowL lWindow = new WindowL();
    addWindowListener(lWindow);
    MouseMotion lMouseMotion = new MouseMotion();
    catchButton.addMouseMotionListener(lMouseMotion);
    MouseClick lMouseClick = new MouseClick();
    catchButton.addMouseListener(lMouseClick);
    Action lAction = new Action();
    exitMenu.addActionListener(lAction);
    aboutMenu.addActionListener(lAction);
  }

  private void toss(){
    xPos = Math.abs(r.nextInt() % 230) + 25;
    yPos = Math.abs(r.nextInt() % 230) + 25;
  }

  public void start_dialog(){
    AboutDialog dlg = new AboutDialog("Om",this,true);
  }

  class WindowL extends WindowAdapter {
    public void windowClosing(WindowEvent event) {
      Object object = event.getSource();
      if (object == MainWindow.this)
        System.exit(0);
      }
  }

  class MouseMotion extends MouseMotionAdapter {
    public void mouseMoved(MouseEvent event){
      Object object = event.getSource();
      if (object == catchButton){
        toss();
        catchButton.setLocation(xPos,yPos);
      }
    }
  }

  class MouseClick extends MouseAdapter {
    public void mouseClicked(MouseEvent event){
      Object object = event.getSource();
      if (object == catchButton){//träff...
        toss();
        setBackground(new Color(xPos,yPos,xPos));
      }
    }
  }

  class Action implements ActionListener{
    public void actionPerformed(ActionEvent event){
      Object object = event.getSource();
      if (object == exitMenu)
        System.exit(0);
      else if (object == aboutMenu)
        start_dialog();
    }
  }
}


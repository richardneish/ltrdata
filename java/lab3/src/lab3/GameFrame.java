
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.awt.*;
import java.awt.event.*;

public class GameFrame extends Frame
  implements MouseListener, WindowListener
{
  GameBoard gameBoard;
  Panel buttons = new Panel();
  Button bStop = new Button("Avsluta");
  Button bRestart = new Button("Starta om");

  public GameFrame()
  {
    setTitle("Fyra i rad");
    bStop.addMouseListener(this);
    bRestart.addMouseListener(this);
    addWindowListener(this);

    setBounds(60, 0, 520, 480);
    gameBoard = new GameBoard(this, getWidth(),
      getHeight()-buttons.getHeight());
    buttons.add(bStop);
    buttons.add(bRestart);

    add(gameBoard);
    add("South", buttons);
    setVisible(true);
  }

  public void update(Graphics g)
  {
    paint(g);
  }

  public GameBoard getGameBoard()
  {
    return gameBoard;
  }

  public void mouseClicked(MouseEvent e)
  {
    if (e.getSource() == bStop)
      System.exit(0);

    if (e.getSource() == bRestart)
      FourInRow.getGameManager().breakGame = true;
  }

  public void mousePressed(MouseEvent e)   { }
  public void mouseReleased(MouseEvent e)  { }
  public void mouseEntered(MouseEvent e)   { }
  public void mouseExited(MouseEvent e)    { }

  public void windowIconified(WindowEvent e)    { }
  public void windowActivated(WindowEvent e)    { }
  public void windowDeactivated(WindowEvent e)  { }
  public void windowDeiconified(WindowEvent e)  { }
  public void windowOpened(WindowEvent e)       { }
  public void windowClosed(WindowEvent e)       { }

  public void windowClosing(WindowEvent e)
  {
    System.exit(0);
  }
}

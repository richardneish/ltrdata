
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab4client;

import java.awt.*;
import java.awt.event.*;
import java.io.*;

// The chat window class.
public class ClientFrame extends Frame
  implements MouseListener, WindowListener, ActionListener
{
  // Controls
  private Panel buttons = new Panel();
  private Button bStop = new Button("Exit");
  private Button bRestart = new Button("Connect");
  private TextField write = new TextField();
  private TextArea txt = new TextArea();

  // Constructor. Sets up window size, position and controls.
  public ClientFrame()
  {
    setBounds(20, 15, 600, 450);
    setTitle("Chat");
    txt.setEditable(false);
    buttons.add(bStop);
    buttons.add(bRestart);
    add("North",buttons);
    add("Center",txt);
    add("South",write);

    write.addActionListener(this);
    bStop.addMouseListener(this);
    bRestart.addMouseListener(this);
    addWindowListener(this);
    pack();
    setVisible(true);
  }

  // Print message into the text area.
  public void printMsg(String msg)
  {
    txt.append(msg + "\n");
  }

  // Event handler for enter key press in the text field.
  public void actionPerformed(ActionEvent e)
  {
    if (e.getSource() == write)
    {
      try
      {
        ClientChat.sendMsg(write.getText());
        write.setText("");
      }
      catch (IOException error)
      {
        // Display error message if unable to send to server.
        txt.append(error.getMessage() + "\n");
      }
    }
  }

  // Event handler for mouse click on buttons.
  public void mouseClicked(MouseEvent e)
  {
    // If "exit" button clicked, quit app.
    if (e.getSource() == bStop)
      System.exit(0);

    // If "Connect" button clicked, bring up connect dialog box. Ignore any
    // failures on connection etc.
    if (e.getSource() == bRestart)
        ClientChat.getSetupDialog().dialogValue();
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

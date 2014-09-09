
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
import java.net.*;

// The connect dialog box.
public class SetupDialog extends QuestionDialog
{
  private TextField f1 = new TextField("Nickname");
  private TextField f2 = new TextField("ing002.ing.hb.se");
  private TextField f3 = new TextField("6666");

  // Constructor. Organizes controls.
  public SetupDialog()
  {
    super (ClientChat.getClientFrame(),"", new String[]{ "OK", "Cancel" });

    setTitle("Connect to chat server");

    setLayout(new FlowLayout());

    add("North", f1);
    add("North", f2);
    add("North", f3);

    pack();
  }

  // Method to display the modal box.
  // Returns true if user selects cancel or if new connection fails.
  public boolean dialogValue()
  {
    setModal(true);
    show();

    // Clicked cancel button
    if (getAnswer().compareTo("Cancel") == 0)
      return true;

    // Get answer text strings
    String nickname = f1.getText();
    String server = f2.getText();
    String port = f3.getText();

    // Clicked OK.
    if (getAnswer().compareTo("OK") == 0)
    {
      // Connect. If failed, display box and return true.
      try
      {
        ClientChat.connectServer(nickname,server,port);
      }
      catch(IOException ioe)
      {
        new QuestionDialog(ClientChat.getClientFrame(), "Connection failed.",
          new String[]{"OK"}).show();
        return true;
      }
      return false;
    }

    return true;
  }
}

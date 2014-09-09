
/**
 * Title:        Lab 4 Client<p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab4client;

import java.io.*;
import java.net.*;
import java.util.*;

// The main class of the chat client. This class contains static instances of
// other classes and some static methods. No objects of this class are created.
public class ClientChat
{
  // Instances of ClientFrame and SetupDialog
  private static ClientFrame clientFrame = new ClientFrame();
  private static SetupDialog setupDialog = new SetupDialog();

  // Possibly changed by event handlers -> volatile required.
  private static volatile BufferedReader streamIn;
  private static volatile OutputStream streamOut;
  private static volatile Socket socket;

  public static ClientFrame getClientFrame()
  {
    return clientFrame;
  }

  public static SetupDialog getSetupDialog()
  {
    return setupDialog;
  }

  // Attempts to connect to server. If not successful, IOException is thrown
  // and existing socket and I/O buffers are left unchanged.
  public static void connectServer(String nickName, String server, String port)
    throws IOException
  {
    int portNr = Integer.parseInt(port);

    Socket newSocket;
    BufferedReader newStreamIn;
    OutputStream newStreamOut;

    newSocket = new Socket(server, portNr);
    clientFrame.printMsg("Connected to " + server + " at port " + portNr);

    newStreamIn = new BufferedReader(new InputStreamReader(
      newSocket.getInputStream()));

    newStreamOut = newSocket.getOutputStream();

    streamIn = newStreamIn;
    streamOut = newStreamOut;

    if (socket != null)
      socket.close();

    socket = newSocket;

    sendMsg(nickName);
  }

  // Method to write a chat message to the server. A terminating newline
  // character is added.
  public static void sendMsg(String msg)
    throws IOException
  {
    if (streamOut == null)
      clientFrame.printMsg("Error: Not connected.");
    else
      streamOut.write((msg + "\n").getBytes());
  }

  public static void main(String[] args)
  {
    // Show connect dialog box.
    // If user selected cancel or connection failed, quit app.
    if (setupDialog.dialogValue())
      System.exit(0);

    // Message loop listening for incoming messages from the server.
    String chatMsg;
    for (;;)
    {
      try
      {
        // Get message from server.
        chatMsg = streamIn.readLine();
      }
      catch(SocketException se)
      {
        // If SocketException is thrown, the socket has been disconnected.
        chatMsg = "Disconnected.";
      }
      catch(IOException ioe)
      {
        // Catch I/O exception on socket buffer.
        chatMsg = "Socket I/O error: " + ioe.getMessage();
      }

      // If user is disconnected, bring up setupDialog again
      if (chatMsg == null)
      {
        clientFrame.printMsg("Disconnected.");

        // If user selects cancel, quit app
        if (setupDialog.dialogValue())
          System.exit(0);
        else
          continue;
      }

      // Print the message in the chat window.
      clientFrame.printMsg(chatMsg);
    }
  }
}

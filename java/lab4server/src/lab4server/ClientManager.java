
/**
 * Title:        null<p>
 * Description:  null<p>
 * Copyright:    null<p>
 * Company:      <p>
 * @author
 * @version null
 */
package lab4server;

import java.net.*;
import java.io.*;
import java.util.*;

// The class for handling client connections. The class instanciates an object
// of class UserNames to get a static hashtable of connected users.
public class ClientManager extends Thread
{
  // Instantiate a UserNames hashtable with initial capacity for 100 users.
  private static UserNames userNames = new UserNames(100);

  private String name;                  // Name of this user
  private Socket socket;                // Connected socket
  private BufferedReader inBuffer;      // Buffer reading socket
  private OutputStream outBuffer;       // Stream writing socket

  // Method to send a message to this user.
  public void sendMsg(String msg)
    throws IOException
  {
    outBuffer.write((msg + "\n").getBytes());
  }

  // Constructor. An object of this class is created for each incoming
  // connection. Takes the accepted socket as argument.
  public ClientManager(Socket socket)
    throws IOException
  {
    this.socket = socket;

    // Create buffer reader for socket input
    inBuffer = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    // Create DataOutputStream for socket write
    outBuffer = socket.getOutputStream();

    // Read user name from socket input buffer
    name = inBuffer.readLine();
    if (name == null)
    {
      socket.close();
      return;
    }

    if (name.length() == 0)
    {
      socket.close();
      return;
    }

    // If a user with this name is already in the hash table, disconnect user.
    if (userNames.get(name.toLowerCase()) != null)
    {
      sendMsg("Sorry, your nickname is already in use.");
      socket.close();
      return;
    }

    // Start the message thread.
    start();
  }

  // The thread servicing each connection.
  public void run()
  {
    // Insert this user to the hash table.
    userNames.put(name.toLowerCase(), this);

    // Notify other users that a new user has joined the chat.
    userNames.broadCastMsg(name + " joined the chat.");

    // Main message loop.
    for (;;)
    {
      // Read chat messages from this user, and break looping if IOException
      // caught (user disconnected) or zero length messages.
      String chatMsg;
      try
      {
        chatMsg = inBuffer.readLine();
      }
      catch(SocketException se)
      {
        break;
      }
      catch(IOException ioe)
      {
        break;
      }

      if (chatMsg == null)
        break;

      if (chatMsg.length() == 0)
        break;

      if (chatMsg.charAt(0) == '/')
         if (parseCommand(chatMsg))
            continue;
         else
            break;

      // Broadcast message to all users.
      userNames.broadCastMsg(name + "> " + chatMsg);
    }

    // Notify other users that this user left the chat.
    userNames.broadCastMsg(name + " left the chat.");

    // Close the socket and remove this user from the hash table.
    try
    {
      socket.close();
    }
    catch(SocketException se)
    {
    }
    catch(IOException ioe)
    {
    }
    userNames.remove(name.toLowerCase());
  }

  // Returns false if connection should be closed.
  private boolean parseCommand(String cmd)
  {
    cmd = cmd.toLowerCase();

    try
    {
      // Send list of connected users.
      if (cmd.equals("/list"))
      {
        sendMsg("List of connected users:");
        int iUserCounter = 0;
        for (Enumeration e = userNames.elements(); e.hasMoreElements(); )
        {
          sendMsg("User " + (++iUserCounter) + ": " +
            ((ClientManager)e.nextElement()).name);
        }
        sendMsg("Total " + iUserCounter + " users connected.");
        return true;
      }

      // Cancel connection.
      if (cmd.equals("/quit"))
        return false;

      sendMsg("Unknown command.");
      return true;
    }
    catch(SocketException se)
    {
    }
    catch(IOException ioe)
    {
    }
    return true;
  }
}

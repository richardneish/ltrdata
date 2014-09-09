
/**
 * Title:        Lab 4 Server<p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab4server;

import java.io.*;
import java.net.*;

// The main class of the chat server. Sets up server socket and contains main
// accept loop.
public class ServerMain
{
  // Listening port == aplication parameter
  public static void main(String[] args)
  {
    // Default listen port is 6666 if none given at the command line.
    int listenerPort = 6666;
    try
    {
      listenerPort = Integer.parseInt(args[0]);
    }
    catch(java.lang.ArrayIndexOutOfBoundsException aioobe)
    {
    }

    // Listening server socket.
    ServerSocket listenerSocket;
    try
    {
      listenerSocket = new ServerSocket(listenerPort);
    }
    catch(BindException be)
    {
      System.out.println("Listen failed: " + be.getMessage());
      return;
    }
    catch(IOException ioe)
    {
      System.out.println("Socket failed: " + ioe.getMessage());
      return;
    }
    // Everything ok msg.
    System.out.println("Listening at port : " + listenerPort);

    // Service thread accept loop.
    for (;;)
    {
      // Accept connections.
      Socket clientSocket;
      try
      {
        clientSocket = listenerSocket.accept();
      }
      catch(IOException ioe)
      {
        System.out.println("Accept failed: " + ioe.getMessage());
        break;
      }
      // Print msg on console for each incoming connection.
      System.out.println(clientSocket.getInetAddress().toString() +
        " connected");

      // Create a ClientManager object for each incoming connection.
      // The constructor will create a new thread servicing the connection.
      try
      {
        new ClientManager(clientSocket);
      }
      catch(IOException ioe)
      {
        System.out.println("ClientManager failed: " + ioe.getMessage());
        break;
      }
   }
  }
}

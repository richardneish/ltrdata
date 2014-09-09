
/**
 * Title:        null<p>
 * Description:  null<p>
 * Copyright:    null<p>
 * Company:      <p>
 * @author
 * @version null
 */
package lab4server;

import java.util.*;
import java.io.*;

// A hashtable that keeps a list of all connected users (ClientManager objects).
// Contains method for broadcasting msg to all objects in the table.
public class UserNames extends Hashtable
{
  public UserNames(int initialCapacity)
  {
    super(initialCapacity);
  }

  // Function to broadcast a message from a user to all users in the table.
  // Calls sendMsg() method on each ClientManager object in the table.
  synchronized public void broadCastMsg(String msg)
  {
    // Print a copy of the msg to the server console.
    System.out.println(msg);

    for (Enumeration e = elements(); e.hasMoreElements(); )
    {
      try
      {
        ((ClientManager)e.nextElement()).sendMsg(msg);
      }
      catch(IOException ioe)
      {
      }
    }
  }
}

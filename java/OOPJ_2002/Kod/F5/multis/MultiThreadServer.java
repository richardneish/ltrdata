import java.io.*;
import java.net.*;
import java.util.*;

public class MultiThreadServer
{
  public static void main(String[] args)
  {
    try
    {
      //create a server socket
      ServerSocket serverSocket = new ServerSocket(4001);

      //to label a thread
      int i = 0;
      System.out.println("Server started");
      while (true)
      {

        //listen for a new connection request
        Socket connectToClient = serverSocket.accept();

        //print the new connect number on the console
        System.out.println("Starting thread "+i);

        //create a new thread for the connection
        ThreadHandler t = new ThreadHandler(connectToClient, i);

        //start the new thread
        t.start();

        //increment i to label the next connection
        i++;
      }
    }
    catch(IOException ex)
    {
      System.err.println("!\n"+ex);
    }
  }
}

//define the thread class for handling new connection
class ThreadHandler extends Thread
{
  private Socket connectToClient; //a connected socket
  private int counter; //label for the connection

  public ThreadHandler(Socket socket, int i)
  {
    connectToClient = socket;
    counter = i;
  }

  public void run()
  {
    try
    {
      //create data input and print streams
      BufferedReader isFromClient = new BufferedReader(
        new InputStreamReader(connectToClient.getInputStream()));
      PrintWriter osToClient =
        new PrintWriter(connectToClient.getOutputStream(), true);

      //continuously serve the client
      while (true)
      {
        //receive data from the client in string
        StringTokenizer st = new StringTokenizer
          (isFromClient.readLine());

        //get radius
        double radius = new Double(st.nextToken()).doubleValue();
        System.out.println("radius received from client: "+radius);

        //compute area
        double area = radius*radius*Math.PI;

        //send area back to the client
        osToClient.println(area);
        System.out.println("Area found: "+area);
      }
    }
    catch(IOException ex)
    {
      System.err.println(",\n"+ex);
    }
    catch(NullPointerException ex)
    {
      System.err.println("*\n"+ex);
    }
  }
}
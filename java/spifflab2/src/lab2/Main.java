
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab2;

import java.io.*;
import java.util.*;

public class Main {
  static public void main(String[] arg)
  {
    // Read initialization file
    try
    {
      readInitFile(arg[0]);
    }
    catch(ArrayIndexOutOfBoundsException aioobe)
    {
      System.out.println("Required parameter missing.");
      return;
    }
    catch(FileNotFoundException fnfe)
    {
      System.out.println("File " + arg[0] + " not found.");
      return;
    }
    catch(IOException ioe)
    {
      System.out.println("I/O error in file " + arg[0] + ".");
      return;
    }

    // Command line interface
    try
    {
      (new Player()).cmdUI();
    }
    catch(IOException ioe)
    {
      System.out.println("I/O error");
      return;
    }
  }

  static void readInitFile(String fileName)
    throws FileNotFoundException, IOException
  {
    // Create FileReader object for data file
    FileReader fr = new FileReader(fileName);

    // Status variable indicating current section in data file
    final int fsNone = 0;
    final int fsRoom = 1;
    final int fsItem = 2;
    int fileSection = fsNone;

    // Create BufferedReader object to read lines from the data file
    BufferedReader infileBuffer = new BufferedReader(fr);
    for(int i=0; ; i++)
    {
      try
      {
        // Read a line
        String str = infileBuffer.readLine();
        if(str == null)
          break;

        if(str.length() == 0)
          continue;

        // If new section
        if(str.charAt(0) == '[')
        {
          if(str.compareToIgnoreCase("[rooms]") == 0)
          {
            fileSection = fsRoom;
            continue;
          }

          if(str.compareToIgnoreCase("[items]") == 0)
          {
            fileSection = fsItem;
            continue;
          }

          // If not recognized section, don't section lines
          fileSection = fsNone;
        }

        // Tokenize the line
        StringTokenizer strtok = new StringTokenizer(str, ";");
        if(!strtok.hasMoreTokens())
          continue;

        // Build object depending on current file section
        switch(fileSection)
        {
          case fsRoom:
            Room.buildRoom(strtok);
            break;
          case fsItem:
            Item.buildItem(strtok);
            break;
        }

      }
      // Some error checking
      catch(NoSuchElementException nsee)
      {
        System.out.println("Error in file " + fileName + " on line " + i);
      }
    }
  }
}

class Player
{
  Room room;      // The room the player is currently in
  Vector items;   // Indexes to the player's inventories

  Player()
  {
    items = new Vector();
    room = Room.getStartRoom();
    room.enter();
  }

  public void cmdUI()
    throws IOException
  {
    // Create buffer reader for console input
    BufferedReader cmdBuffer = new BufferedReader(new InputStreamReader(System.in));

    // Read string lines from console input buffer
    String str;
    while((str = cmdBuffer.readLine()) != null)
    {
      if(str.compareToIgnoreCase("EXIT") == 0)
        break;

      final int cmdNone = 0;
      final int cmdPick = 1;
      final int cmdDrop = 2;
      final int cmdUse = 3;
      final int cmdLook = 4;
      final int cmdWest = 5;
      final int cmdEast = 6;
      final int cmdNorth = 7;
      final int cmdSouth = 8;
      final int cmdInvent = 9;
      final int cmdDone = 10;
      int cmdFound = cmdNone;
      StringTokenizer strtok = new StringTokenizer(str);
      if(!strtok.hasMoreTokens())
        continue;

      while(strtok.hasMoreTokens())
      {
        String tok = strtok.nextToken();
        if((tok.compareToIgnoreCase("PICK") == 0) |
          (tok.compareToIgnoreCase("TAKE") == 0) |
          (tok.compareToIgnoreCase("GET") == 0))
          cmdFound = cmdPick;
        else if(tok.compareToIgnoreCase("DROP") == 0)
          cmdFound = cmdDrop;
        else if((tok.compareToIgnoreCase("LOOK") == 0) |
          (tok.compareToIgnoreCase("EXAMINE") == 0))
        {
          if(!strtok.hasMoreTokens())
            room.inventItemsByName(null);

          cmdFound = cmdLook;
        }
        else if((tok.compareToIgnoreCase("WEST")  == 0) |
          (tok.compareToIgnoreCase("W") == 0))
          cmdFound = cmdWest;
        else if((tok.compareToIgnoreCase("EAST") == 0) |
          (tok.compareToIgnoreCase("E") == 0))
          cmdFound = cmdEast;
        else if((tok.compareToIgnoreCase("NORTH") == 0) |
          (tok.compareToIgnoreCase("N")== 0))
          cmdFound = cmdNorth;
        else if((tok.compareToIgnoreCase("SOUTH") == 0) |
          (tok.compareToIgnoreCase("S") == 0))
          cmdFound = cmdSouth;
        else if(tok.compareToIgnoreCase("INVENT") == 0)
          cmdFound = cmdInvent;
        else if(cmdFound == cmdPick)
        {
          ItemIndex item = room.takeItemByName(tok);
          if(item != null)
          {
            System.out.println("You have picked up the " + tok +
              " from this room");
            items.add(item);
            cmdFound = cmdDone;
          }
        }
        else if(cmdFound == cmdLook)
          room.inventItemsByName(tok);
        else if(cmdFound == cmdDrop)
        {
          ItemIndex item = takeItemByName(tok);
          if(item != null)
          {
            System.out.println("You have dropped the " + tok +
              " into this room");
            room.addItem(item);
            cmdFound = cmdDone;
          }
        }
      }

      switch(cmdFound)
      {
        case cmdPick:
          System.out.println("No such thing in this room.");
          break;
        case cmdDrop:
          System.out.println("You have no such thing");
          break;
        case cmdWest:
          changeRoom(room.getWestRoom());
          break;
        case cmdEast:
          changeRoom(room.getEastRoom());
          break;
        case cmdSouth:
          changeRoom(room.getSouthRoom());
          break;
        case cmdNorth:
          changeRoom(room.getNorthRoom());
          break;
        case cmdInvent:
          inventItems();
          break;
        case cmdDone: case cmdLook:
          break;
        default:
          System.out.println("Sorry, don't understand :(");
      }
    }
  }

  void changeRoom(Room newRoom)
  {
    if(newRoom == null)
    {
      System.out.println("You cannot go to that direction.");
      return;
    }

    int entranceItem = newRoom.getEntranceItem();
    if(entranceItem != -1)
    {
      if(takeItemByIndex(entranceItem) == null)
      {
        System.out.println(Item.getItemByIndex(entranceItem).getName() +
          "...");
        return;
      }

      System.out.println("You used " +
        Item.getItemByIndex(entranceItem).getName() + " to enter the room.");
    }

    newRoom.enter();
    room = newRoom;
  }

  ItemIndex takeItemByName(String itemName)
  {
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      ItemIndex item = ((ItemIndex)e.nextElement());
      if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName)
        == 0)
      {
        items.remove(item);
        return item;
      }
      else
      {
        item = item.takeItemByName(itemName);
        if(item != null)
          return item;
      }
    }

    return null;
  }

  ItemIndex takeItemByIndex(int itemFind)
  {
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      ItemIndex item = ((ItemIndex)e.nextElement());
      if(item.getId() == itemFind)
      {
        items.remove(item);
        return item;
      }
      else
      {
        item = item.takeItemByIndex(itemFind);
        if(item != null)
          return item;
      }
    }

    return null;
  }

  public void inventItems()
  {
    Enumeration e = items.elements();
    if(!e.hasMoreElements())
    {
      System.out.println("You have no items.");
      return;
    }

    System.out.println("Inventory:");
    while(e.hasMoreElements())
      ((ItemIndex)e.nextElement()).inventItems(0);
  }


}

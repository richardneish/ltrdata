
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

// Main class.
public class Main
{
  // main() function. Open world file and create player object.
  public static void main(String[] arg)
  {
    // Read initialization file. File name is first parameter on command line.
    try
    {
      readInitFile(arg[0]);
    }
    // Catch exception if no parameter on command line.
    catch(ArrayIndexOutOfBoundsException aioobe)
    {
      System.out.println("Required parameter missing.");
      return;
    }
    // Catch exception on file not found.
    catch(FileNotFoundException fnfe)
    {
      System.out.println("File " + arg[0] + " not found.");
      return;
    }
    // Catch I/O errors.
    catch(IOException ioe)
    {
      System.out.println("I/O error in file " + arg[0] + ".");
      return;
    }

    // Command line interface
    try
    {
      // Create player object and run command user interface on that object.
      (new Player()).cmdUI();
    }
    // If any I/O error in world file.
    catch(IOException ioe)
    {
      System.out.println("I/O error");
      return;
    }
  }

  // Function to open and read world file.
  private static void readInitFile(String fileName)
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

        // If nothing on that line, or comment, continue on next line.
        if(str.length() == 0)
          continue;
        if(str.charAt(0) == '#')
          continue;

        // If new section
        if(str.charAt(0) == '[')
        {
          // Section header for rooms or items
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
            new Room(strtok);
            break;
          case fsItem:
            new Item(strtok);
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

// Player class containing references to carried items and current room.
class Player
{
  Room room;      // The room the player is currently in
  Vector items;   // Indexes to the player's inventories

  // Constructor. Create empty items vector (carries no items from  the
  // beginning. Get and enter the starting room.
  Player()
  {
    items = new Vector();
    room = Room.getStartRoom();
    room.enter();
  }

  // Command user interface.
  public void cmdUI()
    throws IOException
  {
    // Create buffer reader for console input
    BufferedReader cmdBuffer = new BufferedReader(new InputStreamReader(System.in));

    // Read string lines from console input buffer
    String str;
    while((str = cmdBuffer.readLine()) != null)
    {
      // If command exit, break this loop
      if(str.compareToIgnoreCase("EXIT") == 0)
        break;

      // Status variable indicating which command found.
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

      // Tokenize the command line
      StringTokenizer strtok = new StringTokenizer(str);
      // If nothing at command line, just continue.
      if(!strtok.hasMoreTokens())
        continue;

      while(strtok.hasMoreTokens())
      {
        // Get command and set cmdFound value
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
          // If just look/examine without parameters, look in current room.
          if(!strtok.hasMoreTokens())
          {
            room.inventItemsByName(null);
            cmdFound = cmdDone;
          }

          cmdFound = cmdLook;
        }
        else if(tok.compareToIgnoreCase("WEST") == 0)
          cmdFound = cmdWest;
        else if(tok.compareToIgnoreCase("EAST") == 0)
          cmdFound = cmdEast;
        else if(tok.compareToIgnoreCase("NORTH") == 0)
          cmdFound = cmdNorth;
        else if(tok.compareToIgnoreCase("SOUTH") == 0)
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

      // Do things depending on cmdFound value
      switch(cmdFound)
      {
        // If "PICK" but no items found that set the cmdFound value to cmdDone,
        // print error msg.
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

  // Function to change current room.
  void changeRoom(Room newRoom)
  {
    // If null room, we got null from i.e. getWestRoom() which means there is
    // room in that direction.
    if(newRoom == null)
    {
      System.out.println("You cannot go to that direction.");
      return;
    }

    // Check if the new room requires an item to enter it.
    int entranceItem = newRoom.getEntranceItem();
    if(entranceItem != -1)
    {
      // Try to take the required item from the player.
      if(takeItemByIndex(entranceItem) == null)
      {
        System.out.println("You need " +
          Item.getItemByIndex(entranceItem).getName() +
          " to enter that room.");
        return;
      }

      System.out.println("You used " +
        Item.getItemByIndex(entranceItem).getName() + " to enter the room.");
    }

    // Call enter() function on the room so that it knows the player has
    // entered. It will display room description and items in the room.
    newRoom.enter();
    room = newRoom;
  }

  // Try to take an item from the players carried items.
  ItemIndex takeItemByName(String itemName)
  {
    // Enumerate players items
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      // If item found, remove it from collection and return it.
      ItemIndex item = ((ItemIndex)e.nextElement());
      if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName)
        == 0)
      {
        items.remove(item);
        return item;
      }
      else
      {
        // If not the item we are looking for, check if this item contains the
        // item we are looking for.
        item = item.takeItemByName(itemName);
        if(item != null)
          return item;
      }
    }

    // Nothing found.
    return null;
  }

  // Same as takeItemByName() but take item by index number.
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

  // Print informaiton on all items carried by the player.
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

/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab2;

import java.util.*;   // Import for Vector class
import java.applet.*; // Import for AudioClip class
import java.net.*;    // Import for URL class

public class Room
{
  private static Vector rooms;

  private AudioClip sound;       // Sound when entering room (not implemented)
  private String description;    // Description when entering room.
  private int requiredItem;      // Item required to enter room.
  private int roomWest, roomEast, roomNorth, roomSouth;  // Neighbouring rooms.
  private Vector items;          // Items located in the room.

  // Public constructor. Create a new Room object from tokenized string.
  public Room(StringTokenizer strtok)
  {
    try
    {
      URL soundFile;
      soundFile = new URL(strtok.nextToken());
      sound = Applet.newAudioClip(soundFile);
    }
    catch(MalformedURLException murle)
    {
      System.out.println("Invalid sound file URL.");
      sound = null;
    }
    description = strtok.nextToken();
    requiredItem = Integer.parseInt(strtok.nextToken());
    roomWest = Integer.parseInt(strtok.nextToken());
    roomEast = Integer.parseInt(strtok.nextToken());
    roomNorth = Integer.parseInt(strtok.nextToken());
    roomSouth = Integer.parseInt(strtok.nextToken());

    // Items located in this room. An item collection vector is always created
    // for all rooms, even if there are no items to be added to the vector.
    items = new Vector();
    while(strtok.hasMoreTokens())
    {
      String itemId = strtok.nextToken();
      items.add(new ItemIndex(Integer.parseInt(itemId)));
    }

    // If rooms collection object does not already exist, create one.
    if (rooms == null)
      rooms = new Vector();

    // Add this new object to the rooms collection
    rooms.add(this);
  }

  // Print all information in this room.
  public void printDebugInfo()
  {
    System.out.println("Sound: " + sound);
    System.out.println("Description: " + description);
    System.out.println("Required item: " + requiredItem);
    System.out.println("Room west: " + roomWest);
    System.out.println("Room east: " + roomEast);
    System.out.println("Room north: " + roomNorth);
    System.out.println("Room south: " + roomSouth);
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      int i = ((ItemIndex)e.nextElement()).getId();
      System.out.println("Item in this room: " + i);

    }
  }

  // Try to take item from this room given the name of the item.
  public ItemIndex takeItemByName(String itemName)
  {
    // Enumerate items in this room.
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      // If this is the item we look for, remove it from items in the room and
      // return the item (ItemIndex object).
      ItemIndex item = ((ItemIndex)e.nextElement());
      if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName)
        == 0)
      {
        items.remove(item);
        return item;
      }
      else
      {
        // If this is not the item we are looking for, check items located in
        // this item.
        item = item.takeItemByName(itemName);
        if(item != null)
          return item;
      }
    }

    // No item found.
    return null;
  }

  // Print description for items in this room. If an item name is given, only
  // that item's description and descriptions of items located in that item are
  // printed.
  public void inventItemsByName(String itemName)
  {
    Enumeration e = items.elements();

    if(!e.hasMoreElements())
    {
      System.out.println(description);
      System.out.println("This room contains no items.");
      return;
    }

    if(itemName == null)
    {
      // If no items in this room, print just room description.
      System.out.println(description);
      System.out.println("Items in this room:");
    }

    // Recurse items and print descriptions.
    while(e.hasMoreElements())
    {
      ItemIndex item = ((ItemIndex)e.nextElement());
      if(itemName == null)
        Item.getItemByIndex(item).printDescription();
      else
        if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName)
          == 0)
          item.inventItems(0);
    }
  }

  public void addItem(ItemIndex item)
  {
    items.add(item);
  }

  public final int getEntranceItem()
  {
    return requiredItem;
  }

  public void enter()
  {
    requiredItem = -1;

    inventItemsByName(null);

    if(sound != null)
      sound.play();
 }

  public static Room getStartRoom()
  {
    return (Room)rooms.firstElement();
  }

  // Functions to get neighbouring rooms.
  public Room getWestRoom()
  {
    return (Room)rooms.elementAt(roomWest);
  }

  public Room getEastRoom()
  {
    return (Room)rooms.elementAt(roomEast);
  }

  public Room getNorthRoom()
  {
    return (Room)rooms.elementAt(roomNorth);
  }

  public Room getSouthRoom()
  {
    return (Room)rooms.elementAt(roomSouth);
  }
}

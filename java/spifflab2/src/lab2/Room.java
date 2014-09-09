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
import java.applet.*;
import java.net.*;

public class Room
{
  static Vector rooms;

  AudioClip sound;       // Sound played when entering room (not implemented)
  String description;    // Description of room. Displayed when entering room.
  int requiredItem;      // Item required to enter room.
  int roomWest, roomEast, roomNorth, roomSouth;  // Neighbouring rooms.
  Vector items;          // Items located in the room.

  // Public constructor. Initializes all required data members.
  public Room(AudioClip initSound, String initDescription, int initRequiredItem,
    int initRoomWest, int initRoomEast, int initRoomNorth, int initRoomSouth,
    Vector initItems)
  {
    sound = initSound;
    description = initDescription;
    requiredItem = initRequiredItem;
    roomWest = initRoomWest;
    roomEast = initRoomEast;
    roomNorth = initRoomNorth;
    roomSouth = initRoomSouth;
    items = initItems;

    // If rooms collection object does not already exist, create one
    if (rooms == null)
      rooms = new Vector();

    // Add this new object to the rooms collection
    rooms.add(this);
  }

  // Create a new Room object from tokenized string
  public static void buildRoom(StringTokenizer strtok)
  {
    AudioClip sound;
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
    String description = strtok.nextToken();
    int requiredItem = Integer.parseInt(strtok.nextToken());
    int roomWest = Integer.parseInt(strtok.nextToken());
    int roomEast = Integer.parseInt(strtok.nextToken());
    int roomNorth = Integer.parseInt(strtok.nextToken());
    int roomSouth = Integer.parseInt(strtok.nextToken());

    // Items located in this room. An item collection vector is always created
    // for all rooms, even if there are no items to be added to the vector.
    Vector items = new Vector();
    while(strtok.hasMoreTokens())
    {
      String itemId = strtok.nextToken();
      items.add(new ItemIndex(Integer.parseInt(itemId)));
    }

    // Create the room, and print some debug info.
    new Room(sound, description, requiredItem, roomWest, roomEast,
      roomNorth, roomSouth, items);
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
      int i = ((ItemIndex)e.nextElement()).id;
      System.out.println("Item in this room: " + i);

    }
  }

  public ItemIndex takeItemByName(String itemName)
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

  public void inventItemsByName(String itemName)
  {
    Enumeration e = items.elements();


    if(!e.hasMoreElements())
    {
      System.out.println(description);//RUMSBESKRIVNING??
      System.out.println("This room contains no items.");
      return;
    }

    if(itemName == null)
      System.out.println(description); //RUMSBESKRIVNING??
      System.out.println("Items in this room:");

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
    System.out.println(description);
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

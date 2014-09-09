
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab2;

import java.util.*;

public class Item
{
  static Vector itemsCollection;

  String name;           // Name of item
  String description;    // Description of item. Displayed when picking up the
                         // item.
  Vector items;          // Items located in the item.

  // Public constructor. Initializes all required data members.
  public Item(String initName, String initDescription, Vector initItems)
  {
    name = initName;
    description = initDescription;
    items = initItems;

    // If rooms collection object does not already exist, create one
    if (itemsCollection == null)
      itemsCollection = new Vector();

    // Add this new object to the rooms collection
    itemsCollection.add(this);
  }

  // Create a new Room object from tokenized string
  public static void buildItem(StringTokenizer strtok)
  {
    String name = strtok.nextToken();
    String description = strtok.nextToken();

    // Items located in this room. An item collection vector is always created
    // for all rooms, even if there are no items to be added to the vector.
    Vector items = new Vector();
    while(strtok.hasMoreTokens())
    {
      String itemId = strtok.nextToken();
      items.add(new ItemIndex(Integer.parseInt(itemId)));
    }

    // Create the room, and print some debug info.
    new Item(name, description, items);
  }

  // Print all information in this room.
  public void printDebugInfo()
  {
    System.out.println("Name: " + name);
    System.out.println("Description: " + description);
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      int i = ((ItemIndex)e.nextElement()).id;
      System.out.println("Item in this item: " + i);
    }
  }

  public String getName()
  {
    return new String(name);
  }

  // Return item object from itemsCollection by index number
  public static Item getItemByIndex(ItemIndex index)
  {
    return (Item)itemsCollection.elementAt(index.id);
  }

  public static Item getItemByIndex(int index)
  {
    return (Item)itemsCollection.elementAt(index);
  }

  public Vector getItems()
  {
    return items;
  }

  public void printDescription()
  {
    System.out.println(description);
  }
}

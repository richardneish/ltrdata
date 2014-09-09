
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

// Item class. Objects for items. Contains static vector of all Item objects,
// collection of contained other items.
public class Item
{
  private static Vector itemsCollection;  // All Item objects.

  private String name;           // Name of item
  private String description;    // Description of item. Displayed when picking up the
                                 // item.
  private Vector items;          // Items located in the item (ItemIndex objects).

  // Public constructor. Create a new Item object from tokenized string
  public Item(StringTokenizer strtok)
  {
    name = strtok.nextToken();
    description = strtok.nextToken();

    // Items located in this room. An item collection vector is always created
    // for all rooms, even if there are no items to be added to the vector.
    items = new Vector();
    while(strtok.hasMoreTokens())
    {
      String itemId = strtok.nextToken();
      items.add(new ItemIndex(Integer.parseInt(itemId)));
    }

    // If rooms collection object does not already exist, create one
    if (itemsCollection == null)
      itemsCollection = new Vector();

    // Add this new object to the rooms collection
    itemsCollection.add(this);
  }

  // Print all information in this room.
  public void printDebugInfo()
  {
    System.out.println("Name: " + name);
    System.out.println("Description: " + description);
    for(Enumeration e = items.elements(); e.hasMoreElements(); )
    {
      int i = ((ItemIndex)e.nextElement()).getId();
      System.out.println("Item in this item: " + i);
    }
  }

  // Get name of this object.
  public String getName()
  {
    return new String(name);
  }

  // Get Item object from static itemsCollection given an ItemIndex object.
  public static Item getItemByIndex(ItemIndex index)
  {
    return (Item)itemsCollection.elementAt(index.getId());
  }

  // Get Item object from static iremsCollection given an index number.
  public static Item getItemByIndex(int index)
  {
    return (Item)itemsCollection.elementAt(index);
  }

  // Get vector of ItemIndex objects for items located in this item.
  public Vector getItems()
  {
    return items;
  }

  // Print description of this item.
  public void printDescription()
  {
    System.out.println(description);
  }
}

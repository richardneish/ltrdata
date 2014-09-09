
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

// ItemIndex class. Contains a number identifying an Item object. The number is
// the order of found items in the world file.
public class ItemIndex
{
  private int id;   // Item object's id number.

  // Construct by id number (line number in world file).
  public ItemIndex(int initId)
  {
    id = initId;
  }

  // Get id number.
  public final int getId()
  {
    return id;
  }

  // Return ItemIndex object from contained items by name
  public ItemIndex takeItemByName(String itemName)
  {
    // Enumerate items located within this item.
    for(Enumeration e = Item.getItemByIndex(this).getItems().elements();
      e.hasMoreElements(); )
    {
      // If item found, remove from collection and return it.
      ItemIndex item = (ItemIndex)e.nextElement();
      if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName) == 0)
      {
        Item.getItemByIndex(this).getItems().remove(item);
        return item;
      }
      else
      {
        // If not found, check items located in this item.
        item = item.takeItemByName(itemName);
        if(item != null)
          return item;
      }
    }

    // Nothing found.
    return null;
  }

  // Return ItemIndex object from contained items by item index
  public ItemIndex takeItemByIndex(int itemFind)
  {
    for(Enumeration e = Item.getItemByIndex(this).getItems().elements();
      e.hasMoreElements(); )
    {
      ItemIndex item = (ItemIndex)e.nextElement();
      if(item.getId() == itemFind)
      {
        Item.getItemByIndex(this).getItems().remove(item);
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

  // Call printDescription() on this item object and all item objects
  // contained in this item. Set tablevel to 0 when calling.
  public void inventItems(int tablevel)
  {
    // Indent by recurse level.
    for(int i=0; i<tablevel; i++)
      System.out.print('\t');

    Item.getItemByIndex(this).printDescription();

    Enumeration e = Item.getItemByIndex(this).getItems().elements();
    if(!e.hasMoreElements())
      return;

    for(int i=0; i<=tablevel; i++)
      System.out.print('\t');

    // Recurse call for all items in this item.
    System.out.println(Item.getItemByIndex(this).getName() + " contains:");
    while(e.hasMoreElements())
      ((ItemIndex)e.nextElement()).inventItems(tablevel+1);
  }
}

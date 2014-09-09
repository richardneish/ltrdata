
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

public class ItemIndex
{
  int id;

  public ItemIndex(int initId)
  {
    id = initId;
  }

  public final int getId()
  {
    return id;
  }

  // Return ItemIndex object from contained items by name
  public ItemIndex takeItemByName(String itemName)
  {
    for(Enumeration e = Item.getItemByIndex(this).getItems().elements();
      e.hasMoreElements(); )
    {
      ItemIndex item = (ItemIndex)e.nextElement();
      if(Item.getItemByIndex(item).getName().compareToIgnoreCase(itemName) == 0)
      {
        Item.getItemByIndex(this).getItems().remove(item);
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
  // contained in this item.
  public void inventItems(int tablevel)
  {
    for(int i=0; i<tablevel; i++)
      System.out.print('\t');
    Item.getItemByIndex(this).printDescription();

    Enumeration e = Item.getItemByIndex(this).getItems().elements();
    if(!e.hasMoreElements())
      return;

    for(int i=0; i<=tablevel; i++)
      System.out.print('\t');
    System.out.println(Item.getItemByIndex(this).getName() + " contains:");
    while(e.hasMoreElements())
      ((ItemIndex)e.nextElement()).inventItems(tablevel+1);
  }
}

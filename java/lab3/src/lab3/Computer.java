
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.lang.*;

public class Computer extends Player
{
  public Computer(String playerName, int playerNumber)
  {
    super(playerName, playerNumber);
  }

  public int getNextMove()
  {
    return GameGrid.testInsert(number);
  }
}

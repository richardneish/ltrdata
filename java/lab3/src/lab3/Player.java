
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.awt.*;

abstract public class Player
{
  String name;
  int number;

  public String getName()
  {
    return name;
  }

  protected Player(String playerName, int playerNumber)
  {
    name = playerName;
    number = playerNumber;
  }

  abstract int getNextMove();

  public Color getMarkerColor()
  {
    if (number == 1)
      return Color.yellow;
    else
      return Color.red;
  }

  // Default, do nothing. Override in class Human to play sound or display
  // dialog box.
  void invalidMove()
  {
  }

  // makeMove() returvärden:
  // 0 = spelet fortsätter
  // 1 = Spelaren vann
  // 2 = Oavgjort
  public int makeMove()
  {
    for(;;)
    {
      int move = getNextMove();

      // Check user break
      if (move < 0)
        return 3;

      try
      {
        return GameGrid.doMove(move, number);
      }
      catch(InvalidMoveException ime)
      {
        invalidMove();
      }
    }
  }
}

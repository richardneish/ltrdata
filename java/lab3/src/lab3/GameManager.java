
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

public class GameManager
{
  Player player1;
  Player player2;

  public volatile boolean breakGame;

  public Color getPlayer1MarkerColor()
  {
    return player1.getMarkerColor();
  }

  public Color getPlayer2MarkerColor()
  {
    return player2.getMarkerColor();
  }

  public void addPlayer(Player player)
  {
    if (player1 == null)
    {
      player1 = player;
      return;
    }

    if (player2 == null)
    {
      player2 = player;
      return;
    }
  }

  public void playGame()
  {
    for(;;)
    {
      if (move(player1) == false)
        return;

      if (move(player2) == false)
        return;

      if (breakGame)
        return;
    }
  }

  // Request the player to make a move, display boxes for winner or drawn.
  // Then return false if game should stop.
  private boolean move(Player player)
  {
    switch (player.makeMove())
    {
      default:
        return true;
      case 1:
        WinnerDialog winnerDialog = new WinnerDialog(player.getName());
        winnerDialog.show();
        return false;
      case 2:
        DrawnDialog drawnDialog = new DrawnDialog();
        drawnDialog.show();
        return false;
      case 3:
        return false;
    }
  }
}

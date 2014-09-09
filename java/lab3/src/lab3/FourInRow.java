
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.applet.*;
import java.awt.*;

public class FourInRow
{
  static GameFrame gameFrame;
  static GameManager gm;

  public static GameFrame getGameFrame()
  {
    return gameFrame;
  }

  public static GameManager getGameManager()
  {
    return gm;
  }

  public static void main(String[] arg)
  {
    gameFrame = new GameFrame();

    for(;;)
    {
      GameGrid.clear();
      gm = new GameManager();
      SetupGameDialog setupGameDialog = new SetupGameDialog();
      if (!setupGameDialog.createPlayers(gm))
        System.exit(0);
      gm.playGame();
    }
  }
}
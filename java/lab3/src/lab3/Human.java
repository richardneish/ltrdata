
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.awt.event.*;
import java.applet.*;
import java.net.*;
import java.security.*;

public class Human extends Player
{
  AudioClip errorSound;

  public Human(String playerName, int playerNumber)
  {
    super(playerName, playerNumber);
    try
    {
      errorSound =
        Applet.newAudioClip(new URL("file:///f|/java/lab3/Hihihaha.au"));
    }
    catch(MalformedURLException murle)
    {
      errorSound = null;
    }
    catch(AccessControlException ace)
    {
      errorSound = null;
    }
  }

  void invalidMove()
  {
    System.out.println("Kolumnen är full. Lägg brickan i en annan kolumn.");
    errorSound.play();
  }

  public int getNextMove()
  {
    return FourInRow.getGameFrame().getGameBoard().getUserClickColumn();
  }
}

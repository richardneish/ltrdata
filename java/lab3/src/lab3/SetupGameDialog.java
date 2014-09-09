
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
import java.awt.event.*;

public class SetupGameDialog extends QuestionDialog
{
  TextField f1 = new TextField("Namn på spelare 1");
  TextField f2 = new TextField("Namn på spelare 2");

  Checkbox cb1=new Checkbox("Dator",false);
  Checkbox cb2=new Checkbox("Dator",true);

  public SetupGameDialog()
  {
    super (FourInRow.getGameFrame(),"", new String[]{ "OK", "Avbryt" });
    setLayout(new FlowLayout());
    setTitle("Spelare");

    add("North", f1);
    add(cb1);
    add("North", f2);
    add(cb2);

    pack();
  }

  public boolean createPlayers(GameManager gm)
  {
    setModal(true);
    show();
    if (getAnswer().compareTo("Avbryt") == 0)
      return false;

    // Visa dialogruta med namn på två spelare och ok-knapp
    Player pl1;
    Player pl2;

    if (cb1.getState())
      pl1 = new Computer(f1.getText(), 1);
    else
      pl1 = new Human(f1.getText(), 1);

    if (cb2.getState())
      pl2 = new Computer(f2.getText(), 2);
    else
      pl2 = new Human(f2.getText(), 2);

    gm.addPlayer(pl1);
    gm.addPlayer(pl2);
    return true;
  }

}
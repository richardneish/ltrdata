
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

public class WinnerDialog extends QuestionDialog
{

  public WinnerDialog(String name)
  {
    super(FourInRow.getGameFrame(),name + " vann", new String[]{"OK"});


  }
}

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

public class GameBoard extends Canvas
{
  volatile int clickedColumn;

  MouseListener ml = new MouseAdapter()
  {
    public void mouseClicked(MouseEvent e)
    {
      clickedColumn = e.getX() / (FourInRow.getGameFrame().getWidth() / 7);
    }
  };

  public GameBoard(GameFrame gf, int width, int height)
  {
    setSize(width, height);
    addMouseListener(ml);
  }

  public void update(Graphics g)
  {
    paint(g);
  }

  public void paint(Graphics g)
  {
    int gameState[][] = GameGrid.getGameState();
    Color c=new Color(0x061080);//Blå bakgrund
    setBackground(c);

    for(int x = 0; x < 7; x++)
      for(int y = 0; y < 6; y++)
      {
        g.setColor(Color.white);
        g.drawOval (x * getWidth() / 7+1,
                   y * getHeight() / 6+5 , getWidth() / 8, getHeight() / 7);
        switch(gameState[x][y])
        {
          case 0:
            g.fillOval (x * getWidth() / 7+3,
                   y * getHeight() / 6+2, getWidth() / 8, getHeight() / 7);
            break;
          case 1:
            g.setColor(FourInRow.getGameManager().getPlayer1MarkerColor());
            g.fillOval (x * getWidth() / 7+3,
                       y * getHeight() / 6+2, getWidth() / 8, getHeight() / 7);
            break;
          case 2:
            g.setColor(FourInRow.getGameManager().getPlayer2MarkerColor());
            g.fillOval (x * getWidth() / 7+3,
                       y * getHeight() / 6+2, getWidth() / 8, getHeight() / 7);
            break;
        }
      }
  }

  public int getUserClickColumn()
  {
    clickedColumn = -1;

    while((clickedColumn < 0) | (clickedColumn > 6))
      if (FourInRow.getGameManager().breakGame)
        return -1;

    return clickedColumn;
  }
}

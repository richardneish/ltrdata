
/**
 * Title:        <p>
 * Description:  <p>
 * Copyright:    Copyright (c) <p>
 * Company:      <p>
 * @author
 * @version 1.0
 */
package lab3;

import java.lang.Exception.*;

public class GameGrid
{
  static int[][] GameMatrix=new int[7][6];

  public static void clear(){//Nollar alla fält i matrisen
    for(int i=0; i<7; i++)
      for(int j=0; j<6; j++)
        GameMatrix[i][j]=0;

    FourInRow.getGameFrame().getGameBoard().repaint();
  }

  //Look up position ; retunera radnummer
  private static int LookupPosition(int column) //column kommer från player
    throws InvalidMoveException{
    for(int i=5; i>=0 ; i--)
      if(GameMatrix[column][i]==0)
        return i;

    throw new InvalidMoveException();
  /*
    int i =6;//kanske någon konstant?
    while (GameMatrix[column][i]!=0)
      i--;   // Letar upp första tomma position i kolumnen
    return i;*/
  }

  private static void insertMarker(int column, int row, int playerColor){
    GameMatrix[column][row]=playerColor;//colur=spelarens färg
    FourInRow.getGameFrame().getGameBoard().repaint();
  }

  //testa vad som händer OM PC skulle lägga på i, och går igenom listan tills
  //CheckWin, CheckLoose, (checkStategic), DoRand
  public static int testInsert(int playerColor){
    int columnValue[] = new int[7];

    // First strategy. Use previewCheckWinner() to check where we have
    // chance to win by one move.
    for(int column =0; column<7; column++)
    {
      try
      {
        columnValue[column] = previewCheckWinner(column, LookupPosition(column),
          playerColor);

        if (columnValue[column] >= 3)
          return column;
      }
      catch(InvalidMoveException e)
      {
      }
    }

    // Second, check where oponent have chance to win by one move and try to
    // block.
    int oponentColor;
    if (playerColor == 1)
      oponentColor = 2;
    else
      oponentColor = 1;

    for(int column =0; column<7; column++)
    {
      try
      {
        columnValue[column] = previewCheckWinner(column, LookupPosition(column),
          oponentColor);

        if (columnValue[column] >= 3)
          return column;
      }
      catch(InvalidMoveException e)
      {
      }
    }

    // Third, check where we have two markers
    for (int column = 0; column<7; column++)
      if (columnValue[column] == 2)
        return column;

    // Last strategy, begin at middle column and check for free row.
    for (int i = 0; i <= 3; i++)
    {
      int checkColumn1, checkColumn2;
      if (Math.random() < 0.5)
      {
        checkColumn1 = 3+i;
        checkColumn2 = 3-i;
      }
      else
      {
        checkColumn1 = 3-i;
        checkColumn2 = 3+i;
      }

      try
      {
        LookupPosition(checkColumn1);
        return checkColumn1;
      }
      catch(InvalidMoveException e)
      {
      }
      try
      {
        LookupPosition(checkColumn2);
        return checkColumn2;
      }
      catch(InvalidMoveException e)
      {
      }
    }

    // This will never happen, do random move.
    return (int)(Math.random() * 7);
  }

  public static int[][] getGameState(){
    return GameMatrix;
  }

  private static int checkBounds(int column, int row){

    try{//Returnerar koordinatens färg OM koordinaten finns. Generell catch all out of bounds
      return GameMatrix[column][row];
    }
    catch(IndexOutOfBoundsException e){
      return 3;//3 för att att xxxMarkers funktionerna skall kunna hantera det, 0,1,2 är reserverade
    }
  }

  private static int checkWinner(int column, int row){
    int noWinner=0,win=1, drawn=2;

    if (verticalMarkers(column,row,GameMatrix[column][row])>=3)
      return win;
    else if (horizontalMarkers(column,row,GameMatrix[column][row])>=3)
      return win;
    else if (forwardLeaningMarkers(column,row,GameMatrix[column][row])>=3)
      return win;
    else if (backwardsLeaningMarkers(column,row,GameMatrix[column][row])>=3)
      return win;

    for (int c=0; c<7; c++)
      for (int r=0; r<6; r++)
        if (GameMatrix[c][r] == 0)
          return noWinner;

    return drawn;
  }

  private static int previewCheckWinner(int column, int row, int playerColor){
    int verticals = verticalMarkers(column,row,playerColor);
    int horizontals = horizontalMarkers(column,row,playerColor);
    int forwards = forwardLeaningMarkers(column,row,playerColor);
    int backwards = backwardsLeaningMarkers(column,row,playerColor);

    if (verticals >= 3)
      return verticals;
    else if (horizontals >= 3)
      return horizontals;
    else if (forwards >= 3)
      return forwards;
    else if (backwards >= 3)
      return backwards;
    else if (verticals == 2)
      return verticals;
    else if (horizontals == 2)
      return horizontals;
    else if (forwards == 2)
      return forwards;
    else if (backwards == 2)
      return backwards;
    else
      return 0;
  }

  static int verticalMarkers(int x, int y, int playerColor){
    int counter=0;

    for (int i=y+1;i<6;i++)//6 pga att vi ändrar y-koordinat
      if (checkBounds(x,i)!=playerColor)
        break;
      else
        counter++;

    for (int i=y-1;i>=0;i--)//1 för att inte räkna startkoordinat 2 ggr
      if (checkBounds(x,i)!=playerColor)
        break;
      else
        counter++;

    return counter;
  }

  static int horizontalMarkers(int x, int y, int playerColor){
    int counter=0;

    for (int i=x+1;i<7;i++)//7 för att vi ändrar x-koordinat
      if (checkBounds(i,y)!=playerColor)
        break;
      else
        counter++;

    for (int i=x-1;i>=0;i--)//1 för att inte räkna startkoordinat 2 ggr
      if (checkBounds(i,y)!=playerColor)
        break;
      else
        counter++;

    return counter;
  }

  static int forwardLeaningMarkers(int x, int y, int playerColor){
    int counter=0;

    for (int i=1;i<7;i++)//7 för att vi ändrar x-koordinat
      if (checkBounds(x+i,y+i)!=playerColor)
        break;
      else
        counter++;

    for (int i=1;i<7;i++)//1 för att inte räkna startkoordinat 2 ggr
      if (checkBounds(x-i,y-i)!=playerColor)
        break;
      else
        counter++;

    return counter;
  }

  static int backwardsLeaningMarkers(int x, int y, int playerColor){
    int counter=0;

    for (int i=1;i<7;i++)
      if (checkBounds(x+i,y-i)!=playerColor)
        break;
      else
        counter++;

    for (int i=1;i<7;i++)
      if (checkBounds(x-i,y+i)!=playerColor)
        break;
      else
        counter++;

    return counter;
  }

  public static int doMove(int column, int playerColor)
    throws InvalidMoveException{
    if (column<7){
      int row=LookupPosition(column);
      insertMarker(column,row,playerColor);
      return checkWinner(column, row);
    }
    else
      throw new InvalidMoveException();
  }
}

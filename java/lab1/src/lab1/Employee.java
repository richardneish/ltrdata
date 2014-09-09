
/**
 * Title:        Lab 1
 * Description:  Projektplanering
 * Copyright:    Copyright (c)
 * Company:
 * @author       Andreas Jovid, Johan Lundberg,
 *               Michael Jonsson, Olof Lagerkvist
 * @version      1.0
 */
package lab1;

//Den h�r klassen ligger till grund f�r de olika typerna av anst�llda
public class Employee extends ProjectResource
{
  protected String name;

  protected double monthlyHours;
  protected double salary;
  protected double hourlyWage;

  /*Metoden anv�nds f�r att ber�kna kostnaden f�r viss arbetstid.
    Den fungerar f�r alla anst�llningstyper f�rutom projektanst�llning*/
  public double evalCost(double hours, double months)
  {
    if (hours < (months * monthlyHours))
      return 0;
    else
      return (months * salary) + (hours - (months * monthlyHours)) *
        (hourlyWage + (salary != 0 ? salary / monthlyHours : 0));
  }
}


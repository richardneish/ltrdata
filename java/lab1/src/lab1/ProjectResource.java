
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

//Klass för projektets resurser
public class ProjectResource
{
  protected double budgetHours;
  protected double budgetMonths;
  protected double actualHours;
  protected double actualMonths;

  //Lägger till den faktiska tiden
  public void setActual(double hours, double months)
  {
    actualHours = hours;
    actualMonths = months;
  }

  //Resultatutskrift
  public void print()
  {
    System.out.println("Namn: " + ((Employee)this).name +
      ", budgeterade timmar: " + budgetHours + " (" + getBudgetCost() +
      " kr), utförda timmar: " + actualHours + " (" +
      getActualCost() + " kr).");
  }

  //Hämtar budgeterade arbetstimmar
  public double getBudgetHours()
  {
    return budgetHours;
  }

  //Hämtar butdgeterad kostnad
  public double getBudgetCost()
  {
    return ((Employee)this).evalCost(budgetHours, budgetMonths);
  }

  //Hämtar faktisk arbetstid
  public double getActualHours()
  {
    return actualHours;
  }

  //Hämtar faktisk kostnad
  public double getActualCost()
  {
    return ((Employee)this).evalCost(actualHours, actualMonths);
  }
}


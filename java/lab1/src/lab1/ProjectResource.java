
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

//Klass f�r projektets resurser
public class ProjectResource
{
  protected double budgetHours;
  protected double budgetMonths;
  protected double actualHours;
  protected double actualMonths;

  //L�gger till den faktiska tiden
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
      " kr), utf�rda timmar: " + actualHours + " (" +
      getActualCost() + " kr).");
  }

  //H�mtar budgeterade arbetstimmar
  public double getBudgetHours()
  {
    return budgetHours;
  }

  //H�mtar butdgeterad kostnad
  public double getBudgetCost()
  {
    return ((Employee)this).evalCost(budgetHours, budgetMonths);
  }

  //H�mtar faktisk arbetstid
  public double getActualHours()
  {
    return actualHours;
  }

  //H�mtar faktisk kostnad
  public double getActualCost()
  {
    return ((Employee)this).evalCost(actualHours, actualMonths);
  }
}


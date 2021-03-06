
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
import java.util.Enumeration;
import java.util.Vector;

//Klass f�r projekt
public class Project
{
  private double budget;
  private double hours;

  private Vector Resources; //Inneh�ller projektets resurser

  //Konstruktorn
  public Project(double budgetInit, double hoursInit)
  {
    budget = budgetInit;
    hours = hoursInit;
    Resources = new Vector();
  }

  //L�gger till en resurs i vektorn
  public void addResource(ProjectResource resource)
  {
    Resources.add(resource);
  }

  //Utv�rderar projektet
  public void evaluate()
  {
    double sumBudgetCost = 0;
    double sumBudgetHours = 0;
    double sumActualCost = 0;
    double sumActualHours = 0;
    for (Enumeration e = Resources.elements(); e.hasMoreElements(); )
    {
      ProjectResource r = (ProjectResource) e.nextElement();
      r.print();
      sumBudgetCost += r.getBudgetCost();
      sumBudgetHours += r.getBudgetHours();
      sumActualCost += r.getActualCost();
      sumActualHours += r.getActualHours();
    }

    System.out.println("Totalt antal budgeterade timmar: " + sumBudgetHours +
      " (" + sumBudgetCost + " kr), utf�rda timmar: " + sumActualHours +
      " (" + sumActualCost + " kr).");
  }
}


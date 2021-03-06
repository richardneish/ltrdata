
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

//Det h�r �r klassen f�r projektanst�llda
class ProjectEmployee extends Employee
{
  public ProjectEmployee(String nameInit, double hoursInit, double salaryInit,
    double budgetHoursInit, double budgetMonthsInit)
  {
    budgetHours = budgetHoursInit;
    budgetMonths = budgetMonthsInit;
    name = nameInit;
    monthlyHours = hoursInit;
    salary = salaryInit;
    hourlyWage = 0;
  }

  /*�verskriver metoden f�r att r�kna ut kostnaden f�r
    en viss arbetstid som finns i klassen Employee*/
  public double evalCost(double hours, double months)
  {
    if (hours < monthlyHours)
      return 0;
    else
      return salary;
  }
}

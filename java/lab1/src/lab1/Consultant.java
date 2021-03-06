
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

//Det h�r �r klassen f�r konsulter
class Consultant extends Employee
{
  public Consultant(String nameInit, double hourlyWageInit,
    double budgetHoursInit, double budgetMonthsInit)
  {
    budgetHours = budgetHoursInit;
    budgetMonths = budgetMonthsInit;
    name = nameInit;
    monthlyHours = 0;
    salary = 0;
    hourlyWage = hourlyWageInit;
  }
}

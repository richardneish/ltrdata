
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

//Det h�r �r klassen f�r anst�llda
class PermanentEmployee extends Employee
{
  public PermanentEmployee(String nameInit, double monthlyHoursInit,
    double salaryInit, double overtimePayment,
    double budgetHoursInit, double budgetMonthsInit)
  {
    budgetHours = budgetHoursInit;
    budgetMonths = budgetMonthsInit;
    name = nameInit;
    monthlyHours = monthlyHoursInit;
    salary = salaryInit;
    hourlyWage = overtimePayment;
  }
}

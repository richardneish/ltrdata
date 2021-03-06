
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

// V�r fina Main-klass
public class Main
{
  public static void main(String[] arg)
  {
    System.out.println("\nProjektet Spock's Spaceport");

    // Skapar projektobjekt
    Project SpocksSpacePort = new Project(300000, 1000);

    // Skapa projektresursobjekt f�r varje anst�lld som ska arbeta i projektet
    ProjectResource kalleResource =
      new PermanentEmployee("Kalle", 160, 20000, 100, 320, 2);

    ProjectResource stinaResource =
      new PermanentEmployee("Stina", 160, 21000, 110, 320, 2);

    ProjectResource nisseResource =
      new Consultant("Nisse", 1000, 175, 2);

    ProjectResource albertResource =
      new ProjectEmployee("Albert", 185, 25000, 185, 2);

    // L�gg till projektresursobjekten i projektobjektet
    SpocksSpacePort.addResource(kalleResource);
    SpocksSpacePort.addResource(stinaResource);
    SpocksSpacePort.addResource(nisseResource);
    SpocksSpacePort.addResource(albertResource);

    // Skriv ut budget
    System.out.println("\nProjektets budget:");
    SpocksSpacePort.evaluate();

    // L�gg till faktiska tider i projektresursobjekten
    kalleResource.setActual(450, 2);
    stinaResource.setActual(400, 2);
    nisseResource.setActual(325, 2);
    albertResource.setActual(195, 2);

    // Skriv ut projektresultatet
    System.out.println("\nProjektets resultat:");
    SpocksSpacePort.evaluate();
  }
}

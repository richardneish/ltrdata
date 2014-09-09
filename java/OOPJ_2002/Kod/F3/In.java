/*Exemplet visar hur instr�mmarna kopplas och hur felhanteringen
  fungerar. Dessutom anv�nds klassen NumberFormat f�r att ta fram
  ett double-v�rde ur en str�ng skriven med svensk stil. Notera att
  hela try-avsnittet bryts d� ett undantag kastas och ekekveringen
  �terupptas efter det.*/

import java.io.*;
import java.text.*;
import java.util.*;

class In{

  public static double readDouble(BufferedReader in) throws IOException{
    while(true)
    try{
      System.out.print("Skriv in ett tal ");
      System.out.flush();
      String s=in.readLine();
      NumberFormat nf=NumberFormat.getInstance(new Locale("sv","se"));
      return nf.parse(s).doubleValue(); //parsar ett tal p� "svenska"
    }
    catch (ParseException pe){
      System.out.println("Felaktigt tal, f�rs�k igen");
    }
  }

  public static void main(String args[])throws IOException{
    double b=readDouble(new BufferedReader(new InputStreamReader(System.in)));
    System.out.println(b);
    System.in.read();
  }
}

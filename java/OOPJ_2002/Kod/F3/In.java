/*Exemplet visar hur inströmmarna kopplas och hur felhanteringen
  fungerar. Dessutom används klassen NumberFormat för att ta fram
  ett double-värde ur en sträng skriven med svensk stil. Notera att
  hela try-avsnittet bryts då ett undantag kastas och ekekveringen
  återupptas efter det.*/

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
      return nf.parse(s).doubleValue(); //parsar ett tal på "svenska"
    }
    catch (ParseException pe){
      System.out.println("Felaktigt tal, försök igen");
    }
  }

  public static void main(String args[])throws IOException{
    double b=readDouble(new BufferedReader(new InputStreamReader(System.in)));
    System.out.println(b);
    System.in.read();
  }
}

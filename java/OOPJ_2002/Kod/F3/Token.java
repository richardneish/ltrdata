/*Exemplet visar hur man kopplar strömmarna för att kunna läsa
  från tangentbordet. Dessutom visas hur StringTokenizer kan användas
  för att bryta upp en sträng*/

import java.util.*;
import java.io.*;

 public class Token{
  private String msg;

  public static void main(String [] args)throws IOException{
    Token t=new Token();
    BufferedReader reader=new BufferedReader(new InputStreamReader(System.in));
    System.out.print("Skriv in två tal med space (eller '+') mellan ");
    System.out.flush(); //to clear the buffer
    t.setExpression(reader.readLine());
    System.out.println(t.calc());
    System.in.read();
  }

  public void setExpression(String s){
    msg=s;
  }

  public double calc(){
    StringTokenizer x=new StringTokenizer(msg," +");//space or +
    return  (Double.valueOf(x.nextToken())).doubleValue()+
            (Double.valueOf(x.nextToken())).doubleValue();
  }
}


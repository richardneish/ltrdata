/*Exemplet visar hur man kopplar str�mmarna f�r att kunna l�sa
  fr�n tangentbordet. Dessutom visas hur StringTokenizer kan anv�ndas
  f�r att bryta upp en str�ng*/

import java.util.*;
import java.io.*;

 public class Token{
  private String msg;

  public static void main(String [] args)throws IOException{
    Token t=new Token();
    BufferedReader reader=new BufferedReader(new InputStreamReader(System.in));
    System.out.print("Skriv in tv� tal med space (eller '+') mellan ");
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


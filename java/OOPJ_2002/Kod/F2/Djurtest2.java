/*Exemplet visar hur en abstrakt klass används som superklass. Den
  abstrakta metoden printSound MÅSTE nu implementeras av subklasserna.
  Dessutom visas de inbyggda klasserna Vector och Enumeration.
  Flera filer är klistrade till en*/

import java.util.Vector;
import java.util.Enumeration;

abstract class Djur{
  protected String name;

  public Djur(String n){
    name=n;
  }

  public abstract void printSound();
} //slut klass Djur

class Ko extends Djur{

  public Ko(String n){
    super(n);
  }

  public void printSound(){
    System.out.println(name+" låter Muuuuuu");
  }
} //slut klass Ko

class Varg extends Djur{

  public Varg(String n){
    super(n);
  }

  public void printSound(){
    System.out.println(name+" låter YYYlllll");
  }
} //slut klass Varg


class Djurtest2{

  private Vector v;

  public Djurtest2(){
    v=new Vector();
  }

  public void makeVector(){
    v.addElement(new Ko("Rosa"));
    v.addElement(new Varg("Wille"));
    v.addElement(new Ko("Maja"));
    v.addElement(new Varg("Gråben"));
  }

  public static void main(String[] args) throws java.io.IOException{
    Djurtest2 d=new Djurtest2();
    d.makeVector();
    d.printOut();
    System.in.read();
    Djur d = new Djur();
  }

  public void printOut(){
    //Bortkommenterad kod visar på tekniken med enumerator
    //for(Enumeration e=v.elements();e.hasMoreElements();){
      int i=0; //remove if enumerator
      while(i<v.size()){ //remove if enumerator
      //Object o=e.nextElement();
      Object o=v.elementAt(i++);  //remove if enumerator
      if(o instanceof Djur)
         ((Djur)o).printSound();
    }
  }
} //slut klass Djurtest2






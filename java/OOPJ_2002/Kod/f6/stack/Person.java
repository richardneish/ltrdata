import java.io.IOException;
import java.util.Iterator;

public class Person{
  private String name;

  public Person(String s){
    name=s;
  }

  public String toString(){
    String s=new String(name);
    return s;
  }

  public static void main(String[] args)throws IOException{
    Stack s=new Stack(5); //Fält
    //StackL s=new StackL();//Länkad lista
    //StackI s=new StackI();  // Med Iterator
    s.push(new Ryss("Michailov")); // If you haven't...
    s.push(new Ryss("Petrov")); // seen them, you haven't...
    s.push(new Kanadick("Gretzky"));
    s.push(new Ryss("Charlamov")); // seen anything! CCCP rules
    s.push(new Kanadick("Lafleur"));
    Object o;
     //To test the iterator
  /*  for(Iterator i=s.iterator(); i.hasNext();){
      o=i.next();
      System.out.println(((Person)o).toString());
    }
    System.out.println("====================");*/
    while(!s.isEmpty()){
      o=s.pop();
      System.out.println(((Person)o).toString());
    }
  }
}

class Ryss extends Person{
  public Ryss(String s){
    super(s);
  }
}
class Kanadick extends Person{
  public Kanadick(String s){
    super(s);
  }
}

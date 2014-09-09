import java.util.*;
public class Mangd extends TreeSet{

  public Mangd union(Mangd m){
    Object o;
    Mangd ny=new Mangd();
    ny.addAll(this);
    ny.addAll(m);
   /* for(Iterator i=m.iterator();i.hasNext();){
      o=i.next();
      ny.add(o);
    }*/
    return ny;
  }

  public Mangd snitt(Mangd m){
    Object o;
    Mangd ny=new Mangd();
    for(Iterator i=iterator();i.hasNext();){
      o=i.next();
      if(m.contains(o))
        ny.add(o);
    }
    return ny;
  }

  public void print(){
    Object o;
    System.out.println("\n==========\n");
    for(Iterator i=iterator();i.hasNext();){
      o=i.next();
      System.out.println(o.toString());
    }
  }

  public static void main(String[] args)throws java.io.IOException{
    Mangd m1=new Mangd();
    m1.add(new Integer(1));
    m1.add(new Integer(2));
    m1.add(new Integer(3));
    m1.print();
    Mangd m2=new Mangd();
    m2.add(new Integer(2));
    m2.add(new Integer(3));
    m2.add(new Integer(4));
    Mangd m3=m1.union(m2);
    m3.print();
    Mangd m4=m1.snitt(m2);
    m4.print();

  }
}


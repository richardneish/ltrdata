// Egen iterator...

import java.util.*;

public class ParLista{
  public static void main(String[] args)throws java.io.IOException{
    ParList l=new ParList();
    l.add(new Par(new Person("Kalle"),new Person("Stina")));
    l.add(new Par(new Person("Sven"),new Person("Lisa")));
    l.add(new Par(new Person("Glenn"),new Person("Sara")));
    l.add(new Par(new Person("Bertil"),new Person("Doris")));
    for(Iterator i=l.iterator();i.hasNext();)
      System.out.println(((Person)i.next()).getNamn());
  }
}

class ParList extends ArrayList{
  public Iterator iterator(){
    return new ParIterator(this);
  }
}

class ParIterator implements Iterator{
  private ArrayList listan; //koppling till listan
  private boolean man=true;
  private int index=0;

  public boolean hasNext(){
    return index < listan.size();
  }

  public Object next(){ // Ger först alla män därefter alla kvinnor
    Object o = new Object();
    index++;
    if(man)
      o= (((Par)listan.get(index-1))).getMan();
    else
      o= (((Par)listan.get(index-1))).getKvinna();
    if(index==listan.size() && man==true){
      man=false;
      index=0;
    }
    return o;
  }

  public void remove(){
    listan.remove(listan.get(index));
  }

  public ParIterator(ArrayList l){
    listan=l;
    index=0;
  }
}

class Person{
  private String namn;

  public Person(String n){
    namn=n;
  }
  public String getNamn(){
    return namn;
  }
}

class Par{
  private Person man;
  private Person kvinna;

  public Par(Person m,Person k){
    man=m;
    kvinna=k;
  }
  public Person getMan(){
    return man;
  }
  public Person getKvinna(){
    return kvinna;
  }
}



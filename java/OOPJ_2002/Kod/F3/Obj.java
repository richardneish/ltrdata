/* Exemplet visar hur man kan spara objekt p� fil. Det som sparas �r i det h�r
fallet en hel Vector... Notera att persklassen implementerar Serializable*/

import java.util.*;
import java.io.*;

class Obj{
  private static Vector v=new Vector();

  public static void main(String[] args){
    v.addElement(new Pers("Carl",3));
    v.addElement(new Pers("Per",18));
    sparaObjekt(v,"C:\\temp\\test.dat");
    Vector test=new Vector();
    test=(Vector)l�sInObjekt("c:\\temp\\test.dat");
    System.out.println(((Pers)test.firstElement()).namn);
  }

  public static void sparaObjekt(Object obj, String namn){
    try{
      ObjectOutputStream out= new ObjectOutputStream
                              (new FileOutputStream(namn));
      out.writeObject(obj);
      out.close();
    }
    catch (IOException ie){
      ie.printStackTrace();
      System.exit(1);
    }
  }

  public static Object l�sInObjekt(String namn){
    Object obj=null;
    try{
      ObjectInputStream in= new ObjectInputStream
                            (new FileInputStream(namn));
      obj=in.readObject();
      in.close();
    }
    catch (IOException ie){
      ie.printStackTrace();
      System.exit(1);
    }
    catch (ClassNotFoundException ce){
      ce.printStackTrace();
      System.exit(1);
    }
   return obj;
  }
}

class Pers implements Serializable{
  String namn;
  int �lder;

  public Pers(String n,int �){
    namn=n;
    �lder=�;
  }
}


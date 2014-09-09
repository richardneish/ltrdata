/*Exemplet visar hur man kan lagra primitiver av olika typer i en
  samling genom att använda wrapperklasserna. Dessutom används vissa
  metoder som hör till Object och som därför kan utnyttjas av alla
  object.*/

import java.util.Vector;
class Wrap{
  private static Vector v=new Vector();

  public static void main(String[] args){
    v.addElement(new Integer(5));
    v.addElement(new Double(7.5));
    v.addElement(new Boolean(1==1));
    int i=0;
    while(i<v.size()){
      System.out.println(v.elementAt(i).getClass()+
                    " "+v.elementAt(i).toString());
      i++;
    }
  }
}





/*Det här exemplet visar på enkel polymorfism.
  Två metoder har samma namn och kompilatorn väljer runtime vilken
  den skall använda utifrån anropande objekt. Notera att polymorfa
  funktioner verkligen skall ha samma funktion. */

public class Poly{
  public static void main(String[] args) throws java.io.IOException{
    Kva k=new Kva(3);
    Cir c=new Cir(3);
    System.out.println("Cirkelns area är "+c.calcArea());
    System.out.println("Kvadratens area är "+k.calcArea());
  }
}

class Cir{
  int r;
  public Cir(int radie){
    r=radie;
  }
  public double calcArea(){
    return r*r*Math.PI;
  }
}

class Kva{
  int s;
  public Kva(int sida){
    s=sida;
  }
  public double calcArea(){
    return s*s;
  }
}
/*Det h�r exemplet visar p� enkel polymorfism.
  Tv� metoder har samma namn och kompilatorn v�ljer runtime vilken
  den skall anv�nda utifr�n anropande objekt. Notera att polymorfa
  funktioner verkligen skall ha samma funktion. */

public class Poly{
  public static void main(String[] args) throws java.io.IOException{
    Kva k=new Kva(3);
    Cir c=new Cir(3);
    System.out.println("Cirkelns area �r "+c.calcArea());
    System.out.println("Kvadratens area �r "+k.calcArea());
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
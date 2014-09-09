/*Det här exemplet visar på arv. Klassen kvadrat ärver från rektangel
  och får därmed dess attribut och metoder. Notera dock att attributet måste
  vara protected (inte private) om det skall kunna nås från subklassen.*/
public class Testarv{
  public static void main(String[] args){
    Rektangel r=new Rektangel(3,4);
    Kvadrat k=new Kvadrat(5);
    r.dispArea();
    k.dispArea();
  }
}

class Rektangel{
  protected double height,width;

  public Rektangel(double h,double w){
    height=h;
    width=w;
  }

  public void dispArea(){
    System.out.println("Arean är "+height*width);
  }
}

class Kvadrat extends Rektangel{

  public Kvadrat(double side){
    super(side,side);
  }

  public double getSide(){
    return height;
  }
}

/*Det h�r exemplet visar p� arv. Klassen kvadrat �rver fr�n rektangel
  och f�r d�rmed dess attribut och metoder. Notera dock att attributet m�ste
  vara protected (inte private) om det skall kunna n�s fr�n subklassen.*/
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
    System.out.println("Arean �r "+height*width);
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

/*Exemplet visar polymorfism och överskrivning av metoder. De olika
  djuren ligger i en vektor av typen Djur och anropar sedan en i taget
  metoden printSound. Denna metod ligger i superklassen Djur. För
  masken Berit är dock printSound överskriven eftersom den följer en annan mall.
  Det generella ligger i superklassen medan specialfallet tas hand om med en
  överskrivning. Notera också hur inbyggda fält hanteras. */

class Djur{
  protected String name;
  private String läte;

  public Djur(){
  }

  public Djur(String n,String l){
    name=n;
    läte=l;
  }

  public void printSound(){
  System.out.println(name+" låter så här "+läte);
  }
}

class Ko extends Djur{


  public Ko(String n){
    super(n,"Muuuuuuuuu");
  }
}

class Varg extends Djur{

  public Varg(String n){
    super(n,"Yyyyyyyyl");
  }
}

class Mask extends Djur{


  public Mask(String n){
    super(n,null);
  }

  public void printSound(){
  System.out.println(name+" har inget läte. Stackars "+name);
  }
}

class Djurtest1{
  private static Djur[] v=new Djur[10];
  public static void main(String[] args) throws java.io.IOException{
    v[0]=new Ko("Rosa");
    v[1]=new Varg("Wille");
    v[2]=new Ko("Maja");
    v[3]=new Varg("Gråben");
    v[4]=new Mask("Berit");
    printOut(v);
    System.in.read();
  }

  public static void printOut(Djur[] d){
    int i=-1;
    while(d[++i]!=null)
      d[i].printSound();
  }
}






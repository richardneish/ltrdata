/*Exemplet visar hur en dubbelriktad association implementeras som
  referenser i b�da objekten.*/
  
class Person {
  private String namn, adress;
  private Person makeMaka;

  public Person(String n){
    namn=n;
  }

  public void s�ttAdress(String a){
    adress=a;
  }

  public String avl�sNamn(){
    return namn;
  }

  public String avl�sAdress(){
    return adress;
  }

  public void br�lopp(Person p){
    makeMaka=p;
    p.makeMaka=this;
  }

  public void skilsm�ssa(Person p){
    p.makeMaka=null;
    makeMaka=null;
  }

  public Person giftMed(){
    return makeMaka;
  }

  public static void main(String[] args) throws java.io.IOException{
    Person p1=new Person("Carl");
    Person p2=new Person("Theresia");
    p1.s�ttAdress("Drakens gr�nd 11");
    p1.br�lopp(p2);
    System.out.println(p2.giftMed().adress);
    System.in.read();
  }
}

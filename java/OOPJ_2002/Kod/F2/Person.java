/*Exemplet visar hur en dubbelriktad association implementeras som
  referenser i båda objekten.*/
  
class Person {
  private String namn, adress;
  private Person makeMaka;

  public Person(String n){
    namn=n;
  }

  public void sättAdress(String a){
    adress=a;
  }

  public String avläsNamn(){
    return namn;
  }

  public String avläsAdress(){
    return adress;
  }

  public void brölopp(Person p){
    makeMaka=p;
    p.makeMaka=this;
  }

  public void skilsmässa(Person p){
    p.makeMaka=null;
    makeMaka=null;
  }

  public Person giftMed(){
    return makeMaka;
  }

  public static void main(String[] args) throws java.io.IOException{
    Person p1=new Person("Carl");
    Person p2=new Person("Theresia");
    p1.sättAdress("Drakens gränd 11");
    p1.brölopp(p2);
    System.out.println(p2.giftMed().adress);
    System.in.read();
  }
}

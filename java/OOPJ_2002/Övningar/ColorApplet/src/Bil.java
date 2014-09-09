// Filen Person.java

class Person {
  protected String na, adr;   // Obs! protected
  protected int �r;           // Obs! protected

  protected Person() {} // m�ste finnas f�r subklassens skull

  public Person(String namn, String adress, int �lder) {
    na = namn;
    adr = adress;
    �r = �lder;
  }

  public String heter() {
    return na;
  }

  public String bor() {
    return adr;
  }

  public int  hur_gammal() {
    return �r;
  }
}

class Bilagare extends Person {
  private Bil bilen;

  public Bilagare(String namn, String adress, int �lder) {
    na = namn;
    adr = adress;
    �r = �lder;
  }

  public Bil �ger() {
    return bilen;
  }

  public void s�ljBil() {
    if (bilen != null) {
      bilen.s�ld();
      bilen = null;
    }
  }

  public void k�pBil(Bil nyBil) {
    if (nyBil.�gsAv() == null) {
      bilen = nyBil;
      bilen.k�pt(this);
    }
  }

}

class Bil {
  private String regNr;
  private String bilm�rke;
  private Bilagare �gare;

  public Bil(String nr, String fabrikat) {
    regNr = nr;
    bilm�rke = fabrikat;
  }

  public String toString () {
    return regNr + " " + bilm�rke;
  }

  public Bilagare �gsAv() {
    return �gare;
  }

  public void s�ld() {   // skall endast anropas av bilens �gare
      �gare = null;
  }

  public void k�pt(Bilagare ny�gare) {  // skall endast anropas av bilens �gare
      �gare = ny�gare;
  }
}
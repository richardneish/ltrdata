/*Exemplet visar hur man kastar olika undantag som sedan fångas upp
  i catch-klausuler. Observera ordningen på catch-delarna. En superklass
  fångar ju annars även subklassfel. Notera också att det här egentligen
  inte är något bra exemple. Det är ju inga fel som hanteras utan se det
  bara som exempel på tekniken...*/

class Try {

  public static void burr(int tal){
    boolean d,t,e;
    for(int i=1;i<=100;i++){
      d=i%tal==0; //delbart
      t=i/10==tal; //tiotalssiffran
      e=i-i/10*10==tal; //entalssiffran
      try{
        if(d&&t&&e)
          throw new TripleBurrException();
        if(d&&t || d&&e || e&&t)
          throw new DoubleBurrException();
        if(d||t||e)
          throw new BurrException();
      System.out.println(i);
      }
      //Notera ordningen på catch-klausulerna
      catch (TripleBurrException tbe){
        System.out.println("BurrBurrBurr");
      }
      catch (DoubleBurrException dbe){
        System.out.println("BurrBurr");
      }
      catch (BurrException be){
        System.out.println("Burr");
      }
    }
  }

  public static void main(String args[]){
    burr(3);
  }
}

class BurrException extends Exception{
}

class DoubleBurrException extends BurrException{
}

class TripleBurrException extends DoubleBurrException{
}




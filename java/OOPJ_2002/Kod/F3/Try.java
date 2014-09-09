/*Exemplet visar hur man kastar olika undantag som sedan f�ngas upp
  i catch-klausuler. Observera ordningen p� catch-delarna. En superklass
  f�ngar ju annars �ven subklassfel. Notera ocks� att det h�r egentligen
  inte �r n�got bra exemple. Det �r ju inga fel som hanteras utan se det
  bara som exempel p� tekniken...*/

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
      //Notera ordningen p� catch-klausulerna
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




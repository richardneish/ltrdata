/*Exemplet visar hur man med metoden komp hanterar ett tecken i
  taget. Detta påminner mycket om C. Notera dock de till String
  hörande metoderna length och charAt. I komp2 behandlar man
  fortfarande indata ett tecken i taget men en StringBuffer byggs
  upp efterhand med append.*/

class Pack{
  public static void komp(String txt){
    int i=0, antal=1;
    while(i<txt.length()-1){
      if(txt.charAt(i)==txt.charAt(i+1))
        antal++;
      else{
        System.out.print(antal);
        System.out.print(txt.charAt(i));
        antal=1;
      }
      i++;
    }
    System.out.print(antal);
    System.out.print(txt.charAt(i));
  }

  public static void komp2(String txt){
    StringBuffer s=new StringBuffer();
    int i=0, antal=1;
    while(i<txt.length()-1){
      if(txt.charAt(i)==txt.charAt(i+1))
        antal++;
      else{
        s.append(antal);
        s.append(txt.charAt(i));
        antal=1;
      }
      i++;
    }
    s.append(antal);
    s.append(txt.charAt(i));
    System.out.println("\n"+s);
  }

  public static void main(String[] args){
    String s="aaabbbbcdde";
    komp(s);
    String s2="ghhhhhhhiij";
    komp2(s2);
  }
}
  

public class Acc2 {
  // Detta �r klassvariabler
  private static double interestRate=3.5; //r�ntesats
  private static int noOfAcc2s; //antal skapade konton
  //Detta �r klassmetoder
  public static double getInterestRate(){ //l�s av r�ntesatsen
    return interestRate;
  }

  public static void changeRate(double newRate){ //byt r�ntesats
    interestRate=newRate;
  }

  //H�r �r attributen (instansvariablerna)
  private String Acc2No;    //kontonummer
  private double balance,accInterest; //saldo och ackumulerad r�nta

  //Konstruktorer
  public Acc2(String no,double initialSum){//saldo <> 0 fr�n start
    Acc2No=no;
    balance=initialSum;
    Acc2.noOfAcc2s++;  //ett nytt konto har skapats
  }

  public Acc2(String no){
    this(no,0);  //anropa den andra konstruktorn med saldo=0
  }

  //Metoder
  public void deposit(double amount){ //ins�ttning
    balance+=amount;
  }

  public void withdraw(double amount)throws MissingMoneyE{//uttag
      if(balance<amount)
        throw new MissingMoneyE("Deposit failed");
      balance-=amount;
  }

  public void transfer(Acc2 to,double amount)throws MissingMoneyE{ //�verf�ring
      withdraw(amount);
      to.deposit(amount);
  }

  public void calcDayInterest(){  //dagsr�nta
    accInterest+=balance*interestRate/100/365;
  }

  public void addInterest(){ //l�gg p� r�ntan
    balance+=accInterest;
    accInterest=0;
  }

  public void showInfo(){ //skriv info om ett konto
    System.out.println("Acc2: "+Acc2No+" Balance: "+balance);
  }

  public static void main(String[] args) throws java.io.IOException {
    try{
    Acc2 myAcc2=new Acc2("ABC123",10000); //skapa konton
    Acc2 yourAcc2=new Acc2("DEF456");
    myAcc2.showInfo();  //anropar metoden showInfo
    myAcc2.deposit(1000);
    myAcc2.showInfo();
    yourAcc2.withdraw(1000);
    myAcc2.transfer(yourAcc2,5000);//transfer med yourAcc2 som arg
    myAcc2.showInfo();
    yourAcc2.showInfo();
    Acc2.changeRate(4.0);  //anropar klassmetod
    }
    catch (MissingMoneyE e){
      System.out.println(e.toString());
    }
  }
}

class MissingMoneyE extends Exception{
  public MissingMoneyE(String s){
    super(s);
  }
}


public class Account {
  // Detta �r klassvariabler
  private static double interestRate=3.5; //r�ntesats
  private static int noOfAccounts; //antal skapade konton
  //Detta �r klassmetoder
  public static double getInterestRate(){ //l�s av r�ntesatsen
    return interestRate;
  }

  public static void changeRate(double newRate){ //byt r�ntesats
    interestRate=newRate;
  }

  //H�r �r attributen (instansvariablerna)
  private String accountNo;    //kontonummer
  private double balance,accInterest; //saldo och ackumulerad r�nta

  //Konstruktorer
  public Account(String no,double initialSum){//saldo <> 0 fr�n start
    accountNo=no;
    balance=initialSum;
    Account.noOfAccounts++;  //ett nytt konto har skapats
  }

  public Account(String no){
    this(no,0);  //anropa den andra konstruktorn med saldo=0
  }

  //Metoder
  public void deposit(double amount){ //ins�ttning
    balance+=amount;
  }

  public boolean withdraw(double amount){//uttag
    boolean ok=false;
    if(balance<amount)
      System.out.println("Not enough money on the account "+accountNo);
    else{
      balance-=amount;
      ok=true;
    }
    return ok;
  }

  public void transfer(Account to,double amount){ //�verf�ring
    if(withdraw(amount))
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
    System.out.println("Account: "+accountNo+" Balance: "+balance);
  }

  public static void main(String[] args) throws java.io.IOException {
    Account myAccount=new Account("ABC123",10000); //skapa konton
    Account yourAccount=new Account("DEF456");
    myAccount.showInfo();  //anropar metoden showInfo
    myAccount.deposit(1000);
    myAccount.showInfo();
    yourAccount.withdraw(1000);
    myAccount.transfer(yourAccount,5000);//transfer med yourAccount som arg
    myAccount.showInfo();
    yourAccount.showInfo();
    Account.changeRate(4.0);  //anropar klassmetod
  }
}

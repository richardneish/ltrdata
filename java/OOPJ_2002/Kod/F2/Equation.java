/*Exemplet visar hur ett aggregat implementeras. I den här modelleringen
  består en ekvation av tre koefficienter och två rötter.*/

class Equation {
  private Coefficient A,B,C;
  private Root R1,R2;

  public void read(){
    //Normally read from the keyboard...
    A=new Coefficient(1);
    B=new Coefficient(2);
    C=new Coefficient(1);
  }

  public void calculate() {
      if (!complex()){
      double P = (double) B.get() / A.get();
      double Q = (double) C.get() / A.get();
      R1 = new Root (-P/2+Math.sqrt(P*P/4-Q));
      R2 = new Root (-P/2-Math.sqrt(P*P/4-Q));
      }
  }

  public void print(){
    if (!complex()){
      System.out.println("Root 1 = " + R1.get());
      System.out.println("Root 2 = " + R2.get());
    }
    else
      System.out.println("The equation lacks solution!");
  }

  boolean complex() {
    double P = (double)B.get()/ A.get();
    double Q = (double)C.get()/ A.get();
    return ( (P*P/4.0 - Q)  < 0) ;
  }


  public static void main(String[] args)throws java.io.IOException{
    Equation  equ=new Equation();
    equ.read();
    equ.calculate();
    equ.print();
    System.in.read();
  }
}

class Coefficient {
  private int value;

  public Coefficient(int a){
  value=a;
  }

  public int get(){
    return value;
  }
}

class Root {
  private double value;

  public Root(double v){
    value=v;
  }

  public double get(){
    return value;
  }
}


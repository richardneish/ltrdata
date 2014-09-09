/*Exemplet visar hur man kastar undantag. Det första kastar ett
  undantag av typen IllegalArgument. Detta ärver från runtime så det
  behöver inte specificeras. Därefter visas en egen undantagsklass;
  SmallException. */

class Circle{
  private double radius;

  public Circle(double r){
    if(r<0)
      throw new IllegalArgumentException("Radius must be positive");
    radius=r;
  }

  public void showArea()throws SmallException{
    double area=3.14159*radius*radius;
    if (area<100)
      throw new SmallException("Area must be over 100");
    System.out.println("Area is "+area);
  }

  public static void main(String[] args) throws SmallException{
    int a=40,b=-1,c=2;
    Circle c1=new Circle(a);
    c1.showArea();
    Circle c2=new Circle(b);
    c2.showArea();
    Circle c3=new Circle(c);
    c3.showArea();
  }
}

class SmallException extends Exception{

  public SmallException(){
    super();
  }
  
  public SmallException(String s){
    super(s);
  }
}




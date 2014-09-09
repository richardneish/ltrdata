class Flight{
  private String no, destination, comment="";
  private Tidpunkt dep, arr;

  public Flight(String flightNo, String dest,int depHour,int depMin,
                int arrHour, int arrMin){
    no=flightNo;
    destination=dest;
    dep=new Tidpunkt(depHour,depMin);
    arr=new Tidpunkt(arrHour,arrMin);
  }

  public void setComment(String com){
    comment=com;
  }

  public void delay(int min){
    dep.ticka(min*60);
    arr.ticka(min*60);
    setComment("Delayed");
  }

  public Tidpunkt getDep(){
    return new Tidpunkt(dep.avläsTim(),dep.avläsMin());
  }

  public Tidpunkt getArr(){
    return new Tidpunkt(arr.avläsTim(),arr.avläsMin());
  }

  public String getDestination(){
    return destination;
  }

  public String toString(){
    return no+" "+destination+" "+dep+" "+comment;
  }
}

class Tidpunkt{
  private int t, m, s;
  private boolean visaSek=true;

  public Tidpunkt(){}

  public Tidpunkt (int tim, int min, int sek){
    sätt(tim,min,sek);
  }

  public Tidpunkt(int tim, int min){
    this(tim,min,0);
    visaSek=false;
  }

  public Tidpunkt(int tim){
    this(tim,0);
  }

  public void sätt(int tim, int min, int sek){
    if(tim>=0 && tim<24 && min>=0 && min<60 &&sek>=0 &&sek<60){
      t=tim; m=min; s=sek;
    }
    else
      System.out.println("Felaktig tidpunkt");
  }

  public void sättVisaSek(boolean visa){
    visaSek=visa;
  }

  public int avläsTim(){
    return t;
  }

  public int avläsMin(){
    return m;
  }

  public int avläsSek(){
    return s;
  }

  public void ticka(){
    s++;
    if(s==60){
      s=0;
      m++;
    }
    if(m==60){
      m=0;
      t++;
    }
    if(t==24)
      t=0;
  }

  public void ticka(int antalSek){
    while (antalSek>0){
      ticka();
      antalSek--;
    }
  }

  public String toString(){
    String tid=t+":"+m;
    if(visaSek)
      tid=tid+":"+s;
    return tid;
  }
}


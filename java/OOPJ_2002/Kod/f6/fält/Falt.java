import java.util.*;
import java.math.*;

public class Falt{
  private static final int ANTAL = 1000;
  private static int[] f = new int[ANTAL];

  public static void main(String[] args){
    //strange();
   // sieve();
   //printFält();

    for(int i=0;i<ANTAL;i++)
      f[i]=((int)(Math.random()*ANTAL));
    printFält();
    System.out.println("\n\nSORTERAR NU \n\n =======\n");
    Arrays.sort(f);
    printFält();
  }

  public static void printFält(){
    for(int i=0;i<ANTAL;i++){
      System.out.print(f[i]+" ");
    if(i%1000==0)
      System.out.println();
    }
  }

  public static void sieve(){
    for(int i=2;i<ANTAL;i++){
      if(f[i]==0){
       // f[i]=1; //prime
        System.out.println(i+" ");
        for(int j=i;j<ANTAL;j+=i)
          f[j]=-1; //not prime
      }
    }
    System.exit(1);
  }

  public static int euklid(int a, int b){//a större än b
    if(b==0)
      return(a);
    else
      return(euklid(b,a%b));
  }

  public static boolean coprime(int a, int b){
    if(a>=b)
      return (euklid(a,b)==1);
    else
      return (euklid(b,a)==1);
  }

  public static void strange(){
    int cop=0,x,y;
    int times=10000000; //öka antalet nollor

    for(int i=1;i<times;i++){
      x=((int)(Math.random()*ANTAL+2));
      y=((int)(Math.random()*ANTAL+2));
      if(coprime(x,y))
        cop++;
    }

    double d= Math.sqrt( 6/((double)cop/times));
    System.out.println(d);
  }




}
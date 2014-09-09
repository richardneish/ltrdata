/*Det här exemplet vill visa hur man bygger upp en klass och hur
  gränssnittet är klassens yta utåt. Notera att Stack fungerar utåt
  som man kan förvänta sig. Operationerna (metoderna) är push,pop etc.
  Den inre implementationen är ointressant när klassen väl
  fungerar. Metoder som bara används internt har privatdeklarerats
  Egentligen borde pop returnera ett element...*/

public class Stack{
  private int[] field;
  private int maxElements;

  public Stack(){
    this(10);
  }

  public Stack(int max){
    field=new int[max];
    field[0]=-1;
    maxElements=max;
  }

  private int findSize(){
    int i=0;
    while(field[i]!=-1)
      i++;
    return i;
  }

  private boolean isEmpty(){
    return(findSize()==0);
  }

  private boolean isFull(){
    return(findSize()==maxElements-1);
  }

  public void push (int a){
    if (isEmpty()){
      field[0]=a;
      field[1]=-1;
      System.out.println(a+" is pushed to stack");
    }
    else{
      if(isFull())
        System.out.println("Stack is full");
      else{
        int number=this.findSize();
        for(int j=number;j>=0;j--)
          field[j+1]=field[j];
        field[0]=a;
        System.out.println(a+" is pushed to stack");
      }
    }
  }

  public void pop(){
    if(isEmpty())
      System.out.println("Stack is empty");
    else{
      System.out.println(field[0]+" is popped");
      int i=0;
      while(field[i]!=-1){
        field[i]=field[i+1];
        i++;
      }
    }
  }

  public void print(){
   if(isEmpty())
      System.out.println("Stack is empty");
   else{
      int i=0;
      System.out.print("Here is stack:");
      while(field[i]!=-1){
        System.out.print(field[i]+" ");
        i++;
      }
      System.out.println();
   }
  }

  public static void main(String[] args)throws java.io.IOException{
    Stack s=new Stack();
    s.print();
    s.push(1);
    s.push(3);
    s.push(5);
    s.print();
    s.pop();
    s.print();
  }
}


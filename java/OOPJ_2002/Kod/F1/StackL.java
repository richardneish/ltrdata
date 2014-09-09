/* Stack som en länkad lista men med samma gränssnitt */

public class StackL{
  private Node head;

  public StackL(){
    head=null;
  }

  private boolean isEmpty(){
    return(head==null);
  }

  public void push (int a){
    Node n=new Node(a);
    if (isEmpty())
      head=n;
    else{
      n.link=head;
      head=n;
    }
    System.out.println(a+" is pushed to stack");
  }

  public void pop(){
    if(isEmpty())
      System.out.println("Stack is empty");
    else{
      System.out.println(head.info+" is popped");
      head=head.link;
    }
  }

  public void print(){
   if(isEmpty())
      System.out.println("Stack is empty");
   else{
      System.out.print("Here is stack:");
      Node e=head;
      do{
        System.out.print(e.info+" ");
        e=e.link;
      }while (e!=null);
      System.out.println();
   }
  }

  public static void main(String[] args){
    Stack s=new Stack();
    s.print();
    s.push(2);
    s.push(4);
    s.push(6);
    s.print();
    s.pop();
    s.pop();
    s.print();
  }
}

class Node{
  int info;
  Node link;

  public Node(int n){
    info=n;
    link=null;
  }
}

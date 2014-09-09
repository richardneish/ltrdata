/* En stack internt representerad som länkad lista */
import java.util.Iterator;

class StackL{
  private Node head;

  public StackL(){
    head=null;
  }

  class Node{  // inre klass
    Object info;
    Node link;

    public Node(Object o){
      info=o;
      link=null;
    }
  }

  public boolean isEmpty(){
    return(head==null);
  }

  public void push(Object o){
    Node n=new Node(o);
    if (isEmpty())
      head=n;
    else{
      n.link=head;
      head=n;
    }
  }

  public Object pop(){
    if(isEmpty())
      return null;
    else{
      Object o=head.info;
      head=head.link;
      return o;
    }
  }
}





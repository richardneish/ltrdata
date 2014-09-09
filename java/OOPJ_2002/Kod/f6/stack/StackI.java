/* En stack med en egen iterator*/
import java.util.Iterator;

class StackI{
  private Node head;
  private Node current; // for the iterator

  public StackI(){
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

  public Iterator iterator(){
    current=head;
    return new StackIterator();
  }

  class StackIterator implements Iterator{

    public boolean hasNext(){
      return current != null;
    }

    public Object next(){
      Object o=current.info;
      current=current.link;
      return o;
    }

    public void remove(){/*not implemented*/}
  } // end Iterator
}// end StackI




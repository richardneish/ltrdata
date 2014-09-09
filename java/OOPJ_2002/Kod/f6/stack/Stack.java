/* Stack internt representerd som fält */

class Stack{

  private Object[] stack;
  private int elements;

  public Stack(int size){
    stack=new Object[size];
  }

  public Stack(){
    stack=new Object[10];
  }

  public boolean isEmpty(){
    return elements==0;
  }

  public void push(Object o){
    if(elements<stack.length){
      for(int i=elements;i>0;i--)
        stack[i]=stack[i-1];
      stack[0]=o;
      elements++;
    }
    else
      System.out.println("Stack full");
  }

  public Object pop(){
    if (!isEmpty()){
      Object o=stack[0];
      for(int i=0;i<elements-1;i++)
        stack[i]=stack[i+1];
      stack[--elements]=null;
      return o;
    }
    else
      return null;
  }
}









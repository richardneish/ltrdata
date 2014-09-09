class Tree{
  Node root;

  public Tree(){
    root=null;
  }

   public void addNode(Object o){
    Node newNode=new Node(o);
    if(root!=null)
      addToTree(newNode,root);
    else
      root=newNode;
  }

  private void addToTree(Node newNode, Node pos){
    if(newNode.mindreÄn(pos)){
      if(pos.left==null)
        pos.left=newNode;
      else
        addToTree(newNode,pos.left);
    }
    else{
      if(pos.right==null)
        pos.right=newNode;
      else
        addToTree(newNode,pos.right);
    }
  }

 /* public void mem(int info){
    if(member(info))
      System.out.println("medlem "+info);
    else
      System.out.println("icke medlem "+info);
  }

   private boolean member(int info){
    if(root==null)
      return false;
    else
      return(check_member(root,info));
  }

 private boolean check_member(Node pos, int key){
    boolean answer=false;
    if(key<pos.info){
      if(pos.left==null)
        answer=false;
      else
        answer=check_member(pos.left,key);
    }
    else if(key>pos.info){
      if(pos.right==null)
        answer=false;
      else
        answer=check_member(pos.right,key);
    }
    else if(key==pos.info)
      answer=true;
    return answer;
  }
    */
  public void writeTree(){
    System.out.println("\nInorder");
    inorder(root);
    System.out.println("\nPreorder");
    preorder(root);
    System.out.println("\nPostorder");
    postorder(root);
  }

  private void inorder(Node pos){
    if(pos!=null){
      inorder(pos.left);
      System.out.println(pos.info+"");
      inorder(pos.right);
    }
  }

  private void preorder(Node pos){
    if(pos!=null){
      System.out.println(pos.info+"");
      preorder(pos.left);
      preorder(pos.right);
    }
  }

  private void postorder(Node pos){
    if(pos!=null){
      postorder(pos.left);
      postorder(pos.right);
      System.out.println(pos.info+"");
    }
  }

  public static void main(String[] args)throws java.io.IOException{
    Tree t=new Tree();
    t.addNode(new Integer(5));
    t.addNode(new Integer(4));
    t.addNode(new Integer(6));
    t.addNode(new Integer(1));
    //t.mem(6);
    //t.mem(9);
    t.writeTree();
    //System.in.read();
  }
}

class Node{
  Object info;
  Node left;
  Node right;

  public Node(Object i){
    left=right=null;
    info=i;
  }

  public boolean mindreÄn(Node n){
    return ((Integer)info).compareTo(n.info)<0;   //downcast "för hand"
  }
}



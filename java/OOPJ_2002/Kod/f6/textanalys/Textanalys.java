import java.util.*;
import java.text.*;
import java.io.*;

public class Textanalys {
  public static void main(String[] arg)throws java.io.IOException{
    int c;
    Character ch;
    TreeMap bokstavsTabell=new TreeMap(); //naturligt jämförbara
    try{
      FileReader srcStream = new FileReader("c:\\arbete\\java\\krypto\\c1c.txt");
      while ((c = srcStream.read()) != -1){
        ch=new Character((char)c);
        Integer antal=(Integer)bokstavsTabell.get(ch);
        if(antal==null)
          antal=new Integer(0);
        bokstavsTabell.put(ch,new Integer(antal.intValue()+1));
      }
    }
    catch (FileNotFoundException e){
	System.err.println("file not found");
    }

    //skapa sorterad mängd med alla par av bokstäver, antal
    Set avbildningar=bokstavsTabell.entrySet();
    for(Iterator i=avbildningar.iterator();i.hasNext(); ){
      Map.Entry e=(Map.Entry) i.next();
      System.out.println(e.getKey() + " "+e.getValue());
    }
  }
}


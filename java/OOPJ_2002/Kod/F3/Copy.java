/*Exemplet visar två sätt att läsa och skriva till filer. I första
  delen läses ett tecken i taget och i andra en rad. Notera hur strömmar
  kopplas. Notera också att jag använt kommandoradsargumenten vilka i
  utvecklingsmiljön sätts i file-project properties. Det första
  argumentet är INTE programmets namn.*/

  import java.io.*;

/*class Copy{
	public static void main(String[] arg){
		try{
			File srcFile = new File(arg[0]);
			File destFile = new File(arg[1]);
			FileReader srcStream = new FileReader(srcFile);
			FileWriter destStream = new FileWriter(destFile);
      int c;
      while ((c = srcStream.read()) != -1)
        destStream.write(c);
			srcStream.close();
			destStream.close();
		}
		catch (FileNotFoundException e){
		    System.err.println("file not found:"+arg[0] );
		}
		catch (IOException e){
		    System.err.println("I/O error:" + e.getMessage());
		}
		catch (ArrayIndexOutOfBoundsException e){
			System.err.println("<usage: pgm srcFile destFile>");
    }
	}
} */

class Copy{
	public static void main(String[] arg){
      try{
        BufferedReader inFil=new BufferedReader
        (new FileReader(arg[0]));
        PrintWriter utFil=new  PrintWriter(new BufferedWriter
        (new FileWriter(arg[1])));
        String rad;
        while ((rad=inFil.readLine())!=null)
        utFil.println(rad);
        utFil.close();
      }

		catch (FileNotFoundException e){
		    System.err.println("file not found:"+arg[0] );
		}
		catch (IOException e){
		    System.err.println("I/O error:" + e.getMessage());
		}
		catch (ArrayIndexOutOfBoundsException e){
			System.err.println("<usage: pgm srcFile destFile>");
    }
	}
}

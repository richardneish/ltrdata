import java.applet.*;
import java.awt.*;

public class myapplet extends Applet{
   private String s;

	public void init(){
		s = "init";
		trace();
	}

	public void destroy(){
		s = "destroy";
		trace();
	}	

	public void start(){
		s = "start";
		trace();
	}
	
	public void stop(){
		s = "stop";
		trace();
	}

   public void trace(){
    System.out.println(s);
    repaint();
   }

	public void paint(Graphics g){
		g.drawString("*****",50,20);
		g.drawString(s, 50, 50);
		g.drawString("*****",50,80);
	}
}

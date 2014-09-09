
import java.awt.*;

public class Layout extends java.applet.Applet{

  public void init(){
    Panel s=new Panel();
    Panel e=new Panel();
    setLayout(new BorderLayout());
    add(new Label("Enter text",Label.LEFT), BorderLayout.NORTH);
    add(new TextArea(),BorderLayout.CENTER);
    e.setLayout(new GridLayout(3,1));
    e.add(new Button("Reformat"));
    e.add(new Button("Spell check"));
    e.add(new Button("Options"));
    add(e,BorderLayout.EAST);
    s.setLayout(new FlowLayout());
    s.add(new Button("Save"));
    s.add(new Button("Cancel"));
    s.add(new Button("Help"));
    add(s,BorderLayout.SOUTH);
  }
}

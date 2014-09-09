import java.awt.*;
import java.awt.event.*;

public class AboutDialog extends Dialog implements ActionListener{
  Button okButton=new Button("ok");

  public AboutDialog(String title, Frame parent, boolean modal) {
    super(parent,title,modal);
    setSize(200,100);
    setLayout(new BorderLayout());
    add("North",new Label("Programmed by U.J",Label.CENTER));
    add("South",okButton);
    okButton.addActionListener(this);
    setVisible(true);
  }

  public void actionPerformed(ActionEvent event){
    Object object = event.getSource();
      if (object == okButton)
        dispose();
  }
}


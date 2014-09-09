import java.awt.*;
import java.awt.event.*;
import java.io.*;

class MessageDialog extends Dialog
  implements ActionListener
{
  private Label lblMessage; //for showing message
  private Button btOk;

  public MessageDialog(Frame parent, String title, boolean modal)
  {
    super(parent, title, modal);

    //Create panel to hold the button
    Panel p = new Panel();
    p.setLayout(new FlowLayout(FlowLayout.RIGHT));
    p.add(btOk = new Button("Ok"));

    //place label and panel p in the dialog
    setLayout(new BorderLayout());
    add("Center", lblMessage = new Label());
    add("South", p);

    //register listener
    btOk.addActionListener(this);
  }

  //respond to the button action
  public void actionPerformed(ActionEvent e)
  {
    setVisible(false);
  }

  public void setMessage(String message)
  {
    lblMessage.setText(message);
  }
}

package lab4client;

import java.awt.*;

public class MessageDialog extends Dialog {
  private Panel text = new Panel();  // panel med Labels

  public MessageDialog(Frame parent, String m, boolean modal) {
    super(parent, "", modal);  // Java 1.2
    setLocation((int)parent.getLocationOnScreen().getX() + 10,
      (int)parent.getLocationOnScreen().getY() + 15);
    initText(m);
  }

  private void initText(String m) {
    text.setLayout(new GridLayout(0,1));  // 1 kolumn, godtyckligt många rader
    add("Center", text);
    int i=0, j;   // index för första och sista teckent på en rad
    // plocka ut en rad i taget ur meddelandet
    while ((j=m.indexOf('\n', i)) >= 0) {
      text.add(new Label(m.substring(i,j), Label.CENTER));  // nästa rad
      i = j+1;
    }
    text.add(new Label(m.substring(i), Label.CENTER));  // sista raden
    pack();
  }

  public void setTextFont(Font f) {
    text.setFont(f);
    pack();
  }
}






import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class FileDialogDemo extends MyFrameWithExitHandling
  implements ActionListener
{
  private MenuItem miOpen, miSave,miExit, miAbout;
  private TextArea ta; //for display and edit file
  private Label lblStatus; //for showing operation status
  private FileDialog fileDialog; //file dialog box
  private MessageDialog messageDialog; //message dialog box

  public static void main(String[] args)
  {
    FileDialogDemo f = new FileDialogDemo();
    f.setSize(500,400);
    f.setVisible(true);
  }

  public FileDialogDemo()
  {
    setTitle("MS Wrold");
    //create MenuBar mb
    MenuBar mb = new MenuBar();
    setMenuBar(mb);

    //add a Menu "File" in mb
    Menu fileMenu = new Menu("File");
    mb.add(fileMenu);

    //add a Menu "Help" in mb
    Menu helpMenu = new Menu("Help");
    mb.add(helpMenu);

    //add MenuItems
    fileMenu.add(miOpen = new MenuItem("Open"));
    fileMenu.add(miSave = new MenuItem("Save"));
    fileMenu.addSeparator();
    fileMenu.add(miExit = new MenuItem("Exit"));
    helpMenu.add(miAbout = new MenuItem("About"));

    //set BorderLayout for the frame
    setLayout(new BorderLayout());
    add("Center", ta = new TextArea());
    add("South", lblStatus = new Label());
    lblStatus.setBackground(Color.gray);

    //register listeners
    miOpen.addActionListener(this);
    miSave.addActionListener(this);
    miAbout.addActionListener(this);
    miExit.addActionListener(this); //Added by U.J to make it work..

    //create fileDialog
    fileDialog = new FileDialog(this);

    //create messageDialog
    messageDialog =
      new MessageDialog(this, "TestFileDialog", true);
    messageDialog.setMessage(
      "Ripped from Liang, 'Intro to Java programming'");
    messageDialog.pack();
  }

  //handle ActionEvent for menu items
  public void actionPerformed(ActionEvent e)
  {
    String actionCommand = e.getActionCommand();

    if (e.getSource() instanceof MenuItem)
    {
      if ("Open".equals(actionCommand))
        open();
      else if ("Save".equals(actionCommand))
        save();
      else if ("About".equals(actionCommand))
        messageDialog.show();
      else if ("Exit".equals(actionCommand)){
        dispose();
        System.exit(0);
      }
    }
  }

  //open file
  private void open()
  {
    fileDialog.setMode(FileDialog.LOAD);
    fileDialog.show();

    if (fileDialog.getFile() != null)
      open(fileDialog.getDirectory()+fileDialog.getFile());
  }

  private void open(String fileName)
  {
    try
    {
      BufferedInputStream in = new BufferedInputStream(
        new FileInputStream(new File(fileName)));
      byte[] b = new byte[in.available()];
      in.read(b,0,b.length);
      ta.append(new String(b,0,b.length));
      in.close();

      //Display the status of the Open file
      //operation in lblStatus.
      lblStatus.setText(fileName+ " Opened");
    }
    catch (IOException ex)
    {
      lblStatus.setText("Error opening "+fileName);
    }
  }

  //save file
  private void save()
  {
    fileDialog.setMode(FileDialog.SAVE);
    fileDialog.show();
    if (fileDialog.getFile() != null)
      save(fileDialog.getDirectory()+fileDialog.getFile());
  }

  private void save(String fileName)
  {
    try
    {
      BufferedOutputStream out = new BufferedOutputStream(
        new FileOutputStream(new File(fileName)));
      byte[] b = (ta.getText()).getBytes();
      out.write(b, 0, b.length);
      out.close();

      //Display the status of the Open file
      //operation in statusBar.
      lblStatus.setText(fileName + " Saved ");
    }
    catch (IOException ex)
    {
      lblStatus.setText("Error saving "+fileName);
    }
  }
}

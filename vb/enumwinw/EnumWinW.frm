VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   Caption         =   "Window Manager"
   ClientHeight    =   6435
   ClientLeft      =   1545
   ClientTop       =   2535
   ClientWidth     =   8670
   Icon            =   "EnumWinW.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   6435
   ScaleWidth      =   8670
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   315
      Left            =   0
      TabIndex        =   1
      Top             =   6120
      Width           =   8670
      _ExtentX        =   15293
      _ExtentY        =   556
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   14790
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ListView lstView 
      Height          =   6135
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   8655
      _ExtentX        =   15266
      _ExtentY        =   10821
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   0   'False
      AllowReorder    =   -1  'True
      FullRowSelect   =   -1  'True
      GridLines       =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   4
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Process Id"
         Object.Width           =   1764
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Thread Id"
         Object.Width           =   1764
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "Window Class"
         Object.Width           =   3528
      EndProperty
      BeginProperty ColumnHeader(4) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   3
         Text            =   "Title"
         Object.Width           =   7056
      EndProperty
   End
   Begin VB.Menu mnuList 
      Caption         =   "&List"
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&Large icons"
         Index           =   0
         Visible         =   0   'False
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&Small icons"
         Index           =   1
         Visible         =   0   'False
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&List"
         Index           =   2
         Visible         =   0   'False
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&Detailed list"
         Checked         =   -1  'True
         Index           =   3
         Visible         =   0   'False
      End
      Begin VB.Menu mnuListSep0 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuListChildren 
         Caption         =   "Include &child windows (controls, objects etc)"
      End
      Begin VB.Menu mnuListHidden 
         Caption         =   "Include &hidden windows"
      End
      Begin VB.Menu mnuListSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuListRefresh 
         Caption         =   "&Refresh"
         Shortcut        =   {F5}
      End
   End
   Begin VB.Menu mnuWindow 
      Caption         =   "&Window"
      Enabled         =   0   'False
      Begin VB.Menu mnuWindowShow 
         Caption         =   "&Hide"
         Index           =   0
      End
      Begin VB.Menu mnuWindowShow 
         Caption         =   "&Maximize"
         Index           =   3
      End
      Begin VB.Menu mnuWindowShow 
         Caption         =   "&Show"
         Index           =   5
      End
      Begin VB.Menu mnuWindowShow 
         Caption         =   "Mi&nimize"
         Index           =   6
      End
      Begin VB.Menu mnuWindowShow 
         Caption         =   "&Restore"
         Index           =   9
      End
      Begin VB.Menu mnuWindowSep0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuWindowEnable 
         Caption         =   "&Enable"
      End
      Begin VB.Menu mnuWindowDisable 
         Caption         =   "Disa&ble"
      End
      Begin VB.Menu mnuWindowSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuWindowSetText 
         Caption         =   "Edi&t caption..."
      End
      Begin VB.Menu mnuWindowSep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuWindowClose 
         Caption         =   "&Close"
      End
      Begin VB.Menu mnuWindowDestroy 
         Caption         =   "&Destroy"
      End
      Begin VB.Menu mnuWindowQuit 
         Caption         =   "&Quit"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
    mnuListRefresh_Click
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    lstView.Width = ScaleWidth
    lstView.Height = ScaleHeight - StatusBar.Height
End Sub

Private Sub lstView_DblClick()
  mnuWindowSetText_Click
End Sub

Private Sub lstView_ItemClick(ByVal Item As MSComctlLib.ListItem)
  StatusBar.SimpleText = IIf(Left(Item.Key, 1) = "T", "Top level window", "Child window (control)") & " " & Mid(Item.Key, 2)
  mnuWindow.Enabled = True
End Sub

Private Sub lstView_KeyPress(KeyAscii As Integer)
    If KeyAscii = Asc(vbCr) Then mnuWindowSetText_Click
End Sub

Private Sub lstView_KeyUp(KeyCode As Integer, Shift As Integer)
    If (KeyCode = vbKeyF10 And ((Shift And 1) <> 0)) Or KeyCode = 93 Then lstView_MouseUp 2, 0, lstView.SelectedItem.Left, lstView.SelectedItem.Top
End Sub

Private Sub lstView_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
    If (Button And 2) = 0 Then Exit Sub
    
    If Not mnuWindow.Enabled Then Exit Sub
    
    ScaleMode = vbTwips
    TrackPopupMenu GetSubMenu(GetMenu(hwnd), 1), TPM_RIGHTBUTTON Or TPM_LEFTALIGN, (x + Left + Width - ScaleWidth + lstView.Left) \ Screen.TwipsPerPixelX, (y + Top + Height - ScaleHeight + lstView.Top) \ Screen.TwipsPerPixelY, 0, hwnd, 0
End Sub

Private Sub mnuListChildren_Click()
    mnuListChildren.Checked = Not mnuListChildren.Checked
    mnuListRefresh_Click
End Sub

Private Sub mnuListHidden_Click()
    mnuListHidden.Checked = Not mnuListHidden.Checked
    mnuListRefresh_Click
End Sub

Private Sub mnuListViewMode_Click(Index As Integer)
  Dim MenuItem As Menu
  For Each MenuItem In mnuListViewMode
    MenuItem.Checked = False
  Next
  mnuListViewMode(Index).Checked = True
  
  lstView.View = Index
End Sub

Private Sub mnuListRefresh_Click()
  'On Error Resume Next
  lstView.ListItems.Clear
  ListWindows lstView.ListItems, mnuListChildren.Checked, mnuListHidden.Checked
  StatusBar.SimpleText = "Total " & lstView.ListItems.Count & " windows."
  mnuWindow.Enabled = lstView.ListItems.Count
End Sub

Private Sub mnuWindowClose_Click()
  PostMessage Mid(lstView.SelectedItem.Key, 2), WM_CLOSE, 0, 0
End Sub

Private Sub mnuWindowDisable_Click()
  EnableWindow Mid(lstView.SelectedItem.Key, 2), 0
End Sub

Private Sub mnuWindowEnable_Click()
  EnableWindow Mid(lstView.SelectedItem.Key, 2), 1
End Sub

Private Sub mnuWindowDestroy_Click()
  PostMessage Mid(lstView.SelectedItem.Key, 2), WM_DESTROY, 0, 0
End Sub

Private Sub mnuWindowQuit_Click()
  PostThreadMessage GetWindowThreadProcessId(Mid(lstView.SelectedItem.Key, 2), 0), WM_QUIT, 0, 0
End Sub

Private Sub mnuWindowSetText_Click()
  If lstView.ListItems.Count = 0 Then Exit Sub

  Dim WndText As String
  WndText = Space(65536)
  WndText = Left(WndText, GetWindowText(Mid(lstView.SelectedItem.Key, 2), WndText, Len(WndText)))
  WndText = InputBox("Enter new caption:", "Window or component caption", WndText)
  If WndText = Empty Then Exit Sub
  SetWindowText Mid(lstView.SelectedItem.Key, 2), WndText
  UpdateWindow Mid(lstView.SelectedItem.Key, 2)
  lstView.SelectedItem.SubItems(3) = WndText
End Sub

Private Sub mnuWindowShow_Click(Index As Integer)
  ShowWindow Mid(lstView.SelectedItem.Key, 2), Index
End Sub

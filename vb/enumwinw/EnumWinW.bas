Attribute VB_Name = "mdlMain"
Option Explicit

' Copy & paste from Win32 API Viewer...
Public Declare Function GetWindowThreadProcessId Lib "user32" (ByVal hwnd As Long, ByRef lpdwProcessId As Long) As Long
Public Declare Function GetClassName Lib "user32" Alias "GetClassNameA" (ByVal hwnd As Long, ByVal lpClassName As String, ByVal nMaxCount As Long) As Long
Public Declare Function GetWindowText Lib "user32" Alias "GetWindowTextA" (ByVal hwnd As Long, ByVal lpString As String, ByVal nMaxCount As Long) As Long
Public Declare Function IsWindowVisible Lib "user32" (ByVal hwnd As Long) As Long
Public Declare Function IsWindowEnabled Lib "user32" (ByVal hwnd As Long) As Long
Public Declare Function ShowWindow Lib "user32" (ByVal hwnd As Long, ByVal nCmdShow As Long) As Long
Public Declare Function PostMessage Lib "user32" Alias "PostMessageA" (ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function PostThreadMessage Lib "user32" Alias "PostThreadMessageA" (ByVal idThread As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function DestroyWindow Lib "user32" (ByVal hwnd As Long) As Long
Public Declare Function EnableWindow Lib "user32" (ByVal hwnd As Long, ByVal fEnable As Long) As Long
Public Declare Function TrackPopupMenu Lib "user32" (ByVal hMenu As Long, ByVal wFlags As Long, ByVal x As Long, ByVal y As Long, ByVal nReserved As Long, ByVal hwnd As Long, ByVal lprc As Long) As Long
Public Declare Function GetMenu Lib "user32" (ByVal hwnd As Long) As Long
Public Declare Function GetSubMenu Lib "user32" (ByVal hMenu As Long, ByVal nPos As Long) As Long
Public Declare Function SetWindowText Lib "user32" Alias "SetWindowTextA" (ByVal hwnd As Long, ByVal lpString As String) As Long
Public Declare Function UpdateWindow Lib "user32" (ByVal hwnd As Long) As Long

Public Const WM_DESTROY         As Long = &H2
Public Const WM_CLOSE           As Long = &H10
Public Const WM_QUIT            As Long = &H12

Public Const TPM_LEFTALIGN      As Long = &H0&
Public Const TPM_RIGHTBUTTON    As Long = &H2&

' Private special edition...
Private Declare Function EnumWindowsToListItems Lib "user32" Alias "EnumWindows" (ByVal lpEnumFunc As Long, ByRef TargetObject As ListItems) As Boolean
Private Declare Function EnumChildWindowsToListItems Lib "user32" Alias "EnumChildWindows" (ByVal hWndParent As Long, ByVal lpEnumFunc As Long, ByRef TargetObject As ListItems) As Boolean

Private bEnumChildren   As Boolean
Private bEnumHidden     As Boolean

Public Sub ListWindows(TargetObject As ListItems, EnumChildrenFlag As Boolean, EnumHiddenFlag As Boolean)
  bEnumChildren = EnumChildrenFlag
  bEnumHidden = EnumHiddenFlag
  
  EnumWindowsToListItems AddressOf EnumWindowsToListItems_Found, TargetObject
End Sub

Private Function EnumChildWindowsToListItems_Found(ByVal hwnd As Long, ByRef TargetObject As ListItems) As Boolean
  EnumChildWindowsToListItems_Found = ChildWindowFound(hwnd, TargetObject)
End Function

Private Function ChildWindowFound(hwnd As Long, TargetObject As ListItems) As Boolean
  On Error Resume Next
  
  ChildWindowFound = True
  
  If Not bEnumHidden Then
    If IsWindowVisible(hwnd) = 0 Then Exit Function
  End If
  
  With TargetObject.Add(Key:="C" & hwnd).ListSubItems
    .Add
    
    Dim ClassName     As String
    ClassName = Space(250)
    .Add Text:=Left(ClassName, GetClassName(hwnd, ClassName, Len(ClassName)))
    
    Dim WindowTitle   As String
    WindowTitle = Space(250)
    .Add Text:=Left(WindowTitle, GetWindowText(hwnd, WindowTitle, Len(WindowTitle)))
  End With
  
  If bEnumChildren Then
    EnumChildWindowsToListItems hwnd, AddressOf EnumChildWindowsToListItems_Found, TargetObject
  End If
End Function

Private Function EnumWindowsToListItems_Found(ByVal hwnd As Long, ByRef TargetObject As ListItems) As Boolean
  'On Error Resume Next
  EnumWindowsToListItems_Found = True
  
  If Not bEnumHidden Then
    If IsWindowVisible(hwnd) = 0 Then Exit Function
  End If
  
  Dim pid As Long
  Dim tid As Long
  tid = GetWindowThreadProcessId(hwnd, pid)
  
  With TargetObject.Add(Key:="T" & hwnd, Text:=pid).ListSubItems
    .Add Text:=tid
    
    Dim ClassName     As String
    ClassName = Space(250)
    .Add Text:=Left(ClassName, GetClassName(hwnd, ClassName, Len(ClassName)))
    
    Dim WindowTitle   As String
    WindowTitle = Space(250)
    .Add Text:=Left(WindowTitle, GetWindowText(hwnd, WindowTitle, Len(WindowTitle)))
  End With
  
  If bEnumChildren Then
    EnumChildWindowsToListItems hwnd, AddressOf EnumChildWindowsToListItems_Found, TargetObject
  End If
End Function

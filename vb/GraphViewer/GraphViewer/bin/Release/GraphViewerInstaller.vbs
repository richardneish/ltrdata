Option Explicit
On Error Resume Next

Dim WshShell
Set WshShell = CreateObject("WScript.Shell")

Dim AppDir
AppDir = WshShell.RegRead("HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\ProgramFilesDir")
If Right(AppDir, 1) <> "\" And Right(AppDir, 1) <> ":" Then AppDir = AppDir & "\"

AppDir = InputBox("GraphViewer will be installed in the directory in the box below." & vbNewLine & vbNewLine & "If you want to install GraphViewer in another directory than the default, enter the directory path you like to install GraphViewer in here.", "GraphViewer", AppDir & "GraphViewer")

If AppDir = "" Then WScript.Quit 1

If Right(AppDir, 1) <> "\" And Right(AppDir, 1) <> ":" Then AppDir = AppDir & "\"

With CreateObject("Scripting.FileSystemObject")
	.CreateFolder AppDir

	.CopyFile "GraphViewer.exe", AppDir, True
	.CopyFile "LTRLib20.dll", AppDir, True
End With

With WshShell.CreateShortcut(WshShell.SpecialFolders("Desktop") & "\GraphViewer.lnk")
  .TargetPath = AppDir & "GraphViewer.exe"
  .WorkingDirectory = AppDir
  .Description = "GraphViewer"
  .Save
End With

If Err Then
	MsgBox "Error installing GraphViewer:" & vbNewLine & vbNewLine & Error, 48, "GraphViewer"
Else
	MsgBox "GraphViewer installed successfully. Doubleclick the GraphViewer icon on your desktop to start.", 64, "GraphViewer"
End If

	
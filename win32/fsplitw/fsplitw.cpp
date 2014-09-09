#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <commdlg.h>
#include <commctrl.h>
#if defined(_DLL) && !defined(_WIN64)
#include <minwcrtlib.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>

#include "fsplitw.rc.h"

#if defined(_DLL) && !defined(_WIN64)
#pragma comment(lib, "minwcrt.lib")
#endif
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")

HINSTANCE hInstance = NULL;
bool bCancel = false;
bool bSilent = false;
DWORD dwCounter = 0;
HWND hwndProgressDialog = NULL;
char cSourcePath[MAX_PATH] = "";
char cTargetPath[MAX_PATH + 11] = "";

bool
DoEvents()
{
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (!IsDialogMessage(hwndProgressDialog, &msg))
	TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  return !bCancel;
}

void
DoSplitFile(LPSTR lpBuffer, DWORD dwBufSize)
{
  DoEvents();

  if (bCancel)
    return;

  WINDOWPLACEMENT windowplacement = { 0 };
  windowplacement.length = sizeof windowplacement;
  GetWindowPlacement(GetDlgItem(hwndProgressDialog, IDC_PROGRESSBAR_BKG),
		     &windowplacement);
  windowplacement.rcNormalPosition.right -=
    windowplacement.rcNormalPosition.left;
  windowplacement.rcNormalPosition.bottom -=
    windowplacement.rcNormalPosition.top;
  MoveWindow(GetDlgItem(hwndProgressDialog, IDC_PROGRESSBAR),
	     windowplacement.rcNormalPosition.left,
	     windowplacement.rcNormalPosition.top, 0,
	     windowplacement.rcNormalPosition.bottom, TRUE);

  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
		 "Opening source file...");

  //  HÄR BÖRJAR DET VIKTIGA...  *************************************

  HANDLE hIn = CreateFile(cSourcePath, GENERIC_READ, FILE_SHARE_READ, NULL,
			  OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (hIn == INVALID_HANDLE_VALUE)
    {
      WErrMsg errmsg;
      _snprintf(cTargetPath, sizeof cTargetPath,
		"Error opening file '%s':\n%s", cSourcePath, errmsg);
      cTargetPath[sizeof(cTargetPath)-1] = 0;
      MessageBox(hwndProgressDialog, cTargetPath, "FileSplit",
		 MB_ICONINFORMATION);
      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, "Cancelled.");
      return;
    }

  BY_HANDLE_FILE_INFORMATION by_handle_file_information;
  if (!GetFileInformationByHandle(hIn, &by_handle_file_information))
    {
      WErrMsg errmsg;
      _snprintf(cTargetPath, sizeof cTargetPath,
		"Error reading file '%s':\n%s", cSourcePath, errmsg);
      cTargetPath[sizeof(cTargetPath)-1] = 0;
      MessageBox(hwndProgressDialog, cTargetPath, "FileSplit",
		 MB_ICONINFORMATION);
      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, "Cancelled.");
      CloseHandle(hIn);
      return;
    }

  LARGE_INTEGER FileSize;
  FileSize.HighPart = by_handle_file_information.nFileSizeHigh;
  FileSize.LowPart = by_handle_file_information.nFileSizeLow;
  LARGE_INTEGER FilePos = { 0 };

  if ((FileSize.QuadPart / dwBufSize) > (DWORD) -1)
    {
      MessageBox(hwndProgressDialog, "This program cannot split files into "
		 "more than 4,294,967,295 parts.", "FileSplit",
		 MB_ICONINFORMATION);
      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, "Cancelled.");
      CloseHandle(hIn);
      return;
    }

  char *lpExt = cTargetPath + strlen(cTargetPath) + 1;
  lpExt[-1] = '.';

  DWORD dwBytesRead;
  DWORD dwFileCounter = 0;
  char buf[512];

  for (;;)
    {
      DoEvents();
      if (bCancel)
	{
	  CloseHandle(hIn);
	  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, "Cancelled.");
	  return;
	}

      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
		     "Reading source file...");

      if (!ReadFile(hIn, lpBuffer, dwBufSize, &dwBytesRead, NULL))
	{
	  WErrMsg errmsg;
	  _snprintf(buf, sizeof buf, "Error reading file '%s':\n"
		    "%s", cSourcePath, errmsg);
	  buf[sizeof(buf)-1] = 0;
	  if (MessageBox(hwndProgressDialog, buf, "FileSplit",
			 MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDRETRY)
	    continue;

	  CloseHandle(hIn);
	  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, "Cancelled.");
	  return;
	}

      if (dwBytesRead == 0)
	break;

      _snprintf(lpExt, sizeof(cTargetPath)-(lpExt-cTargetPath),
		"%u", ++dwFileCounter);
      cTargetPath[sizeof(cTargetPath)-1] = 0;
      _snprintf(buf, sizeof buf, "Writing part %s...", lpExt);
      buf[sizeof(buf)-1] = 0;
      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, buf);

      HANDLE hOutFile;
      for (;;)
	{
	  DoEvents();
	  if (bCancel)
	    {
	      CloseHandle(hIn);
	      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
			     "Cancelled.");
	      return;
	    }

	  hOutFile = CreateFile(cTargetPath, GENERIC_WRITE,
				FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	  if (hOutFile == INVALID_HANDLE_VALUE)
	    {
	      WErrMsg errmsg;
	      _snprintf(buf, sizeof buf, "Error creating part file '%s':\n"
			"%s", cTargetPath, errmsg);
	      buf[sizeof(buf)-1] = 0;
	      if (MessageBox(hwndProgressDialog, buf, "FileSplit",
			     MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDRETRY)
		continue;

	      CloseHandle(hIn);
	      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
			     "Cancelled.");
	      return;
	    }
	  break;
	}

      for (;;)
	{
	  DoEvents();
	  if (bCancel)
	    {
	      CloseHandle(hIn);
	      CloseHandle(hOutFile);
	      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
			     "Cancelled.");
	      return;
	    }

	  DWORD dwRetryLen = dwBytesRead;
	  DWORD dwBytesWritten;
	  while ((WriteFile(hOutFile, lpBuffer, dwRetryLen, &dwBytesWritten,
			    NULL) == FALSE) & (dwRetryLen > 0))
	    {
	      DoEvents();
	      if (bCancel)
		{
		  CloseHandle(hIn);
		  CloseHandle(hOutFile);
		  return;
		}

	      _snprintf(buf, sizeof buf,
			"Not enough free space, retrying %u byte...",
			dwRetryLen -= 512);
	      buf[sizeof(buf)-1] = 0;
	      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, buf);
	    }

	  CloseHandle(hOutFile);
	  FilePos.QuadPart += dwBytesWritten;
	  MoveWindow(GetDlgItem(hwndProgressDialog, IDC_PROGRESSBAR),
		     windowplacement.rcNormalPosition.left,
		     windowplacement.rcNormalPosition.top,
		     (int)(FilePos.QuadPart *
			   windowplacement.rcNormalPosition.right /
			   FileSize.QuadPart),
		     windowplacement.rcNormalPosition.bottom, TRUE);

	  if (dwBytesWritten != dwBytesRead)
	    {
	      // Disken måste vara full om inte hela bufferten kunde skrivas.
	      // Vi måste nu ha en ny full buffert, dwBytesRead lång, att
	      // skriva. Det första i den bufferten måste vara det vi ännu
	      // inte skrivit av den gamla. Kopiera därför ner återstoden till
	      // botten. Läs sedan dwBytesWritten byte från infilen till
	      // bufferten.

	      if (dwBytesWritten == 0)
		DeleteFile(cTargetPath);
	      else
		{
		  _snprintf(lpExt, sizeof(cTargetPath)-(lpExt-cTargetPath),
			    "%u", ++dwFileCounter);
		  cTargetPath[sizeof(cTargetPath)-1] = 0;
		}

	      MoveMemory(lpBuffer, lpBuffer + dwBytesWritten,
			 dwBytesRead - dwBytesWritten);

	      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
			     "Reading source file...");

	      DWORD dwReport;
	      for (;;)
		{
		  DoEvents();
		  if (bCancel)
		    {
		      CloseHandle(hIn);
		      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
				     "Cancelled.");
		      return;
		    }

		  if (!ReadFile(hIn, lpBuffer + dwBytesRead - dwBytesWritten,
				dwBytesWritten, &dwReport, NULL))
		    {
		      WErrMsg errmsg;
		      _snprintf(buf, sizeof buf,
				"Error reading file '%s':\n%s",
				cSourcePath, errmsg);
		      buf[sizeof(buf)-1] = 0;
		      if (MessageBox(hwndProgressDialog, buf, "FileSplit",
				     MB_RETRYCANCEL | MB_ICONEXCLAMATION) ==
			  IDRETRY)
			continue;

		      CloseHandle(hIn);
		      SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
				     "Cancelled.");
		      return;
		    }
		  break;
		}

	      dwBytesRead -= dwBytesWritten - dwReport;
	  
	      _snprintf(buf, sizeof buf,
			"The target disk is full.\r\nChange disks and select "
			"OK to continue with '%s' or select Cancel to stop.",
			cTargetPath);
	      buf[sizeof(buf)-1] = 0;
	      if (MessageBox(hwndProgressDialog, buf, "FileSplit",
			     MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)
		{
		  CloseHandle(hIn);
		  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
				 "Cancelled.");
		  return;
		}
	    }
	  else
	    break;

	  _snprintf(buf, sizeof buf, "Writing part %s...", lpExt);
	  buf[sizeof(buf)-1] = 0;
	  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, buf);

	  for (;;)
	    {
	      DoEvents();
	      if (bCancel)
		{
		  CloseHandle(hIn);
		  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
				 "Cancelled.");
		  return;
		}

	      hOutFile = CreateFile(cTargetPath, GENERIC_WRITE,
				    FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0,
				    NULL);
	      if (hOutFile == INVALID_HANDLE_VALUE)
		{
		  WErrMsg errmsg;
		  _snprintf(buf, sizeof buf,
			    "Error creating file '%s':\n%s",
			    cTargetPath, errmsg);
		  buf[sizeof(buf)-1] = 0;
		  if (MessageBox(hwndProgressDialog, buf, "FileSplit",
				 MB_RETRYCANCEL | MB_ICONEXCLAMATION) ==
		      IDRETRY)
		    continue;

		  CloseHandle(hIn);
		  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS,
				 "Cancelled.");
		  return;
		}
	      break;
	    }
	}
    }

  _snprintf(buf, sizeof buf, "File splitted into %s parts.", lpExt);
  SetDlgItemText(hwndProgressDialog, IDC_STATICSTATUS, buf);

  CloseHandle(hIn);

  if (FilePos.QuadPart != FileSize.QuadPart)
    {
      _snprintf(buf, sizeof buf,
		"%.4g %s not copied. The file split operation was therefore "
		"not complete.",
		TO_h(FileSize.QuadPart - FilePos.QuadPart),
		TO_p(FileSize.QuadPart - FilePos.QuadPart));
      buf[sizeof(buf)-1] = 0;
      MessageBox(hwndProgressDialog, buf, "FileSplit Warning",
		 MB_ICONEXCLAMATION);
    }
}

INT_PTR
CALLBACK
ProgressDlgProc(HWND /*hWnd*/, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
{
  if (uMsg == WM_INITDIALOG)
    {
      bCancel = false;
      return TRUE;
    }

  if ((uMsg == WM_COMMAND) & (LOWORD(wParam) == IDCANCEL))
    {
      bCancel = true;
      return TRUE;
    }

  return FALSE;
}

INT_PTR
CALLBACK
MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
{
  switch (uMsg)
    {
    case WM_INITDIALOG:
      {
	if (__argc > 1)
	  SetDlgItemText(hWnd, IDC_EDIT_SOURCE, __argv[1]);
	if (__argc > 2)
	  SetDlgItemText(hWnd, IDC_EDIT_TARGET, __argv[2]);
	if (__argc > 3)
	  if (strtoul(__argv[3], NULL, 0) != 0)
	    {
	      _snprintf(cSourcePath, sizeof cSourcePath, "%u",
			strtoul(__argv[3], NULL, 0));
	      cSourcePath[sizeof(cSourcePath)-1] = 0;
	      SetDlgItemText(hWnd, IDC_EDIT_SIZE, cSourcePath);
	    }

	SetClassLongPtr(hWnd, GCLP_HICON,
			(LONG) LoadIcon(hInstance,
					MAKEINTRESOURCE(IDI_APPICON)));
	LOGFONT logfont = { 0 };
	logfont.lfHeight = -11;
	logfont.lfWeight = FW_EXTRABOLD;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfPitchAndFamily = FF_SWISS;
	HFONT hFontStaticHelpText = CreateFontIndirect(&logfont);
	SetProp(hWnd, "hFontStaticHelpText", hFontStaticHelpText);
	SendDlgItemMessage(hWnd, IDC_STATIC_HELPTEXT, WM_SETFONT,
			   (WPARAM) hFontStaticHelpText, MAKELPARAM(FALSE, 0));
	SetDlgItemText(hWnd, IDC_STATIC_HELPTEXT,
		       "Enter the path and name of the file you want to split "
		       "into the 'Name of file to split' box and enter the "
		       "base name you would like to give the part files into "
		       "the 'Name of split files'. Each part file will get "
		       "the name you type followed by a dot and the ordinal "
		       "number of the part. Example: FILE.ZIP.1, FILE.ZIP.2 "
		       "etc. The program changes to next part file when the "
		       "size typed in the 'Max size' box is reached or when "
		       "there is no space left on the disk. If disk space "
		       "limit is reached first you are prompted to insert a "
		       "new disk.");

	SendDlgItemMessage(hWnd, IDC_EDIT_SOURCE, EM_LIMITTEXT,
			   (WPARAM)sizeof(cSourcePath) - 1, 0);
	SendDlgItemMessage(hWnd, IDC_EDIT_TARGET, EM_LIMITTEXT,
			   (WPARAM)sizeof(cSourcePath) - 12, 0);
	SendDlgItemMessage(hWnd, IDC_EDIT_SIZE, EM_LIMITTEXT, (WPARAM)10, 0);

	return TRUE;
      }

    case WM_DESTROY:
      DeleteObject(GetProp(hWnd, "hFontStaticHelpText"));
      RemoveProp(hWnd, "hFontStaticHelpText");
      return FALSE;

    case WM_COMMAND:
      if ((GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_SOURCE)) > 0) &
	  (GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_TARGET)) > 0) &
	  (GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_SIZE)) > 0))
	EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
      else
	EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);

      switch (LOWORD(wParam))
	{
	case IDOK:
	  {
	    // GetDlgItemInt() internally only handles 16-bit ints on Win32s,
	    // so just to be safe...
	    if (GetDlgItemText(hWnd, IDC_EDIT_SIZE, cSourcePath,
			       sizeof cSourcePath) == 0)
	      return TRUE;

	    UINT uiBufSize = strtoul(cSourcePath, NULL, 0);
	    // Round buffersize down to nearest 512 bytes
	    uiBufSize >>= 9;
	    uiBufSize <<= 9;
	    _snprintf(cSourcePath, sizeof cSourcePath, "%u", uiBufSize);
	    cSourcePath[sizeof(cSourcePath)-1] = 0;
	    SetDlgItemText(hWnd, IDC_EDIT_SIZE, cSourcePath);
	    LPSTR lpBuffer = (LPSTR)LocalAlloc(LMEM_FIXED, uiBufSize);
	    if (lpBuffer == NULL)
	      {
		MessageBox(hWnd, "Not enough memory is available for the "
			   "selected maximum part size. You need enough free "
			   "memory to store an entire file part in memory. "
			   "Try a smaller maximum part size.",
			   "FileSplit", MB_ICONSTOP);
		return TRUE;
	      }

	    if (GetDlgItemText(hWnd, IDC_EDIT_SOURCE, cSourcePath,
			       sizeof cSourcePath) == 0)
	      return TRUE;

	    if (GetDlgItemText(hWnd, IDC_EDIT_TARGET, cTargetPath,
			       sizeof(cTargetPath) - 11) == 0)
	      return TRUE;

	    hwndProgressDialog =
	      CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOGPROGRESS),
			   hWnd, ProgressDlgProc);

	    if (hwndProgressDialog == NULL)
	      {
		WErrMsg errmsg;
		MessageBox(hWnd, errmsg, "CreateDialog() failed.",
			   MB_ICONSTOP);
		return TRUE;
	      }

	    EnableWindow(hWnd, FALSE);

	    DoSplitFile(lpBuffer, uiBufSize);

	    LocalFree(lpBuffer);

	    SetDlgItemText(hwndProgressDialog, IDCANCEL, "&Close");
	    bCancel = false;
	    while (!bCancel)
	      {
		WaitMessage();
		DoEvents();
	      }

	    EnableWindow(hWnd, TRUE);

	    DestroyWindow(hwndProgressDialog);

	    return TRUE;
	  }

	case IDCANCEL:
	  EndDialog(hWnd, 0);
	  return TRUE;

	case IDC_BUTTON_FIND_SOURCE:
	  {
	    OPENFILENAME of = { 0 };
	    of.lStructSize = sizeof(of);
	    of.hwndOwner = hWnd;
	    of.hInstance = hInstance;
	    of.lpstrFilter =
	      "All\0*.*\0"
	      "Zip\0*.zip\0"
	      "Self Extractors\0*.exe\0"
	      "GNU-Zip\0*.??z;*.gz;*.z\0"
	      "UNIX Tape Archive\0*.tar\0"
	      "bzip2\0*.bz2\0"
	      "lzma\0*.lzma\0"
	      "7-zip\0*.7z\0"
	      "Rar\0*.rar\0";
	    of.nFilterIndex = 1;
	    GetDlgItemText(hWnd, IDC_EDIT_SOURCE, cSourcePath,
			   sizeof cSourcePath);
	    of.lpstrFile = cSourcePath;
	    of.nMaxFile = sizeof cSourcePath;
	    of.lpstrTitle = "Select file to split";
	    of.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES;

	    if (GetOpenFileName(&of))
	      SetDlgItemText(hWnd, IDC_EDIT_SOURCE, cSourcePath);
	    else if(CommDlgExtendedError() != 0)
	      {
		WErrMsg errmsg(CommDlgExtendedError());
		MessageBox(hWnd, errmsg, "Error", MB_ICONEXCLAMATION);
	      }
	    return TRUE;
	  }

	case IDC_BUTTON_FIND_TARGET:
	  {
	    OPENFILENAME of = { 0 };
	    of.lStructSize = sizeof of;
	    of.hwndOwner = hWnd;
	    of.hInstance = hInstance;
	    of.lpstrFilter = "\0";
	    of.nFilterIndex = 1;
	    GetDlgItemText(hWnd, IDC_EDIT_SOURCE, cTargetPath,
			   sizeof cTargetPath);
	    of.lpstrFile = cTargetPath;
	    of.nMaxFile = sizeof cTargetPath;
	    of.lpstrTitle = "Select base name for part files";
	    of.Flags = OFN_HIDEREADONLY | OFN_LONGNAMES |
	      OFN_NOTESTFILECREATE | OFN_PATHMUSTEXIST | OFN_SHAREAWARE;

	    if (GetSaveFileName(&of))
	      SetDlgItemText(hWnd, IDC_EDIT_TARGET, cTargetPath);
	    else if (CommDlgExtendedError())
	      {
		WErrMsg errmsg(CommDlgExtendedError());
		MessageBox(hWnd, errmsg, "Error", MB_ICONEXCLAMATION);
	      }
	    return TRUE;
	  }

	case IDC_EDIT_TARGET:
	  {
	    if (HIWORD(wParam) != 0x100)
	      return TRUE;

	    static bool bWorking = false;

	    if (bWorking)
	      return TRUE;

	    bWorking = true;

	    char buf[260] = "";
	    if (GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_TARGET)) == 0)
	      {
		GetDlgItemText(hWnd, IDC_EDIT_SOURCE, buf, sizeof buf);
		SetDlgItemText(hWnd, IDC_EDIT_TARGET, buf);
		PostMessage(GetDlgItem(hWnd, IDC_EDIT_TARGET), EM_SETSEL, 0,
			    -1);
	      }
	    bWorking = false;
	    return TRUE;
	  }

	case IDHELP:
	  MessageBox(hWnd,
		     "File Splitter for Windows, Copyright (C) Olof Lagerkvist, 1998-2007.\r\n"
		     "http://www.ltr-data.se      olof@ltr-data.se\r\n"
		     "\n"
		     "\n"
		     "This program splits a big file into smaller part files "
		     "so that each part e.g. can fit on a floppy disk.\r\n"
		     "\n"
		     "To recreate the big file use the command "
		     "'copy /b partfile.* bigfile' (without ' sign) at the "
		     "DOS Prompt or Windows Command Prompt.", "FileSplit",
		     MB_ICONINFORMATION);
	  return TRUE;
	}
      return FALSE;
    }

  return FALSE;
}

int WINAPI
WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
  if (__argc > 4 ? true : __argc > 1 ? strcmp(__argv[1], "/?") == 0 : false)
    {
      MessageBox(NULL,
		 "Syntax:\r\n"
		 "\n"
		 "fsplitw [source_file [partfile_base_name [max_part_size]]]",
		 "FileSplit", MB_ICONINFORMATION | MB_TASKMODAL);
      return 0;
    }

  hInstance = hInst;
  if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, MainDlgProc)
      == -1)
    {
      WErrMsg errmsg;
      MessageBox(NULL, errmsg, "Cannot display main window",
		 MB_ICONSTOP | MB_TASKMODAL);
      return 0;
    }

  return 1;
}

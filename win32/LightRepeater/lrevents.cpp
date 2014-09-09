/* LightRepeater Control Panel Applet
 *
 * LightRepeater v 1.0 by SM6XMK, Olof Lagerkvist
 * Send comments and suggestions to sm6xmk@qsl.net
 *
 * Latest version available at http://www.qsl.net/sm6xmk/apps/LightRepeater/
 *
 * Use this Control Panel applet to setup repeater announcements at sheduled
 * times.
 *
 *  SM6XMK LightRepeater, Repeater control software
 *  Copyright (C) 2001  SM6XMK, Olof Lagerkvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN
#include <winstrct.h>
#include <cpl.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <shlobj.h>

#include <wfilever.h>

#include "lrevents.rc.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")

#pragma comment(linker, "/export:CPlApplet=_CPlApplet@16")

#define PROP_HKEY_APP_SETTINGS_NAME "RegKeyAppSettings"
#define PROP_CTLWNDPROC_PREV_NAME   "CtlWndProc"

HINSTANCE hInstance = NULL;

LRESULT CALLBACK
CtlWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if ((uMsg == WM_KEYDOWN) & (wParam == VK_DELETE))
    if (GetDlgCtrlID(hWnd) == IDC_LSTB_ANNOUNCE_TIMES)
      if (IsWindowVisible(GetDlgItem(GetParent(hWnd), IDC_BTN_DELETE)))
	SendDlgItemMessage(GetParent(hWnd), IDC_BTN_DELETE, BM_CLICK, 0, 0);

  return CallWindowProc((WNDPROC)GetProp(hWnd, PROP_CTLWNDPROC_PREV_NAME),
			hWnd, uMsg, wParam, lParam);
}

void
InstallCtlWndProc(HWND hWnd, int iItem)
{
  HWND hwndCtl = GetDlgItem(hWnd, iItem);
  SetProp(hwndCtl, PROP_CTLWNDPROC_PREV_NAME,
	  (HANDLE) SetWindowLong(hwndCtl, GWL_WNDPROC, (LONG) CtlWndProc));
}

void
RemoveCtlWndProc(HWND hWnd, int iItem)
{
  HWND hwndCtl = GetDlgItem(hWnd, iItem);
  SetWindowLong(hwndCtl, GWL_WNDPROC,
		(LONG) GetProp(hwndCtl, PROP_CTLWNDPROC_PREV_NAME));
  RemoveProp(hwndCtl, PROP_CTLWNDPROC_PREV_NAME);
}

void
SaveCurrentItem(HWND hWnd)
{
  HKEY hKeyAppSettings = (HKEY)GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);

  char buf[6] = "";
  int idx =
    SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETCURSEL, 0, 0);
  
  if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXTLEN,
			 (WPARAM)idx, 0) != 5)
    return;
  if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXT,
			 (WPARAM)idx, (LPARAM)buf) != 5)
    return;

  char cValueData[MAX_PATH] = "";
  if (IsDlgButtonChecked(hWnd, IDC_RDB_NO_SOUND))
    strcpy(cValueData, ":");
  else if (IsDlgButtonChecked(hWnd, IDC_RDB_FILE))
    GetDlgItemText(hWnd, IDC_EDT_FILE, cValueData, sizeof cValueData);

  LONG lErrNo = RegSetValueEx(hKeyAppSettings, buf, 0, REG_SZ,
			      (LPBYTE) cValueData, strlen(cValueData) + 1);
  if (lErrNo != NO_ERROR)
    {
      WErrMsg errmsg(lErrNo);
      _snprintf(buf, sizeof buf,
		"Error writing to application registry key:\n%s", errmsg);
      buf[sizeof(buf)-1] = 0;
      MessageBox(hWnd, buf, "LightRepeater", MB_ICONEXCLAMATION);
      return;
    }
}

void
DeleteCurrentItem(HWND hWnd)
{
  HKEY hKeyAppSettings = (HKEY)GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);

  char buf[MAX_PATH];
  int idx =
    SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETCURSEL, 0, 0);
  
  if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXTLEN,
			 (WPARAM)idx, 0) != 5)
    return;
  if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXT,
			 (WPARAM)idx, (LPARAM)buf) != 5)
    return;

  LONG lErrNo = RegDeleteValue(hKeyAppSettings, buf);
  if (lErrNo != NO_ERROR)
    {
      WErrMsg errmsg(lErrNo);
      _snprintf(buf, sizeof buf,
		"Error writing to application registry key:\n%s", errmsg);
      buf[sizeof(buf)-1] = 0;
      MessageBox(hWnd, buf, "LightRepeater", MB_ICONEXCLAMATION);
      return;
    }

  SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_DELETESTRING,
		     (WPARAM)idx, 0);
}

bool
AutoEnableCreateNewButton(HWND hWnd)
{
  BOOL bTranslOk;
  UINT iNewHour =
    GetDlgItemInt(hWnd, IDC_EDT_HOUR, &bTranslOk, FALSE);
  if ((!bTranslOk) | (iNewHour > 23))
    {
      SendDlgItemMessage(hWnd, IDC_BTN_CREATE_NEW, BM_SETSTYLE,
			 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
      EnableWindow(GetDlgItem(hWnd, IDC_BTN_CREATE_NEW), FALSE);
      return false;
    }

  UINT iNewMinute =
    GetDlgItemInt(hWnd, IDC_EDT_MINUTE, &bTranslOk, FALSE);
  if ((!bTranslOk) | (iNewMinute > 59))
    {
      SendDlgItemMessage(hWnd, IDC_BTN_CREATE_NEW, BM_SETSTYLE,
			 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
      EnableWindow(GetDlgItem(hWnd, IDC_BTN_CREATE_NEW), FALSE);
      return false;
    }

  EnableWindow(GetDlgItem(hWnd, IDC_BTN_CREATE_NEW), TRUE);

  switch (GetDlgCtrlID(GetFocus()))
    {
    case IDC_EDT_HOUR: case IDC_EDT_MINUTE:
      SendDlgItemMessage(hWnd, IDC_BTN_CREATE_NEW, BM_SETSTYLE,
			 (WPARAM) BS_DEFPUSHBUTTON, MAKELPARAM(TRUE, 0));
      return true;
    default:
      SendDlgItemMessage(hWnd, IDC_BTN_CREATE_NEW, BM_SETSTYLE,
			 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
      return true;
    }
}

void
AutoEnableItemOperationControls(HWND hWnd)
{
  HKEY hKeyAppSettings = (HKEY)GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);

  int iShow = SW_HIDE;
  int idx =
    SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETCURSEL, 0, 0);
  char buf[6];

  if (idx == LB_ERR)
    iShow = SW_HIDE;
  else if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXTLEN,
			 (WPARAM)idx, 0) != 5)
    iShow = SW_HIDE;
  else if (SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_GETTEXT,
			      (WPARAM)idx, (LPARAM)buf) != 5)
    iShow = SW_HIDE;
  else
    {
      DWORD dwType;
      char cValueData[MAX_PATH] = "";
      DWORD dwSize = sizeof cValueData;
      LONG lErrNo = RegQueryValueEx(hKeyAppSettings, buf, NULL, &dwType,
				    (LPBYTE)cValueData, &dwSize);
      if (lErrNo != NO_ERROR)
	{
	  SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_DELETESTRING,
			     (WPARAM)idx, 0);
	  iShow = SW_HIDE;
	}
      else if (dwType != REG_SZ)
	iShow = SW_HIDE;
      else if (dwSize <= 1)
	{
	  SetDlgItemText(hWnd, IDC_EDT_FILE, "");
	  SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_SETSTYLE,
			     (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));

	  CheckDlgButton(hWnd, IDC_RDB_DEFAULT_SOUND, BST_CHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_NO_SOUND, BST_UNCHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_FILE, BST_UNCHECKED);

	  iShow = SW_SHOWNORMAL;
	}
      else if ((cValueData[0] == ':') & (cValueData[1] == 0))
	{
	  SetDlgItemText(hWnd, IDC_EDT_FILE, "");
	  SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_SETSTYLE,
			     (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));

	  CheckDlgButton(hWnd, IDC_RDB_DEFAULT_SOUND, BST_UNCHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_NO_SOUND, BST_CHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_FILE, BST_UNCHECKED);

	  iShow = SW_SHOWNORMAL;
	}
      else
	{
	  SetDlgItemText(hWnd, IDC_EDT_FILE, cValueData);
	  SendDlgItemMessage(hWnd, IDC_EDT_FILE, EM_SETSEL, 0, (LPARAM) -1);
	  SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_SETSTYLE,
			     (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));

	  CheckDlgButton(hWnd, IDC_RDB_DEFAULT_SOUND, BST_UNCHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_NO_SOUND, BST_UNCHECKED);
	  CheckDlgButton(hWnd, IDC_RDB_FILE, BST_CHECKED);

	  iShow = SW_SHOWNORMAL;
	}
    }

  ShowWindow(GetDlgItem(hWnd, IDC_BTN_DELETE), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_GRPB_SOUND), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_RDB_DEFAULT_SOUND), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_RDB_NO_SOUND), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_RDB_FILE), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_EDT_FILE), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_BTN_BROWSE_FILE), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_BTN_TEST), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT), iShow);
  ShowWindow(GetDlgItem(hWnd, IDC_BTN_UNDO_ANNOUNCEMENT), iShow);
}

void
RefreshTimesList(HWND hWnd)
{
  HKEY hKeyAppSettings = (HKEY)GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);

  SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_RESETCONTENT, 0, 0);

  CheckDlgButton(hWnd, IDC_RDB_DEFAULT_SOUND, BST_UNCHECKED);
  CheckDlgButton(hWnd, IDC_RDB_NO_SOUND, BST_UNCHECKED);
  CheckDlgButton(hWnd, IDC_RDB_FILE, BST_UNCHECKED);

  char cBuf[6];
  for (DWORD dwIndex = 0; ; dwIndex++)
    {
      DWORD dwBufSize = sizeof cBuf;
      LONG lRet = RegEnumValue(hKeyAppSettings, dwIndex, cBuf, &dwBufSize,
			       NULL, NULL, NULL, NULL);

      if (lRet == ERROR_NO_MORE_ITEMS)
	break;

      if (lRet == ERROR_INSUFFICIENT_BUFFER)
	continue;

      if (dwBufSize == 5)
	if (cBuf[2] == ':')
          SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_ADDSTRING,
			     (WPARAM)0, (LPARAM)cBuf);
    }
}

BOOL CALLBACK
DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
    {
    case WM_INITDIALOG:
      {
	InstallCtlWndProc(hWnd, IDC_LSTB_ANNOUNCE_TIMES);

	SendDlgItemMessage(hWnd, IDC_EDT_HOUR, EM_LIMITTEXT, (WPARAM)2, 0);
	SendDlgItemMessage(hWnd, IDC_EDT_MINUTE, EM_LIMITTEXT, (WPARAM)2, 0);
	SendDlgItemMessage(hWnd, IDC_EDT_FILE, EM_LIMITTEXT,
			   (WPARAM)MAX_PATH, 0);
	SendDlgItemMessage(hWnd, IDC_EDT_PATH, EM_LIMITTEXT,
			   (WPARAM)MAX_PATH, 0);

	HKEY hKeyAppSettings = NULL;
	LONG lErrNo = RegCreateKey(HKEY_LOCAL_MACHINE,
				   "Software\\SM6XMK\\LightRepeater",
				   &hKeyAppSettings);

	char buf[MAX_PATH];
	if (lErrNo != NO_ERROR)
	  {
	    WErrMsg errmsg(lErrNo);
	    _snprintf(buf, sizeof buf,
		      "Error creating application registry key:\n%s", errmsg);
	    buf[sizeof(buf)-1] = 0;
	    MessageBox(hWnd, buf, "LightRepeater", MB_ICONEXCLAMATION);
	    EndDialog(hWnd, -1);
	    return TRUE;
	  }

	SetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME, hKeyAppSettings);

	RefreshTimesList(hWnd);

	DWORD dwType;
	DWORD dwSize = sizeof buf;
	if (RegQueryValueEx(hKeyAppSettings, "VoicePath", NULL, &dwType,
			    (LPBYTE)buf, &dwSize) == NO_ERROR)
	  if (dwType == REG_SZ)
	    {
	      SetDlgItemText(hWnd, IDC_EDT_PATH, buf);
	      SendDlgItemMessage(hWnd, IDC_EDT_PATH, EM_SETSEL, 0,
				 (LPARAM) -1);
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_VOICE, BM_SETSTYLE,
				 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
	    }

	return FALSE;
      }

    case WM_DESTROY:
      {
	RemoveCtlWndProc(hWnd, IDC_LSTB_ANNOUNCE_TIMES);

	HKEY hKeyAppSettings =
	  (HKEY) GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);
	if (hKeyAppSettings != NULL)
	  {
	    RemoveProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);
	    RegCloseKey(hKeyAppSettings);
	  }

	PlaySound(NULL, NULL, SND_PURGE);
	return TRUE;
      }

    case WM_CLOSE:
      EndDialog(hWnd, IDOK);
      return TRUE;

    case WM_COMMAND:
      switch (LOWORD(wParam))
	{
	case IDOK:
	  switch (GetDlgCtrlID(GetFocus()))
	    {
	    case IDC_EDT_HOUR: case IDC_EDT_MINUTE:
	      if (IsWindowEnabled(GetDlgItem(hWnd, IDC_BTN_CREATE_NEW)))
		  SendDlgItemMessage(hWnd, IDC_BTN_CREATE_NEW, BM_CLICK, 0, 0);
	      return TRUE;
	    case IDC_EDT_FILE:
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_CLICK, 0,
				 0);
	      return TRUE;
	    case IDC_EDT_PATH:
	      if (IsWindowEnabled(GetDlgItem(hWnd, IDC_BTN_SAVE_VOICE)))
		  SendDlgItemMessage(hWnd, IDC_BTN_SAVE_VOICE, BM_CLICK, 0, 0);
	      return TRUE;
	    }

	  EndDialog(hWnd, IDOK);
	  return FALSE;

	case IDC_BTN_BROWSE_FILE:
	  {
	    char cFileBuf[MAX_PATH+1] = "";
	    GetDlgItemText(hWnd, IDC_EDT_FILE, cFileBuf, sizeof cFileBuf);

	    OPENFILENAME ofn = { 0 };
	    ofn.hwndOwner = hWnd;
	    ofn.lStructSize = sizeof ofn;
	    ofn.lpstrFilter = "Wave files\x0*.wav\x0";
	    ofn.lpstrFile = cFileBuf;
	    ofn.nMaxFile = sizeof cFileBuf;
	    ofn.lpstrTitle = "Select sound file";
	    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	    ofn.lpstrDefExt = "wav";
	    if (GetOpenFileName(&ofn))
	      {
		SetDlgItemText(hWnd, IDC_EDT_FILE, cFileBuf);
		SendDlgItemMessage(hWnd, IDC_EDT_FILE, EM_SETSEL, 0,
				   (LPARAM) -1);
	      }
	    return TRUE;
	  }

	case IDC_BTN_BROWSE_VOICE_PATH:
	  {
	    CoInitialize(NULL);

	    char cDirName[MAX_PATH] = "";

	    BROWSEINFO bif = { 0 };
	    bif.hwndOwner = hWnd;
	    bif.pszDisplayName = cDirName;
	    bif.lpszTitle =
	      "Select folder where voice synthesis files are stored";
	    bif.ulFlags = BIF_RETURNONLYFSDIRS;

	    LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bif);
	    if (pItemIDList == NULL)
	      {
		CoUninitialize();
		return TRUE;
	      }

	    if (!SHGetPathFromIDList(pItemIDList, cDirName))
	      MessageBox(hWnd, "Selected folder is not a file folder.", NULL,
			 MB_ICONEXCLAMATION);
	    else
	      {
		SetDlgItemText(hWnd, IDC_EDT_PATH, cDirName);
		SendDlgItemMessage(hWnd, IDC_EDT_PATH, EM_SETSEL, 0,
				   (LPARAM) -1);
	      }

	    LPMALLOC Malloc;
	    SHGetMalloc(&Malloc);
	    Malloc->Free(pItemIDList);
	    CoUninitialize();
	    return TRUE;
	  }

	case IDC_BTN_ABOUT:
	  {
	    char cModuleFileName[MAX_PATH] = "";
	    GetModuleFileName(hInstance, cModuleFileName,
			      sizeof cModuleFileName);
	    WFileVerInfo fvi(cModuleFileName);
	    char *pcDescr = (char*)fvi.QueryValue();
	    if (pcDescr == NULL)
	      pcDescr = "About LightRepeater";

	    MessageBox(hWnd,
		       "Repeater controlling software.\r\n"
		       "Copyright (C) 2001-2005  SM6XMK, Olof Lagerkvist\r\n"
		       "Visit the LightRepeater project's homepage: "
		       "http://www.qsl.net/sm6xmk/apps/LightRepeater/\r\n"
		       "\r\n"
		       "This program is free software; you can redistribute "
		       "it and/or modify it. See COPYING.TXT for details.\r\n"
		       "\r\n"
		       "This program is distributed in the hope that it will "
		       "be useful, but WITHOUT ANY WARRANTY; without even the "
		       "implied warranty of MERCHANTABILITY or FITNESS FOR A "
		       "PARTICULAR PURPOSE.",
		       pcDescr,
		       MB_ICONINFORMATION);
	    return TRUE;
	  }

	case IDC_RDB_DEFAULT_SOUND:
	case IDC_RDB_NO_SOUND:
	  switch (HIWORD(wParam))
	    {
	    case BN_CLICKED:
	      SetDlgItemText(hWnd, IDC_EDT_FILE, "");
	      return TRUE;
	    }
	  return FALSE;

	case IDC_BTN_SAVE_ANNOUNCEMENT:
	  SaveCurrentItem(hWnd);
	  return TRUE;

	case IDC_BTN_UNDO_ANNOUNCEMENT:
	  AutoEnableItemOperationControls(hWnd);
	  return TRUE;

	case IDC_BTN_TEST:
	  {
	    char buf[MAX_PATH] = "";

	    GetDlgItemText(hWnd, IDC_EDT_FILE, buf, sizeof buf);

	    PlaySound(buf, NULL, SND_ASYNC | SND_NODEFAULT);

	    return TRUE;
	  }

	case IDC_BTN_SAVE_VOICE:
	  {
	    char buf[MAX_PATH] = "";

	    GetDlgItemText(hWnd, IDC_EDT_PATH, buf, sizeof buf);

	    LONG lErrNo =
	      RegSetValueEx((HKEY) GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME),
			    "VoicePath", 0, REG_SZ, (LPBYTE) buf,
			    strlen(buf) + 1);
	    if (lErrNo != NO_ERROR)
	      {
		WErrMsg errmsg(lErrNo);
		_snprintf(buf, sizeof buf,
			  "Error writing to application registry key:\n%s",
			  errmsg);
		buf[sizeof(buf)-1] = 0;
		MessageBox(hWnd, buf, "LightRepeater", MB_ICONEXCLAMATION);
	      }
	    return TRUE;
	  }

	case IDC_BTN_UNDO_VOICE:
	  {
	    char buf[MAX_PATH] = "";
	    DWORD dwType;
	    DWORD dwSize = sizeof buf;
	    if (RegQueryValueEx((HKEY) GetProp(hWnd,
					       PROP_HKEY_APP_SETTINGS_NAME),
				"VoicePath", NULL, &dwType, (LPBYTE) buf,
				&dwSize) != NO_ERROR)
	      buf[0] = 0;

	    if (dwType != REG_SZ)
	      buf[0] = 0;

	    SetDlgItemText(hWnd, IDC_EDT_PATH, buf);
	    SendDlgItemMessage(hWnd, IDC_EDT_PATH, EM_SETSEL, 0, (LPARAM) -1);
	    SendDlgItemMessage(hWnd, IDC_BTN_SAVE_VOICE, BM_SETSTYLE,
			       (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));

	    return TRUE;
	  }

	case IDC_BTN_CREATE_NEW:
	  {
	    AutoEnableCreateNewButton(hWnd);
	    if (!IsWindowEnabled((HWND) lParam))
	      return TRUE;

	    UINT iNewHour = GetDlgItemInt(hWnd, IDC_EDT_HOUR, NULL, FALSE);
	    if (iNewHour > 23)
	      return TRUE;

	    UINT iNewMinute = GetDlgItemInt(hWnd, IDC_EDT_MINUTE, NULL, FALSE);
	    if (iNewMinute > 59)
	      return TRUE;

	    char cHourBuf[6];
	    sprintf(cHourBuf, "%02i:%02i", iNewHour, iNewMinute);

	    HKEY hKeyAppSettings =
	      (HKEY) GetProp(hWnd, PROP_HKEY_APP_SETTINGS_NAME);
	    if (RegQueryValueEx(hKeyAppSettings, cHourBuf, NULL, NULL, NULL,
				NULL) != ERROR_SUCCESS)
	      RegSetValueEx(hKeyAppSettings, cHourBuf, 0, REG_SZ, (LPBYTE) "",
			    1);

	    RefreshTimesList(hWnd);

	    SendDlgItemMessage(hWnd, IDC_LSTB_ANNOUNCE_TIMES, LB_SELECTSTRING,
			       (WPARAM) -1, (LPARAM) cHourBuf);

	    AutoEnableItemOperationControls(hWnd);

	    return TRUE;
	  }

	case IDC_LSTB_ANNOUNCE_TIMES:
	  {
	    switch (HIWORD(wParam))
	      {
	      case LBN_SELCHANGE:
		AutoEnableItemOperationControls(hWnd);
		return TRUE;
	      }
	    return FALSE;
	  }

	case IDC_BTN_DELETE:
	  DeleteCurrentItem(hWnd);
	  AutoEnableItemOperationControls(hWnd);
	  return TRUE;

	case IDC_BTN_CLOSE:
	  EndDialog(hWnd, IDOK);
	  return TRUE;

	case IDC_RDB_FILE:
	  switch (HIWORD(wParam))
	    {
	    case BN_CLICKED:
	      SendDlgItemMessage(hWnd, IDC_EDT_FILE, EM_SETSEL, 0, (LPARAM)-1);
	      SetFocus(GetDlgItem(hWnd, IDC_EDT_FILE));
	      return TRUE;
	    }
	  return FALSE;

	case IDC_EDT_FILE:
	  switch (HIWORD(wParam))
	    {
	    case EN_CHANGE:
	      if (GetWindowTextLength(GetDlgItem(hWnd, IDC_EDT_FILE)) > 0)
		{
		  CheckDlgButton(hWnd, IDC_RDB_DEFAULT_SOUND, BST_UNCHECKED);
		  CheckDlgButton(hWnd, IDC_RDB_NO_SOUND, BST_UNCHECKED);
		  CheckDlgButton(hWnd, IDC_RDB_FILE, BST_CHECKED);
		}
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_SETSTYLE,
				 (WPARAM) BS_DEFPUSHBUTTON,
				 MAKELPARAM(TRUE, 0));
	      return TRUE;

	    case EN_KILLFOCUS:
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_ANNOUNCEMENT, BM_SETSTYLE,
				 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
	      return TRUE;
	    }
	  return FALSE;

	case IDC_EDT_PATH:
	  switch (HIWORD(wParam))
	    {
	    case EN_CHANGE:
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_VOICE, BM_SETSTYLE,
				 (WPARAM) BS_DEFPUSHBUTTON,
				 MAKELPARAM(TRUE, 0));
	      return TRUE;

	    case EN_KILLFOCUS:
	      SendDlgItemMessage(hWnd, IDC_BTN_SAVE_VOICE, BM_SETSTYLE,
				 (WPARAM) BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
	      return TRUE;
	    }
	  return FALSE;

	case IDC_EDT_HOUR:
	  switch (HIWORD(wParam))
	    {
	    case EN_CHANGE:
	      if (GetWindowTextLength((HWND) lParam) >= 2)
		{
		  SendDlgItemMessage(hWnd, IDC_EDT_MINUTE, EM_SETSEL, 0,
				     (LPARAM)-1);
		  SetFocus(GetDlgItem(hWnd, IDC_EDT_MINUTE));
		}

	      AutoEnableCreateNewButton(hWnd);

	      return TRUE;

	    case EN_KILLFOCUS:
	      AutoEnableCreateNewButton(hWnd);

	      return TRUE;
	    }
	  return FALSE;

	case IDC_EDT_MINUTE:
	  switch (HIWORD(wParam))
	    {
	    case EN_CHANGE:
	      AutoEnableCreateNewButton(hWnd);

	      return TRUE;

	    case EN_KILLFOCUS:
	      AutoEnableCreateNewButton(hWnd);

	      return TRUE;
	    }
	}
    }

  return FALSE;
}

EXTERN_C LONG APIENTRY
CPlApplet(HWND hwndCPl,	// handle to Control Panel window
	  UINT uMsg,	        // message
	  LONG /*lParam1*/,	// first message parameter
	  LONG lParam2 	// second message parameter
	  )
{
  switch (uMsg)
    {
    case CPL_DBLCLK:
      {
	if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_CPLAPPLET), hwndCPl,
		      DialogProc) == -1)
	  MessageBox(hwndCPl, "Error loading dialog box.", "LightRepeater",
		     MB_ICONEXCLAMATION);
	return 0;
      }
    case CPL_EXIT:
      return 0;
    case CPL_GETCOUNT:
      return 1;
    case CPL_INIT:
      return 1;
    case CPL_INQUIRE:
      {
        LPCPLINFO lpcpli = (LPCPLINFO)lParam2;
        lpcpli->idIcon = IDI_FIRSTICON;
        lpcpli->idName = IDS_CPLAPPLET_TITLE;
        lpcpli->idInfo = IDS_CPLAPPLET_DESCRIPTION;
        lpcpli->lData = 0;
        return 0;
      }
    case CPL_STOP:
      return 0;
    default:
      return 1;
    }
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL,  DWORD, LPVOID)
{
  hInstance = hinstDLL;
  return TRUE;
}

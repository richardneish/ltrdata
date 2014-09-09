#ifndef _INC_WFILEVER_
#define _INC_WFILEVER_

#ifndef _sntprintf
#ifdef _T
#undef _T
#endif
#include <tchar.h>
#endif

#include <winstrct.h>

class WFileVerInfo
{
 private:
  LPVOID lpvDataBlock;

 public:
  explicit WFileVerInfo(LPTSTR lpstrFileName):lpvDataBlock(NULL)
    {
      DWORD dwVerFileHandle;
      DWORD dwVerInfoSize = GetFileVersionInfoSize(lpstrFileName,
						   &dwVerFileHandle);
      if (!dwVerInfoSize)
        return;
    
      lpvDataBlock = halloc(dwVerInfoSize);
      if (!lpvDataBlock)
        return;

      if (GetFileVersionInfo(lpstrFileName, dwVerFileHandle, dwVerInfoSize,
			     lpvDataBlock))
        return;

      hfree(lpvDataBlock);
      lpvDataBlock = NULL;
      return;
    }

    WFileVerInfo():lpvDataBlock(NULL)
      {
	TCHAR lpstrFileName[MAX_PATH];
	if (!GetModuleFileName(NULL, lpstrFileName,
			       sizeof(lpstrFileName) / sizeof(*lpstrFileName)))
	  return;

	DWORD dwVerFileHandle;
	DWORD dwVerInfoSize = GetFileVersionInfoSize(lpstrFileName,
						     &dwVerFileHandle);
	if (!dwVerInfoSize)
	  return;
    
	lpvDataBlock = halloc(dwVerInfoSize);
	if (!lpvDataBlock)
	  return;
    
	if (GetFileVersionInfo(lpstrFileName, dwVerFileHandle, dwVerInfoSize,
			       lpvDataBlock))
	  return;

	hfree(lpvDataBlock);
	lpvDataBlock = NULL;
	return;
      }

      ~WFileVerInfo()
	{
	  if (lpvDataBlock)
	    {
	      hfree(lpvDataBlock);
	      lpvDataBlock = NULL;
	    }
	}

      operator  bool() const
      {
	if (!this)
	  return false;
	else
	  return lpvDataBlock != NULL;
      }

      const LPTSTR QueryValue(LPTSTR lpSubBlock = NULL) const
      {
	if (this ? !lpvDataBlock : true)
	  return NULL;

	if (!lpSubBlock)
	  lpSubBlock = TEXT("\\StringFileInfo\\040904E4\\FileDescription");

	LPVOID lpVerBuf;
	UINT uiVerBufSiz;
	if (!VerQueryValue(lpvDataBlock, lpSubBlock, &lpVerBuf, &uiVerBufSiz))
	  return NULL;
	else
	  return (LPTSTR) lpVerBuf;
      }

      bool PrintFileVerRecord(LPTSTR strRecordName = NULL,
			      LPCTSTR strRecordDescription = NULL,
			      DWORD dwTranslationCode = (DWORD) -1) const
      {
	if (this ? !lpvDataBlock : true)
	  return false;

	if (!strRecordDescription)
	  strRecordDescription = strRecordName;

	DWORD dwDefaultTranslationCode = 0x04E40409;
	LPWORD lpwTranslationCode = (LPWORD) &dwTranslationCode;
	if (dwTranslationCode == (DWORD) -1)
	  {
	    lpwTranslationCode =
	      (LPWORD) QueryValue(TEXT("\\VarFileInfo\\Translation"));
	    if (!lpwTranslationCode)
	      lpwTranslationCode = (LPWORD) & dwDefaultTranslationCode;
	  }

	TCHAR strSubBlock[128];
	_sntprintf(strSubBlock, sizeof(strSubBlock) / sizeof(strSubBlock[0]),
		   TEXT("\\StringFileInfo\\%04X%04X\\%s"),
		   lpwTranslationCode[0], lpwTranslationCode[1],
		   strRecordName ? strRecordName : TEXT("FileDescription"));
	strSubBlock[sizeof(strSubBlock) / sizeof(strSubBlock[0]) - 1] = 0;

	LPSTR lpVerBuf = (LPSTR) QueryValue(strSubBlock);
	if (lpVerBuf)
	  {
	    CharToOem(lpVerBuf, lpVerBuf);

	    printf("%s: %s\n",
		   (strRecordDescription ? strRecordDescription :
		    "File description"), lpVerBuf);
	    return true;
	  }
	else
	  return false;
      }

      bool PrintCommonFileVerRecords() const
      {
	if (this ? !lpvDataBlock : true)
	  return false;

	LPDWORD lpdwTranslationCode =
	  (LPDWORD) QueryValue(TEXT("\\VarFileInfo\\Translation"));

	DWORD dwTranslationCode;
	if (lpdwTranslationCode)
	  {
	    dwTranslationCode = *lpdwTranslationCode;
	    TCHAR tcLanguageName[128];
	    if (VerLanguageName(dwTranslationCode & 0x0000FFFF, tcLanguageName,
				sizeof tcLanguageName))
	      {
#ifndef UNICODE
		CharToOem(tcLanguageName, tcLanguageName);
#endif
		printf("Language: (%p) %s\n", (void *)dwTranslationCode,
		       tcLanguageName);
	      }
	  }
	else
	  dwTranslationCode = 0x04E40409;

	PrintFileVerRecord(TEXT("CompanyName"), TEXT("Company name"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("FileDescription"), TEXT("File description"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("FileVersion"), TEXT("File version"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("InternalName"), TEXT("Internal name"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("LegalCopyright"), TEXT("Legal copyright"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("OriginalFilename"), TEXT("Original filename"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("ProductName"), TEXT("Product name"),
			   dwTranslationCode);
	PrintFileVerRecord(TEXT("ProductVersion"), TEXT("Product version"),
			   dwTranslationCode);

	return true;
      }

      DWORD GetLanguageName(LPTSTR szLang, DWORD nSize)
      {
	LPWORD lpwTranslationCode =
	  (LPWORD) QueryValue(TEXT("\\VarFileInfo\\Translation"));

	if (!lpwTranslationCode)
	  return 0;

	return VerLanguageName(*lpwTranslationCode, szLang, nSize);
      }
};
#endif // _INC_WFILEVER_

#ifndef _INC_WNTSECUR_
#define _INC_WNTSECUR_

#include <windows.h>

EXTERN_C BOOL EnablePrivilegeA(LPCSTR lpPrivName);
EXTERN_C BOOL EnablePrivilegeW(LPCWSTR lpPrivName);
#ifdef UNICODE
#define EnablePrivilege EnablePrivilegeW
#else
#define EnablePrivilege EnablePrivilegeA
#endif

#ifdef __cplusplus

/// SECURITY_ATTRIBUTES with initialization constructor.
struct WSecurityAttributes : public SECURITY_ATTRIBUTES
{
   WSecurityAttributes(SECURITY_ATTRIBUTES &sa)
   {
      *(SECURITY_ATTRIBUTES*)this = sa;
   }

   WSecurityAttributes(BOOL bSetInheritHandle = false,
      LPVOID lpSetSecurityDescriptor = NULL)
   {
      nLength = sizeof *this;
      lpSecurityDescriptor = lpSetSecurityDescriptor;
      bInheritHandle = bSetInheritHandle;
   }
};

// SECURITY_DESCRIPTOR with encapsulated API functions
struct WSecurityDescriptor : private SECURITY_DESCRIPTOR
{
   // Structure manipulation and validation
   BOOL GetControl(PSECURITY_DESCRIPTOR_CONTROL pControl, LPDWORD lpdwRevision)
   {
      return GetSecurityDescriptorControl(this, pControl, lpdwRevision);
   }

   DWORD GetLength()
   {
      return GetSecurityDescriptorLength(this);
   }

   BOOL IsValid()
   {
      return IsValidSecurityDescriptor(this);
   }

   BOOL GetDacl(LPBOOL lpbDaclPresent,	PACL *pDacl, LPBOOL lpbDaclDefaulted)
   {
      return GetSecurityDescriptorDacl(this, lpbDaclPresent, pDacl,
         lpbDaclDefaulted);
   }

   BOOL GetSacl(LPBOOL lpbSaclPresent,	PACL *pSacl, LPBOOL lpbSaclDefaulted)
   {
      return GetSecurityDescriptorSacl(this, lpbSaclPresent, pSacl,
         lpbSaclDefaulted);
   }

   BOOL GetGroup(PSID *pGroup, LPBOOL lpbGroupDefaulted)
   {
      return GetSecurityDescriptorGroup(this, pGroup, lpbGroupDefaulted);
   }

   BOOL GetOwner(PSID *pOwner, LPBOOL lpbOwnerDefaulted)
   {
      return GetSecurityDescriptorOwner(this, pOwner, lpbOwnerDefaulted);
   }

   BOOL SetDacl(BOOL bDaclPresent, PACL pDacl, BOOL bDaclDefaulted)
   {
      return SetSecurityDescriptorDacl(this, bDaclPresent, pDacl,
         bDaclDefaulted);
   }

   BOOL SetSacl(BOOL bSaclPresent, PACL pSacl, BOOL bSaclDefaulted)
   {
      return SetSecurityDescriptorSacl(this, bSaclPresent, pSacl,
         bSaclDefaulted);
   }

   BOOL SetGroup(PSID pGroup, BOOL bGroupDefaulted)
   {
      return SetSecurityDescriptorGroup(this, pGroup, bGroupDefaulted);
   }

   BOOL SetOwner(PSID pOwner, BOOL bOwnerDefaulted)
   {
      return SetSecurityDescriptorOwner(this, pOwner, bOwnerDefaulted);
   }

   // Object set/get functions
   BOOL FromFile(LPCTSTR lpFileName, SECURITY_INFORMATION RequestedInformation,
      DWORD nLength, LPDWORD lpnLengthNeeded = NULL)
   {
      DWORD dwLengthNeeded;
      if( !lpnLengthNeeded )
         lpnLengthNeeded = &dwLengthNeeded;

      return GetFileSecurity(lpFileName, RequestedInformation, this, nLength,
         lpnLengthNeeded);
   }

   BOOL ToFile(LPCTSTR lpFileName, SECURITY_INFORMATION SecurityInformation)
   {
      return SetFileSecurity(lpFileName, SecurityInformation, this);
   }

   BOOL FromKernelObject(HANDLE Handle,
      SECURITY_INFORMATION RequestedInformation,
      DWORD nLength, LPDWORD lpnLengthNeeded = NULL)
   {
      DWORD dwLengthNeeded;
      if( !lpnLengthNeeded )
         lpnLengthNeeded = &dwLengthNeeded;

      return GetKernelObjectSecurity(Handle, RequestedInformation, this,
         nLength, lpnLengthNeeded);
   }

   BOOL ToKernelObject(HANDLE Handle,
      SECURITY_INFORMATION SecurityInformation)
   {
      return SetKernelObjectSecurity(Handle, SecurityInformation, this);
   }

   BOOL FromUserObject(HANDLE hObj, PSECURITY_INFORMATION pSIRequested,
      DWORD nLength, LPDWORD lpnLengthNeeded = NULL)
   {
      DWORD dwLengthNeeded;
      if( !lpnLengthNeeded )
         lpnLengthNeeded = &dwLengthNeeded;

      return GetUserObjectSecurity(hObj, pSIRequested, this,
         nLength, lpnLengthNeeded);
   }

   BOOL ToUserObject(HANDLE hObj, PSECURITY_INFORMATION pSIRequested)
   {
      return SetUserObjectSecurity(hObj, pSIRequested, this);
   }

   BOOL FromPrivateObject(PSECURITY_DESCRIPTOR ObjectDescriptor,
      SECURITY_INFORMATION RequestedInformation,
      DWORD nLength, LPDWORD lpnLengthNeeded = NULL)
   {
      DWORD dwLengthNeeded;
      if( !lpnLengthNeeded )
         lpnLengthNeeded = &dwLengthNeeded;

      return GetPrivateObjectSecurity(ObjectDescriptor, RequestedInformation,
         this, nLength, lpnLengthNeeded);
   }

   BOOL ToPrivateObject(SECURITY_INFORMATION SecurityInformation,
      PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
      PGENERIC_MAPPING GenericMapping, HANDLE Token)
   {
      return SetPrivateObjectSecurity(SecurityInformation, this,
         ObjectsSecurityDescriptor, GenericMapping, Token);
   }

   // Initializes using InitializeSecurityDescriptor()
   BOOL Initialize()
   {
      return InitializeSecurityDescriptor(this, SECURITY_DESCRIPTOR_REVISION);
   }

   WSecurityDescriptor()
   {
      Initialize();
   }
};

// SECURITY_DESCRIPTOR_CONTROL with initialization
struct WSecurityDescriptorControl
{
   SECURITY_DESCRIPTOR_CONTROL sdcData;

   operator SECURITY_DESCRIPTOR_CONTROL() const
   {
      return sdcData;
   }

   WSecurityDescriptorControl(SECURITY_DESCRIPTOR_CONTROL &sdc)
   {
      sdcData = sdc;
   }

   WSecurityDescriptorControl(WSecurityDescriptor *&pwsd)
   {
      DWORD dwRevision;
      pwsd->GetControl(&sdcData, &dwRevision);
   }
};

#endif // __cplusplus

#endif // _INC_WNTSECUR_


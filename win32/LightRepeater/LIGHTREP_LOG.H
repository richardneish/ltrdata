// Name of statistics file
#define LR_STATISTICS_FILE_NAME        "lightrep_stat.dat"

// Global name of statistics file mapping object. If this is NULL, each
// instance of LightRepeater Engine may create it's own statistics file (if
// running in separate directories). If it is a valid name, such as
// LRStatistics, all instances of LightRepeater Engine always updates the same
// statistics file.
//#define LR_STATISTICS_FILEMAP_NAME     NULL
#define LR_STATISTICS_FILEMAP_NAME     "LRStatistics"

#include <wfilemap.h>
#include <wntsecur.h>

#include <stdlib.h>

class LRStatistics
{
   public:
   SYSTEMTIME  stLastOpenedTime;          // Date and time of last open
   SYSTEMTIME  stLastClosedTime;          // Date and time of last close
   SYSTEMTIME  stLongestSessionOpenTime;
   SYSTEMTIME  stLongestSessionCloseTime;
   SYSTEMTIME  stShortestSessionOpenTime;
   SYSTEMTIME  stShortestSessionCloseTime;
   DWORD       dwLongestSessionTime;      // Longest time on air (ms)
   DWORD       dwShortestSessionTime;     // Shortest time on air (ms)
   DWORD       dwTotalTXTime;             // Total time on air (sec)
   DWORD       dwQSOCount;                // QSO Counter
   DWORD       dwGummitummeCount;
   DWORD       dwLongSpeakerCount;

   // Constructor. If object is stored in memory only, initialize all members
   // to zero.
   LRStatistics()
   {
      if( !IsFileView() )
         ZeroMemory(this, sizeof(LRStatistics));
   }

   // new operator tries to map the statistics file. If it fails, a memory
   // only storage is allocated.
   void *operator new(size_t)
   {
      // Security descriptor granting Everyone full access
      WSecurityDescriptor secdesc;
      secdesc.SetDacl(true, NULL, false);
      WSecurityAttributes secattr(false, &secdesc);

      if( !pfm )
         pfm = new WMappedFile(LR_STATISTICS_FILE_NAME, FMAP_RW, FILE_SHARE_READ|
            FILE_SHARE_WRITE, sizeof(LRStatistics), 0, 0, OPEN_ALWAYS,
            LR_STATISTICS_FILEMAP_NAME, FILE_ATTRIBUTE_NORMAL|
            FILE_FLAG_RANDOM_ACCESS, &secattr);

      // If file mapping failed, make RAM only statistics object
      if( pfm->GetView() )
      {
	 puts("Using "
	  LR_STATISTICS_FILE_NAME
	  " log file.");
         ++iRefCount;
         return pfm->GetView();
      }
      else
      {
         win_errno = pfm->dwLastError;
         win_perror("Cannot open statistics file");
	 puts("Using memory log only.");
         return ::new LRStatistics;
      }
   }

   // delete operator unmaps file if no more object refers to the file mapping.
   void operator delete(void *__this, size_t)
   {
      if( !__this )
         return;

      if( ((LRStatistics*)__this)->IsFileView() )
      {
         if( --iRefCount < 1 )
         {
            delete pfm;
            pfm = NULL;
         }

         return;
      }

      ::delete __this;
      return;
   }

   // If view of file mapping this function flushes the view to disk.
   // If memory only object, this function does nothing.
   void Flush() const
   {
      if( IsFileView() )
         pfm->Flush();
   }

   // Returns true if object is stored in the view of the mapped statistics
   // file. Returns false if object is stored in memory only (or not allocated
   // at all).
   bool IsFileView() const
   {
      if( !this )
         return false;

      return this == pfm->GetView();
   }

   private:
   static WMappedFile *pfm;
   static int iRefCount;
};


//
// These data structures and type definitions are needed for compilation and
// use of the internal Windows APIs defined in this header.
//
typedef LONG NTSTATUS;

typedef CONST char *PCSZ;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef PSTRING PCANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef CONST STRING* PCOEM_STRING;

typedef struct _RTL_HEAP_DEFINITION {
  ULONG                   Length;
  ULONG                   Unknown1;
  ULONG                   Unknown2;
  ULONG                   Unknown3;
  ULONG                   Unknown4;
  ULONG                   Unknown5;
  ULONG                   Unknown6;
  ULONG                   Unknown7;
  ULONG                   Unknown8;
  ULONG                   Unknown9;
  ULONG                   Unknown10;
  ULONG                   Unknown11;
  ULONG                   Unknown12;
} RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

typedef enum _SECTION_INHERIT {
    ViewShare=1,
    ViewUnmap=2
} SECTION_INHERIT, *PSECTION_INHERIT;

typedef struct _FILE_NAME_INFORMATION {
  ULONG  FileNameLength;
  WCHAR  FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

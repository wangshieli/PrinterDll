/****************************************************************************
*                                                                           *
* base_type.h -- Basic Type Definitions                                *
*                                                                           *
****************************************************************************/
#ifndef __BASE_TYPE_H__
#define __BASE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)

#define NOCRYPT

#ifndef _WINSOCKAPI_	//Ã»ÓÐ°üº¬"winsock.h"
	#include <winsock2.h>
#endif
#include <windows.h>

#if defined(_WIN32)
typedef __int64		INT64;
typedef ULONGLONG	UINT64;
#endif

#else // _WIN32
/*
 * BASETYPES is defined in ntdef.h if these types are already defined
 */

#ifndef BASETYPES
#define BASETYPES

#define IN 
#define OUT

typedef unsigned int ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;

#define __stdcall
#define _stdcall

#endif  /* !BASETYPES */

#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH          260


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef int LONG;

typedef long long INT64;			//ckw add
typedef unsigned long long UINT64;	//ckw add

typedef unsigned int        DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef float           *PFLOAT;
typedef BOOL            *PBOOL;
typedef BOOL             *LPBOOL;
typedef BYTE            byte;
typedef BYTE            *PBYTE;
typedef BYTE             *LPBYTE;
typedef int             *PINT;
typedef int              *LPINT;
typedef WORD            *PWORD;
typedef WORD             *LPWORD;
typedef int             *LPLONG;
typedef DWORD           *PDWORD;
typedef DWORD           *LPDWORD;
typedef void				*PVOID;
typedef void             *LPVOID;
typedef const void       *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;


typedef char				*LPTSTR;
typedef const char			*LPCTSTR;
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef const CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef const CHAR *LPCSTR, *PCSTR;


typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character

typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef const WCHAR *LPCWSTR, *PCWSTR;

#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct _GUID {          // size is 16
    DWORD Data1;
    WORD   Data2;
    WORD   Data3;
    BYTE  Data4[8];
} GUID;

#endif // !GUID_DEFINED

typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

typedef PVOID HANDLE;
typedef HANDLE *PHANDLE;
typedef HANDLE HINSTANCE;
typedef HINSTANCE HMODULE;
typedef HANDLE HKEY;
typedef HANDLE *PHKEY;
typedef HANDLE HWND;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#include <sys/time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <unistd.h>
#include <errno.h>

#define _MAX_PATH 256

#endif /* _WIN32 */

typedef unsigned int        UINT32;
typedef unsigned int        UINT4;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned short      UINT2;
typedef int                 INT32;
typedef short               INT16;


#ifdef __cplusplus
}
#endif 


#endif /* __BASE_TYPE_H__ */


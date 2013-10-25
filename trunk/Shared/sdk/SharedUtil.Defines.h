/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Defines.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// System wide defines
//
#define DEFINED_SHARED_UTIL 1

// Enable WITH_ALLOC_TRACKING to monitor server module memory usage. *Has a negative performance impact*
#ifndef WITH_ALLOC_TRACKING
    #ifndef MTA_CLIENT
        #define WITH_ALLOC_TRACKING 0       // Alloc track server - (Can also be defined in individual modules, just above #include "SharedUtil.h")
    #else
        #define WITH_ALLOC_TRACKING 0       // Alloc track client - (Can also be defined in individual modules, just above #include "SharedUtil.h")
    #endif
#endif

//
// vsnprintf with buffer full check
//
#define VSNPRINTF( buffer, count, format, argptr ) \
    { \
        int iResult = vsnprintf ( buffer, count, format, argptr ); \
        if( iResult == -1 || iResult == (count) ) \
            (buffer)[(count)-1] = 0; \
    }

//
// strncpy with null termination
//
#ifndef STRNCPY
    #define STRNCPY( dest, source, count ) \
        { \
            strncpy( dest, source, (count)-1 ); \
            (dest)[(count)-1] = 0; \
        }
#endif

//
// Copy null terminated string to a temporary buffer on the stack
//
#define COPY_CSTR_TO_TEMP_BUFFER( tempname, src, maxsize ) \
    char tempname [maxsize] = ""; \
    if ( src ) \
        STRNCPY( tempname, src, maxsize );

#ifndef _MSC_VER
    #define _isnan isnan
    #define wcsicmp wcscasecmp
#endif

#ifdef _MSC_VER
    #define atoi64 _atoi64
    #define PRId64  "I64d"
    #define PRIx64  "I64x"
#else
    #define atoi64 std::atoll
    #define PRId64  "lld"
    #define PRIx64  "llx"
#endif

//
// Macro for counting the number of elements in a static array
//
#ifndef NUMELMS     // from DShow.h
    #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif


#ifdef WIN32
    #define PATH_SEPERATOR "\\"
#else
    #define PATH_SEPERATOR "/"
#endif


// Auto clear a class when new'ed. (Won't work for inline creation.)
#define ZERO_ON_NEW \
    void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr,0,size); return ptr; } \
    void* operator new ( size_t size, void* where ) { memset(where,0,size); return where; }


// As NDEBUG is not defined across most MTA projects, assert() will always be enabled
// Use dassert for debug build only assertations
#if defined(MTA_DEBUG) || defined(DEBUG) || defined(_DEBUG) 
    #define dassert assert
#else
    #define dassert(_Expression)     ((void)0)
    #ifdef WIN32
        // This, along with RedirectedSetUnhandledExceptionFilter means we can get reports from all crashes with the correct crash address in the file name
        #undef assert
        #define assert(_Expression) (void)( (!!(_Expression)) || ( *((int*)NULL) = 0) )
    #endif
#endif

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define PI (3.14159265358979323846f)

// Clear all member variables to zero for a struct.
// Note: Struct should have simple member variables and no virtual functions.
#define ZERO_POD_STRUCT(ptr) \
        memset ( ptr, 0, sizeof(*(ptr)) )

// Crazy thing
#define LOCAL_FUNCTION_START    struct local {
#define LOCAL_FUNCTION_END      };
#define LOCAL_FUNCTION          local

// Inline callback definition for std::sort
#define sort_inline(a,b,c) \
        { \
            LOCAL_FUNCTION_START \
                static bool SortPredicate c \
            LOCAL_FUNCTION_END \
            std::sort ( a, b, LOCAL_FUNCTION::SortPredicate ); \
        }

// printf/wprintf helpers
//
// http://www.firstobject.com/wchar_t-string-on-linux-osx-windows.htm
//
// In VC++, you can use "%s" in the format string of swprintf (or wprintf, fwprintf) to insert a wide string.
// But in POSIX you have to use "%ls". This may be compiler dependent rather than operating system dependent.
//
// type     meaning in sprintf      meaning in swprintf
//          Windows     POSIX       Windows     POSIX
//ls or lS  wchar_t     wchar_t     wchar_t     wchar_t
//s         char        char        wchar_t     char
//S         wchar_t     char        char        ????
//
#define PRSinS   "%s"       // i.e. SString (  "name:" PRSinS,  "dave" );
#define PRWinS   "%ls"      // i.e. SString (  "name:" PRWinS, L"dave" );
#define PRWinW  L"%ls"      // i.e. WString ( L"name:" PRWinW, L"dave" );
#ifdef _MSC_VER
    #define PRSinW  L"%S"       // i.e. WString ( L"name:" PRSinW,  "dave" );
#else
    #define PRSinW  L"%s"       // i.e. WString ( L"name:" PRSinW,  "dave" );
#endif

// Generic way of showing a uchar index is not set
#define UCHAR_INVALID_INDEX 0xFF

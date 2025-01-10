/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Defines.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
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
// Architecture
//
#if defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_AMD64 ) || defined( __amd64__ ) 
    #define ANY_x64
    #ifdef _WIN64
        #define WIN_x64
    #elif defined(__APPLE__)
        #define APPLE_x64
    #else
        #define LINUX_x64
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ANY_arm64
    #ifdef _M_ARM64
        #define WIN_arm64
    #elif defined(__APPLE__)
        #define APPLE_arm64
    #else
        #define LINUX_arm64
    #endif
#elif defined(__arm__) || defined(_M_ARM)
    #define ANY_arm
    #ifdef _M_ARM
        #define WIN_arm
    #else
        #define LINUX_arm
    #endif
#else
    #define ANY_x86
    #ifdef WIN32
        #define WIN_x86
    #else
        #define LINUX_x86
    #endif
#endif

//
// Location of server binaries
//
#ifdef ANY_x86
    #define SERVER_BIN_PATH     ""
    #define SERVER_BIN_PATH_MOD "mods/deathmatch/"
#elif defined(ANY_arm)
    #define SERVER_BIN_PATH     "arm/"
    #define SERVER_BIN_PATH_MOD "arm/"
#elif defined(ANY_arm64)
    #define SERVER_BIN_PATH     "arm64/"
    #define SERVER_BIN_PATH_MOD "arm64/"
#else
    #define SERVER_BIN_PATH     "x64/"
    #define SERVER_BIN_PATH_MOD "x64/"
#endif

//
// Location of server crash dumps
//
#ifdef WIN32
    #define SERVER_DUMP_PATH    "dumps"
#else
    #define SERVER_DUMP_PATH    "mods/deathmatch/dumps"
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
    #define wcsicmp wcscasecmp
#endif

//
// Macro for counting the number of elements in a static array
//
#ifndef NUMELMS     // from DShow.h
    #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif

//
// Stringize the result of expansion of a macro argument
//
#define MTA_STR_EXPAND(token) #token
#define MTA_STR(token) MTA_STR_EXPAND(token)

#ifdef WIN32
    #define PATH_SEPERATOR "\\"
#else
    #define PATH_SEPERATOR "/"
#endif

// Auto clear a class when new'ed. (Won't work for inline creation.)
#define ZERO_ON_NEW \
    void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr == (void*)-1 ? 0 : ptr,0,size); return ptr; } \
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

#define BYTE_MASK(n) ((1 << (n)) - 1)

// printf/wprintf helpers
//
// http://www.firstobject.com/wchar_t-string-on-linux-osx-windows.htm
//
// In VC++, you can use "%s" in the format string of swprintf (or wprintf, fwprintf) to insert a wide string.
// But in POSIX you have to use "%ls". This may be compiler dependent rather than operating system dependent.
//
// type     meaning in sprintf      meaning in swprintf
//          Windows     POSIX       Windows     POSIX
// ls or lS  wchar_t     wchar_t     wchar_t     wchar_t
// s         char        char        wchar_t     char
// S         wchar_t     char        char        ????
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

// Tick count conversions
#define TICKS_FROM_SECONDS(m)   ((m)*1000)
#define TICKS_FROM_MINUTES(m)   (TICKS_FROM_SECONDS(m)*60)
#define TICKS_FROM_HOURS(m)     (TICKS_FROM_MINUTES(m)*60)
#define TICKS_FROM_DAYS(m)      (TICKS_FROM_HOURS(m)*24)

// Export function
#if defined _WIN32
    #define MTAEXPORT extern "C" __declspec(dllexport)
#else
    #define MTAEXPORT extern "C" __attribute__ ((visibility ("default")))
#endif

#define BUILD_YEAR ((((__DATE__ [7]-'0')*10+(__DATE__ [8]-'0'))*10+(__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))
#define BUILD_MONTH (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 0 : 5) \
                : __DATE__ [2] == 'b' ? 1 \
                : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M'? 2 : 3) \
                : __DATE__ [2] == 'y' ? 4 \
                : __DATE__ [2] == 'l' ? 6 \
                : __DATE__ [2] == 'g' ? 7 \
                : __DATE__ [2] == 'p' ? 8 \
                : __DATE__ [2] == 't' ? 9 \
                : __DATE__ [2] == 'v' ? 10 : 11)
#define BUILD_DAY ((__DATE__ [4]==' ' ? 0 : __DATE__[4]-'0')*10+(__DATE__[5]-'0'))
static constexpr float FLOAT_EPSILON = 0.0001f;

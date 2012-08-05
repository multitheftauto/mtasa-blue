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

// Enable WITH_ALLOC_TRACKING to monitor server module memory usage. *Has a negative performance impact*
// (Also only works for the server. Will give compile errors in the client projects.)
#define WITH_ALLOC_TRACKING 0

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

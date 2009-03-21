/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// class SString
//
// Adds some functionality to the std::string class
//
#include <string>

#ifdef WIN32
    #define va_copy(dest, orig) (dest) = (orig)
#endif

class SString : public std::string
{
public:
    // Constructors
    SString ( )
        : std::string ()
    { }

    explicit SString ( const char* szText )
        : std::string ( szText ? szText : "" )
    { }

    SString ( const char* szFormat, ... )
        : std::string ()
    {
        if ( szFormat )
        {
            va_list vl;

            va_start ( vl, szFormat );
            vFormat ( szFormat, vl );
            va_end ( vl );
        }
    }

    SString ( const std::string& strText )
        : std::string ( strText )
    { }


    SString& Format ( const char* szFormat, ... )
    {
        va_list vl;

        va_start ( vl, szFormat );
        SString& str = vFormat ( szFormat, vl );
        va_end ( vl );

        return str;
    }

    SString& vFormat ( const char* szFormat, va_list vl )
    {

#ifdef WIN32

        va_list vlLocal;
        size_t curCapacity = std::string::capacity ();
        char* szDest = const_cast < char* > ( std::string::data() );

        // Make sure to have a capacity greater than 0, so vsnprintf
        // returns -1 in case of not having enough capacity
        if ( curCapacity == 0 )
        {
            std::string::reserve ( 16 );
            curCapacity = std::string::capacity ();
        }

        va_copy ( vlLocal, vl );

        // Try to format the string into the std::string buffer. If we will need
        // more capacity it will return -1 and we will resize. Else we've finished.
        int iSize = _vsnprintf ( szDest, curCapacity, szFormat, vlLocal );
        if ( iSize == -1 || static_cast < size_t > ( iSize ) >= curCapacity )
        {
            // We didn't have enough capacity to fit the string. Count how much
            // characters would we need.
            va_copy ( vlLocal, vl );
            int iRequiredCapacity = _vscprintf ( szFormat, vlLocal );

            if ( iRequiredCapacity == -1 )
            {
                // If it failed for a reason not related to the capacity, then force it
                // to return -1.
                iSize = -1;
            }
            else
            {
                // Reserve at least the new required capacity
                std::string::reserve ( iRequiredCapacity + 1 );

                // Grab the new data for the resized string.
                curCapacity = std::string::capacity ();
                szDest = const_cast < char * > ( std::string::data () );

                // Finally format it
                va_copy ( vlLocal, vl );
                iSize = _vsnprintf ( szDest, curCapacity, szFormat, vlLocal );
            }
        }

        // If there weren't any errors, give the formatted string back to std::string.
        if ( iSize > -1 )
        {
            szDest [ iSize ] = '\0';
            std::string::assign ( szDest );
        }

        return *this;

#else

        va_list vlLocal;
        size_t curCapacity = std::string::capacity ();
        char* szDest = const_cast < char* > ( std::string::data () );

        // Make sure to have a capacity greater than 0, so vsnprintf
        // returns -1 in case of not having enough capacity
        if ( curCapacity == 0 )
        {
            std::string::reserve ( 15 );
            curCapacity = std::string::capacity ();
        }

        va_copy ( vlLocal, vl );

        // Try to format the string into the std::string buffer. If we will need
        // more capacity it will return -1 in glibc 2.0 and a greater capacity than
        // current in glibc 2.1, so we will resize. Else we've finished.
        int iSize = vsnprintf ( szDest, curCapacity, szFormat, vlLocal );
        if ( iSize == -1 )
        {
            // glibc 2.0 - Returns -1 when it hasn't got enough capacity.
            // Duplicate the buffer size until we get enough capacity
            do
            {
                va_copy ( vlLocal, vl );
                std::string::reserve ( curCapacity * 2 );
                curCapacity = std::string::capacity ();
                szDest = const_cast < char * > ( std::string::data () );

                iSize = vsnprintf ( szDest, curCapacity, szFormat, vlLocal );
            } while ( iSize == -1 );
        }
        else if ( static_cast < size_t > ( iSize ) >= curCapacity )
        {
            // glibc 2.1 - Returns the required capacity.
            va_copy ( vlLocal, vl );
            std::string::reserve ( iSize + 1 );
            curCapacity = std::string::capacity ();
            szDest = const_cast < char * > ( std::string::data () );

            iSize = vsnprintf ( szDest, curCapacity, szFormat, vlLocal );
        }

        // If there weren't any errors, give the formatted string back to std::string.
        if ( iSize > -1 )
        {
            szDest [ iSize ] = '\0';
            std::string::assign ( szDest );
        }

        return *this;

#endif

    }

    // Assignment  
    operator const char*() const    { return c_str (); }        // Auto assign to const char* without using c_str()
};


//
// _vsnprintf with buffer full check
//
#define _VSNPRINTF( buffer, count, format, argptr ) \
    { \
        int iResult = _vsnprintf ( buffer, count, format, argptr ); \
        if( iResult == -1 || iResult == (count) ) \
            (buffer)[(count)-1] = 0; \
    }

#ifndef WIN32
    #define _vsnprintf vsnprintf
#endif


//
// Get startup directory as saved in the registry by the launcher
// Used in the Win32 Client only
//
static SString GetMTASABaseDir()
{
    static TCHAR szInstallRoot[MAX_PATH]=TEXT("");
    if( !szInstallRoot[0] )
    {
        memset ( szInstallRoot, 0, MAX_PATH );

        HKEY hkey = NULL;
        DWORD dwBufferSize = MAX_PATH;
        DWORD dwType = 0;
        if ( RegOpenKeyEx ( HKEY_LOCAL_MACHINE, "Software\\Multi Theft Auto: San Andreas", 0, KEY_READ, &hkey ) == ERROR_SUCCESS ) 
        {
            // Read out the MTA installpath
            if ( RegQueryValueEx ( hkey, "Last Run Location", NULL, &dwType, (LPBYTE)szInstallRoot, &dwBufferSize ) != ERROR_SUCCESS ||
                strlen ( szInstallRoot ) == 0 )
            {
                MessageBox ( 0, "Multi Theft Auto has not been installed properly, please reinstall.", "Error", MB_OK );
                RegCloseKey ( hkey );
                TerminateProcess ( GetCurrentProcess (), 9 );
            }
            RegCloseKey ( hkey );
        }
    }
    return szInstallRoot;
}


//
// Turns a relative MTASA path i.e. "MTA\file.dat"
// into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
//
static SString CalcMTASAPath ( const SString& strPath )
{
    SString strNewPath = GetMTASABaseDir();
    strNewPath += '\\';
    strNewPath += strPath;
    return strNewPath;
}

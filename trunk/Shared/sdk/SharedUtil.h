/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

using namespace std;


//
// class SString
//
// Adds some functionality to the std::string class
//
class SString : public string
{
public:
    // Constructors
    SString ( ) : string () {}
    SString ( const char* szText ) : string ( szText ? szText : "" ) {}
    SString ( const string& strText ) : string ( strText ) {}

    // Assignment  
    operator const char*() const    { return c_str (); }        // Auto assign to const char* without using c_str()

    // Static functions
    static SString Printf ( const char *format, ... );
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


//
// Turns a relative MTASA path i.e. "MTA\file.dat"
// into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
//
SString CalcMTASAPath ( const SString& strPath );



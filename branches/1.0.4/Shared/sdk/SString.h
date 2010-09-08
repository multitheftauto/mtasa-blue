/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SString.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

//
// class SString
//
// Adds some functionality to the std::string class
//
#include <string>
#include <vector>
#include <stdarg.h>

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

    SString ( const char* szText )
        : std::string ( szText ? szText : "" )
    { }

    explicit SString ( const char* szFormat, ... )
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

    SString& vFormat ( const char* szFormat, va_list vl );

    // Access
    char& operator[]( int iOffset )
    {
        return std::string::operator[]( iOffset );
    }

    // Assignment  
    operator const char*() const    { return c_str (); }        // Auto assign to const char* without using c_str()

    // Functions
    void        Split               ( const SString& strDelim, std::vector < SString >& outResult, unsigned int uiMaxAmount = 0 ) const;
    bool        Split               ( const SString& strDelim, SString* pstrLeft, SString* pstrRight, bool bFromRight = false ) const;
    SString     Replace             ( const char* szOld, const char* szNew ) const;
    SString     TrimStart           ( const char* szOld ) const;
    SString     TrimEnd             ( const char* szOld ) const;
    SString     ToLower             ( void ) const;
    SString     ToUpper             ( void ) const;
    SString     ConformLineEndings  ( void ) const;
};

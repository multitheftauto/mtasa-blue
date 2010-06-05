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

    // Functions
    void        Split               ( const SString& strDelim, std::vector < SString >& outResult ) const;
    SString     Replace             ( const char* szOld, const char* szNew ) const;
    SString     TrimEnd             ( const char* szOld ) const;

};

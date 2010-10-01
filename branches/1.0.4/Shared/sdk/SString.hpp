/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SString.hpp
*  PURPOSE:     SString implementation 
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
// Format a string
//
SString& SString::vFormat ( const char* szFormat, va_list vl )
{
#ifdef WIN32

    va_list vlLocal;

    // Calc size
    va_copy ( vlLocal, vl );
    int iRequiredCapacity = _vscprintf ( szFormat, vlLocal );

    if ( iRequiredCapacity < 1 )
    {
        // Error or empty string
        clear ();
        return *this;
    }

    // Allocate buffer
    char* szDest = static_cast < char* > ( malloc ( iRequiredCapacity + 1 ) );

    // Try to format the string into the buffer.
    va_copy ( vlLocal, vl );
    int iSize = vsnprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );

    if ( iSize < 1 )
    {
        // Error
        clear ();
    }
    else
    {
        // Copy from buffer
        szDest [ iSize ] = '\0';
        std::string::assign ( szDest );
    }

    // Delete buffer
    free ( szDest );

    // Done
    return *this;

#else

    va_list vlLocal;

    // Guess size
    int iRequiredCapacity = 220;

    // Allocate buffer
    char* szDest = static_cast < char* > ( malloc ( iRequiredCapacity + 1 ) );

    // Try to format the string into the buffer. If we will need
    // more capacity it will return -1 in glibc 2.0 and a greater capacity than
    // current in glibc 2.1, so we will resize. Else we've finished.
    va_copy ( vlLocal, vl );
    int iSize = vsnprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
    if ( iSize == -1 )
    {
        // glibc 2.0 - Returns -1 when it hasn't got enough capacity.
        // Duplicate the buffer size until we get enough capacity
        do
        {
            iRequiredCapacity *= 2;
            szDest = static_cast < char* > ( realloc ( szDest, iRequiredCapacity + 1 ) );
            va_copy ( vlLocal, vl );
            iSize = vsnprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
        } while ( iSize == -1 );
    }
    else if ( iSize > iRequiredCapacity )
    {
        // glibc 2.1 - Returns the required capacity.
        iRequiredCapacity = iSize + 1;
        szDest = static_cast < char* > ( realloc ( szDest, iRequiredCapacity + 1 ) );

        va_copy ( vlLocal, vl );
        iSize = vsnprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
    }

    if ( iSize < 1 )
    {
        // Error or empty string
        clear ();
    }
    else
    {
        // Copy from buffer
        szDest [ iSize ] = '\0';
        std::string::assign ( szDest );
    }

    // Delete buffer
    free ( szDest );

    // Done
    return *this;
#endif
}


//
// Split into parts
//
void SString::Split ( const SString& strDelim, std::vector < SString >& outResult, unsigned int uiMaxAmount ) const
{
    outResult.clear ();
    unsigned long ulStartPoint = 0;

    while ( true )
    {
        unsigned long ulPos = find ( strDelim, ulStartPoint );

        if ( ulPos == npos || ( uiMaxAmount > 0 && uiMaxAmount <= outResult.size () + 1 ) )
        {
            if ( ulStartPoint <= length () )
                outResult.push_back ( substr ( ulStartPoint ) );
            return;
        }

        outResult.push_back ( substr ( ulStartPoint, ulPos - ulStartPoint ) );

        ulStartPoint = ulPos + strDelim.length ();
    }
}

//
// Split in two
//
bool SString::Split ( const SString& strDelim, SString* pstrLeft, SString* pstrRight, bool bFromRight ) const
{
    unsigned long ulPos = bFromRight ? rfind ( strDelim ) : find ( strDelim );

    if ( ulPos == npos )
    {
        if ( pstrLeft )
            *pstrLeft = bFromRight ? "" : c_str ();
        if ( pstrRight )
            *pstrRight = bFromRight ? c_str () : "";
        return false;
    }

    if ( pstrLeft )
        *pstrLeft = substr ( 0, ulPos );

    if ( pstrRight )
        *pstrRight = substr ( ulPos + strDelim.length (), length () - ( ulPos + strDelim.length () ) );
 
    return true;
}


//
// Replace all occurrences of the string szOld with szNew
//
SString SString::Replace ( const char* szOld, const char* szNew ) const
{
    int iOldLength = strlen ( szOld );
    int iNewLength = strlen ( szNew );
    SString strResult = *this;
    int idx = 0;
    while( ( idx = strResult.find ( szOld, idx ) ) >= 0 )
    {
        strResult.replace ( idx, iOldLength, szNew );
        idx += iNewLength - iOldLength + 1;
    }
    return strResult;
}

//
// Case insensitive version of Replace()
//
SString SString::ReplaceI ( const char* szOld, const char* szNew ) const
{
    int iOldLength = strlen ( szOld );
    int iNewLength = strlen ( szNew );
    SString strResult = *this;
    int idx = 0;
    while( ( idx = strResult.ToUpper ().find ( SString ( szOld, 0 ).ToUpper (), idx ) ) >= 0 )
    {
        strResult.replace ( idx, iOldLength, szNew );
        idx += iNewLength - iOldLength + 1;
    }
    return strResult;
}


//
// Remove szOlds from the start of the string.
//
SString SString::TrimStart ( const char* szOld ) const
{
    const uint uiOldLength = strlen ( szOld );
    SString strResult = *this;
    while ( strResult.substr ( 0, uiOldLength ) == szOld )
        strResult = strResult.substr ( uiOldLength );
    return strResult;
}

//
// Remove szOlds from the end of the string.
//
SString SString::TrimEnd ( const char* szOld ) const
{
    const uint uiOldLength = strlen ( szOld );
    SString strResult = *this;
    while ( strResult.length () >= uiOldLength && strResult.substr ( strResult.length () - uiOldLength ) == szOld )
        strResult = strResult.substr ( 0, strResult.length () - uiOldLength );
    return strResult;
}

//
// Change to all lower case characters.
//
SString SString::ToLower ( void ) const
{
    SString strResult = *this;
    std::transform ( strResult.begin(), strResult.end(), strResult.begin(), ::tolower );
    return strResult;
}

//
// Change to all upper case characters.
//
SString SString::ToUpper ( void ) const
{
    SString strResult = *this;
    std::transform ( strResult.begin(), strResult.end(), strResult.begin(), ::toupper );
    return strResult;
}

//
// Change '0x0a' or '0x0d' or '0x0d 0x0a' to '\n'.
//
SString SString::ConformLineEndings ( void ) const
{
    assert ( '\n' == '\x0A' );
    if ( std::count( begin(), end(), '\n' ) )
        return Replace ( "\x0D", "" );
    else
        return Replace ( "\x0D", "\n" );
}

//
// Test if string contains strOther
//
bool SString::Contains ( const SString& strOther ) const
{
    return find ( strOther ) != std::string::npos;
}

//
// Test if string contains strOther. Case insensitive.
//
bool SString::ContainsI ( const SString& strOther ) const
{
    return ToUpper ().find ( strOther.ToUpper () ) != std::string::npos;
}

//
// Case insensitive compate.
//
bool SString::CompareI ( const SString& strOther ) const
{
    return stricmp ( *this, strOther ) == 0;
}

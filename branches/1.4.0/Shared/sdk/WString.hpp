/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        WString.hpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
// Format a string
//
WString& WString::vFormat ( const wchar_t* szFormat, va_list vl )
{
#ifdef WIN32

    va_list vlLocal;

    // Calc size
    va_copy ( vlLocal, vl );
    int iRequiredCapacity = _vscwprintf ( szFormat, vlLocal );

    if ( iRequiredCapacity < 1 )
    {
        // Error or empty string
        clear ();
        return *this;
    }

    // Allocate buffer
    wchar_t* szDest = static_cast < wchar_t* > ( malloc ( ( iRequiredCapacity + 1 ) * sizeof ( wchar_t ) ) );

    // Try to format the string into the buffer.
    va_copy ( vlLocal, vl );
    int iSize = _vsnwprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );

    if ( iSize < 1 )
    {
        // Error
        clear ();
    }
    else
    {
        // Copy from buffer
        szDest [ iSize ] = '\0';
        std::wstring::assign ( szDest );
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
    wchar_t* szDest = static_cast < wchar_t* > ( malloc ( ( iRequiredCapacity + 1 ) * sizeof ( wchar_t ) ) );

    // Try to format the string into the buffer. If we will need
    // more capacity it will return -1 in glibc 2.0 and a greater capacity than
    // current in glibc 2.1, so we will resize. Else we've finished.
    va_copy ( vlLocal, vl );
    int iSize = vswprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
    if ( iSize == -1 )
    {
        // glibc 2.0 - Returns -1 when it hasn't got enough capacity.
        // Duplicate the buffer size until we get enough capacity
        do
        {
            iRequiredCapacity *= 2;
            szDest = static_cast < wchar_t* > ( realloc ( szDest,  ( iRequiredCapacity + 1 ) * sizeof ( wchar_t ) ) );
            va_copy ( vlLocal, vl );
            iSize = vswprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
        } while ( iSize == -1 );
    }
    else if ( iSize > iRequiredCapacity )
    {
        // glibc 2.1 - Returns the required capacity.
        iRequiredCapacity = iSize + 1;
        szDest = static_cast < wchar_t* > ( realloc ( szDest,  ( iRequiredCapacity + 1 ) * sizeof ( wchar_t ) ) );

        va_copy ( vlLocal, vl );
        iSize = vswprintf ( szDest, iRequiredCapacity, szFormat, vlLocal );
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
        std::wstring::assign ( szDest );
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
void WString::Split ( const WString& strDelim, std::vector < WString >& outResult, unsigned int uiMaxAmount, unsigned int uiMinAmount ) const
{
    outResult.clear ();
    unsigned long ulStartPoint = 0;

    while ( true )
    {
        size_t ulPos = find ( strDelim, ulStartPoint );

        if ( ulPos == npos || ( uiMaxAmount > 0 && uiMaxAmount <= outResult.size () + 1 ) )
        {
            if ( ulStartPoint <= length () )
                outResult.push_back ( substr ( ulStartPoint ) );
            break;
        }

        outResult.push_back ( substr ( ulStartPoint, ulPos - ulStartPoint ) );

        ulStartPoint = ulPos + strDelim.length ();
    }

    while ( outResult.size () < uiMinAmount )
        outResult.push_back ( L"" );        
}

//
// Split in two
//
// eg  "a.b.c.d.e" with strDelim == "." and iIndex == 1  gives "a" and "b.c.d.e"
//     "a.b.c.d.e" with strDelim == "." and iIndex == -2 gives "a.b.c" and "d.e"
//
bool WString::Split ( const WString& strDelim, WString* pstrLeft, WString* pstrRight, int iIndex ) const
{
    // Check for self-overwrite
    if ( this == pstrLeft || this == pstrRight )
        return WString ( *this ).Split ( strDelim, pstrLeft, pstrRight, iIndex );

    assert ( iIndex );
    bool bFromEnd = iIndex < 0;
    size_t ulPos;
    if ( !bFromEnd )
    {
        ulPos = 0;
        for ( int i = 0 ; i < iIndex && ulPos != npos ; i++ )
        {
            if ( i )
                ulPos += strDelim.length ();
            if ( ulPos < length () )
            {
                ulPos = find ( strDelim, ulPos );
            }
            else
            {
                ulPos = npos;
                break;
            }
        }
    }
    else
    {
        ulPos = length ();
        for ( int i = 0 ; i < -iIndex && ulPos != npos ; i++ )
        {
            if ( ulPos >= strDelim.length () )
            {
                ulPos -= strDelim.length ();
                ulPos = rfind ( strDelim, ulPos );
            }
            else
            {
                ulPos = npos;
                break;
            }
        }
    }

    if ( ulPos == npos )
    {
        if ( pstrLeft )
            *pstrLeft = bFromEnd ? L"" : c_str ();
        if ( pstrRight )
            *pstrRight = bFromEnd ? c_str () : L"";
        return false;
    }

    if ( pstrLeft )
        *pstrLeft = substr ( 0, ulPos );

    if ( pstrRight )
        *pstrRight = substr ( ulPos + strDelim.length (), length () - ( ulPos + strDelim.length () ) );
 
    return true;
}


//
// Specialization of Split that returns the left side of the split
//
WString WString::SplitLeft ( const WString& strDelim, WString* pstrRight, int iIndex ) const
{
    WString strLeft;
    Split ( strDelim, &strLeft, pstrRight, iIndex );
    return strLeft;
}


//
// Specialization of Split that returns the right side of the split
//
WString WString::SplitRight ( const WString& strDelim, WString* pstrLeft, int iIndex ) const
{
    WString strRight;
    Split ( strDelim, pstrLeft, &strRight, iIndex );
    return strRight;
}


//
// Replace all occurrences of the string szOld with szNew
//
WString WString::Replace ( const wchar_t* szOld, const wchar_t* szNew, bool bSearchJustReplaced ) const
{
    // Check if anything to replace first
    size_t idx = 0;
    if( ( idx = this->find ( szOld, idx ) ) == npos )
        return *this;

    size_t iOldLength = wcslen ( szOld );
    size_t iNewLength = wcslen ( szNew );
    WString strResult = *this;
    do
    {
        strResult.replace ( idx, iOldLength, szNew );
        if ( !bSearchJustReplaced )
            idx += iNewLength;
    }
    while( ( idx = strResult.find ( szOld, idx ) ) != npos );
    return strResult;
}

//
// Case insensitive version of Replace()
//
WString WString::ReplaceI ( const wchar_t* szOld, const wchar_t* szNew, bool bSearchJustReplaced ) const
{
    WString strOldUpper = WStringX ( szOld ).ToUpper ();

    // Check if anything to replace first
    size_t idx = 0;
    if( ( idx = this->ToUpper ().find ( strOldUpper, idx ) ) == npos )
        return *this;

    size_t iOldLength = wcslen ( szOld );
    size_t iNewLength = wcslen ( szNew );
    WString strResult = *this;
    do
    {
        strResult.replace ( idx, iOldLength, szNew );
        if ( !bSearchJustReplaced )
            idx += iNewLength;
    }
    while( ( idx = strResult.ToUpper ().find ( strOldUpper, idx ) ) != npos );
    return strResult;
}


//
// Remove szOlds from the start of the string.
//
WString WString::TrimStart ( const wchar_t* szOld ) const
{
    const size_t uiOldLength = wcslen ( szOld );
    WString strResult = *this;
    while ( strResult.substr ( 0, uiOldLength ) == szOld )
        strResult = strResult.substr ( uiOldLength );
    return strResult;
}

//
// Remove szOlds from the end of the string.
//
WString WString::TrimEnd ( const wchar_t* szOld ) const
{
    const size_t uiOldLength = wcslen ( szOld );
    WString strResult = *this;
    while ( strResult.length () >= uiOldLength && strResult.substr ( strResult.length () - uiOldLength ) == szOld )
        strResult = strResult.substr ( 0, strResult.length () - uiOldLength );
    return strResult;
}

//
// Change to all lower case characters.
//
WString WString::ToLower ( void ) const
{
    WString strResult = *this;
    std::transform ( strResult.begin(), strResult.end(), strResult.begin(), ::tolower );
    return strResult;
}

//
// Change to all upper case characters.
//
WString WString::ToUpper ( void ) const
{
    WString strResult = *this;
    std::transform ( strResult.begin(), strResult.end(), strResult.begin(), ::toupper );
    return strResult;
}

//
// Change '0x0a' or '0x0d' or '0x0d 0x0a' to '\n'.
//
WString WString::ConformLineEndings ( void ) const
{
    assert ( '\n' == '\x0A' );
    if ( std::count( begin(), end(), '\n' ) )
        return Replace ( L"\x0D", L"" );
    else
        return Replace ( L"\x0D", L"\n" );
}

//
// Test if string contains strOther
//
bool WString::Contains ( const WString& strOther ) const
{
    return find ( strOther ) != std::wstring::npos;
}

//
// Test if string contains strOther. Case insensitive.
//
bool WString::ContainsI ( const WString& strOther ) const
{
    return ToUpper ().find ( strOther.ToUpper () ) != std::wstring::npos;
}

//
// Case insensitive compate.
//
bool WString::CompareI ( const WString& strOther ) const
{
    return wcsicmp ( *this, strOther ) == 0;
}

// Fault tolerant version of substr
WString WString::SubStr ( int iPos, int iCount ) const
{
    if ( iPos < 0 )
    {
        iCount += iPos;
        iPos = 0;
    }
    iCount = Max ( 0, iCount );
    if ( iPos + iCount > (int)length () )
    {
        iCount = length () - iPos;
    }
    if ( iCount < 1 )
        return L"";
    return substr ( iPos, iCount );
}

// Left most number of characters
WString WString::Left ( int iCount ) const
{
    return SubStr ( 0, iCount );
}

// Right most number of characters
WString WString::Right ( int iCount ) const
{
    return SubStr ( (int)length () - iCount, iCount );
}


bool WString::EndsWith ( const WString& strOther ) const
{
    return Right ( (int)strOther.length () ) == strOther;
}

bool WString::EndsWithI ( const WString& strOther ) const
{
    return wcsicmp ( Right ( (int)strOther.length () ), strOther ) == 0;
}

bool WString::BeginsWith ( const WString& strOther ) const
{
    return Left ( (int)strOther.length () ) == strOther;
}

bool WString::BeginsWithI ( const WString& strOther ) const
{
    return wcsicmp ( Left ( (int)strOther.length () ), strOther ) == 0;
}

// Static function
WString WString::Join ( const WString& strDelim, const std::vector < WString >& parts, int iFirst, int iCount )
{
    WString strResult;
    int iLast = Min < int > ( iFirst + iCount, parts.size () ) - 1;
    iFirst = Max < int > ( iFirst, 0 );
    for ( int i = iFirst ; i <= iLast ; i++ )
    {
        if ( i != iFirst )
            strResult += strDelim;
        strResult += parts[i];
    }
    return strResult;
}

void WString::AssignLeft ( const wchar_t* szOther, uint uiMaxLength )
{
    assign ( WStringX ( szOther ).Left ( uiMaxLength ) );
}

WString::WString ( const char* szText )
{
    *this = FromUTF8( szText );
}

SString WString::ToAnsi ( void ) const
{
    return ToUTF8( *this );
}

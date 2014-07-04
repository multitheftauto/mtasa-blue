/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        WString.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// class WString
//
// Adds some functionality to the std::wstring class
//
#include <stdarg.h>

#ifdef WIN32
    #define va_copy(dest, orig) (dest) = (orig)
#endif

//
// class WString
//
// wchar_t version of SString
//
class WString : public std::wstring
{
public:
    // Constructors
    WString ( void )
        : std::wstring ()
    { }

    WString ( const wchar_t* szText )
        : std::wstring ( szText ? szText : L"" )
    { }

    WString ( const char* szText );

    explicit WString ( const wchar_t* szFormat, ... )
        : std::wstring ()
    {
        if ( szFormat )
        {
            va_list vl;

            va_start ( vl, szFormat );
            vFormat ( szFormat, vl );
            va_end ( vl );
        }
    }

    WString ( const std::wstring& strText )
        : std::wstring ( strText )
    { }

    WString& Format ( const wchar_t* szFormat, ... )
    {
        va_list vl;

        va_start ( vl, szFormat );
        WString& str = vFormat ( szFormat, vl );
        va_end ( vl );

        return str;
    }

    WString& vFormat ( const wchar_t* szFormat, va_list vl );

    // Access
    wchar_t& operator[]( int iOffset )
    {
        return std::wstring::operator[]( iOffset );
    }

    // Operators  
    WString operator+( const wchar_t* other ) const
    {
        return std::wstring ( *this ) + other;
    }
    WString operator+( const std::wstring& other ) const
    {
        return std::wstring ( *this ) + other;
    }
	WString operator+( const WString& other ) const
    {
        return std::wstring ( *this ) + other;
    }

    // Assignment  
    operator const wchar_t*() const    { return c_str (); }        // Auto assign to const wchar_t* without using c_str()
    const wchar_t* operator*( void ) const
    {
        return c_str ();
    }

    // Functions
    void            Split               ( const WString& strDelim, std::vector < WString >& outResult, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0 ) const;
    bool            Split               ( const WString& strDelim, WString* pstrLeft, WString* pstrRight, int iIndex = 1 ) const;
    WString         SplitLeft           ( const WString& strDelim, WString* pstrRight = NULL, int iIndex = 1 ) const;
    WString         SplitRight          ( const WString& strDelim, WString* pstrLeft = NULL, int iIndex = 1 ) const;
    WString         Replace             ( const wchar_t* szOld, const wchar_t* szNew, bool bSearchJustReplaced = false ) const;
    WString         ReplaceI            ( const wchar_t* szOld, const wchar_t* szNew, bool bSearchJustReplaced = false ) const;
    WString         TrimStart           ( const wchar_t* szOld ) const;
    WString         TrimEnd             ( const wchar_t* szOld ) const;
    WString         ToLower             ( void ) const;
    WString         ToUpper             ( void ) const;
    WString         ConformLineEndings  ( void ) const;
    bool            Contains            ( const WString& strOther ) const;
    bool            ContainsI           ( const WString& strOther ) const;
    bool            CompareI            ( const WString& strOther ) const;
    WString         SubStr              ( int iPos, int iCount = 0x3fffffff ) const;
    WString         Left                ( int iCount ) const;
    WString         Right               ( int iCount ) const;
    bool            EndsWith            ( const WString& strOther ) const;
    bool            EndsWithI           ( const WString& strOther ) const;
    bool            BeginsWith          ( const WString& strOther ) const;
    bool            BeginsWithI         ( const WString& strOther ) const;
    static WString  Join                ( const WString& strDelim, const std::vector < WString >& parts, int iFirst = 0, int iCount = 0x3fffffff );
    void            AssignLeft          ( const wchar_t* szOther, uint uiMaxLength );

    SString         ToAnsi              ( void ) const;
};


class WStringX : public WString
{
public:
    WStringX ( const wchar_t* szText )
        : WString ( std::wstring ( szText ? szText : L"" ) )
    { }
    WStringX ( const wchar_t* szText, uint uiLength )
        : WString ( std::wstring ( szText ? szText : L"", uiLength ) )
    { }
};

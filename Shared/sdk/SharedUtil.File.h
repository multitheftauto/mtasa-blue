/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.File.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef _WIN32
#define _File_PathCharComp( c1, c2 ) ( tolower( c1 ) == tolower( c2 ) )

struct char_traits_i : public std::char_traits <char>
{
    static bool __CLRCALL_OR_CDECL eq( const char left, const char right )
    {
        return toupper(left) == toupper(right);
    }

    static bool __CLRCALL_OR_CDECL ne( const char left, const char right )
    {
        return toupper(left) != toupper(right);
    }

    static bool __CLRCALL_OR_CDECL lt( const char left, const char right )
    {
        return toupper(left) < toupper(right);
    }

    static int __CLRCALL_OR_CDECL compare( const char *s1, const char *s2, size_t n )
    {
        while ( n-- )
        {
            if ( toupper(*s1) < toupper(*s2) )
                return -1;

            if ( toupper(*s1) > toupper(*s2) )
                return 1;

            ++s1; ++s2;
        }

        return 0;
    }

    static const char* __CLRCALL_OR_CDECL find( const char *s, size_t cnt, char a )
    {
        while ( cnt-- && toupper(*s++) != toupper(a) );

        return s;
    }
};

class filePath : public std::basic_string <char, char_traits_i>
{
    typedef std::basic_string <char, char_traits_i> _baseString;

public:
    filePath( const std::string& right )
        : _baseString( right.c_str(), right.size() )
    { }

    bool operator ==( const filePath& right ) const
    {
        if ( right.size() != size() )
            return false;

        return compare( 0, size(), right.c_str() ) == 0;
    }

    bool operator ==( const char *right ) const
    {
        return compare( right ) == 0;
    }

    bool operator ==( const std::string& right ) const
    {
        if ( right.size() != size() )
            return false;

        return compare( 0, size(), right.c_str() ) == 0;
    }

    filePath& operator +=( char right )
    {
        push_back( right );
        return *this;
    }

    filePath& operator +=( const char *right )
    {
        append( right );
        return *this;
    }

    filePath& operator +=( const filePath& right )
    {
        append( right.c_str(), right.size() );
        return *this;
    }

    filePath& operator +=( const std::string& right )
    {
        append( right.c_str(), right.size() );
        return *this;
    }

    filePath operator +( const std::string& right ) const
    {
        return filePath( *this ).append( right.c_str(), right.size() );
    }

    filePath operator +( const char right ) const
    {
        filePath outPath( *this );
        outPath.push_back( right );
        return outPath;
    }

    operator std::string () const
    {
        return std::string( c_str(), size() );
    }
#else
#define _File_PathCharComp( c1, c2 ) ( c1 == c2 )

class filePath : public std::string
{
    typedef std::string _baseString;
public:
#endif
    filePath()
        : _baseString()
    {
    }

    filePath( const filePath& right )
        : _baseString( right.c_str(), right.size() )
    { }

    filePath( const _baseString& right )
        : _baseString( right )
    { }

    explicit filePath( const SString& right )
        : _baseString( right.c_str(), right.size() )
    { }

    filePath( const char *right, size_t len )
        : _baseString( right, len )
    { }

    filePath( const char *right )
        : _baseString( right )
    { }

    filePath GetFilename() const
    {
        size_t pos = rfind( '/' );

        if ( pos == -1 )
            if ( ( pos = rfind( '\\' ) ) == -1 )
                return filePath( *this );

        return substr( pos + 1, size() );
    }

    filePath GetPath() const
    {
        size_t pos = rfind( '/' );

        if ( pos == -1 )
            if ( ( pos = rfind( '\\' ) ) == -1 )
                return filePath();

        return substr( 0, pos );
    }

    const char* GetExtension() const
    {
        size_t pos = rfind( '.' );
        size_t posSlash;

        if ( pos == -1 )
            return NULL;

        posSlash = rfind( '/' );

        if ( posSlash == -1 )
            if ( ( posSlash = rfind( '\\' ) ) == -1 )
                return c_str() + pos;

        if ( posSlash > pos )
            return NULL;
        
        return c_str() + pos;
    }

    bool IsDirectory() const
    {
        if ( empty() )
            return false;

        return *rbegin() == '/' || *rbegin() == '\\';
    }

    char operator []( int idx ) const
    {
        return _baseString::operator []( idx );
    }

    operator const char* () const
    {
        return c_str();
    }

    const char* operator* () const
    {
        return c_str();
    }

    operator SString () const
    {
        return SString( c_str(), size() );
    }

    filePath operator +( const _baseString& right ) const
    {
        return _baseString( *this ) + right;
    }

    filePath operator +( const char *right ) const
    {
        return _baseString( *this ) + right;
    }
};

namespace SharedUtil
{
    //
    // Returns true if the file/directory exists
    //
    bool            FileExists                      ( const SString& strFilename );
    bool            DirectoryExists                 ( const SString& strPath );

    //
    // Load from a file
    //
    bool            FileLoad                        ( const SString& strFilename, std::vector < char >& buffer, int iMaxSize = 0x7FFFFFFF );
    bool            FileLoad                        ( const SString& strFilename, SString& strBuffer, int iMaxSize = 0x7FFFFFFF );

    //
    // Save to a file
    //
    bool            FileSave                        ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true );
    bool            FileSave                        ( const SString& strFilename, const SString& strBuffer, bool bForce = true );

    //
    // Append to a file
    //
    bool            FileAppend                      ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true );
    bool            FileAppend                      ( const SString& strFilename, const SString& strBuffer, bool bForce = true );

    //
    // Get a file size
    //
    uint            FileSize                        ( const SString& strFilename );

    //
    // Ensure all directories exist to the file
    //
    void            MakeSureDirExists               ( const SString& strPath );

    SString         PathConform                     ( const SString& strInPath );
    SString         PathJoin                        ( const SString& str1, const SString& str2 );
    SString         PathJoin                        ( const SString& str1, const SString& str2, const SString& str3, const SString& str4 = "", const SString& str5 = "" );
    SString         PathMakeRelative                ( const SString& strInBasePath, const SString& strInAbsPath );
    void            ExtractFilename                 ( const SString& strPathFilename, SString* strPath, SString* strFilename );
    bool            ExtractExtension                ( const SString& strFilename, SString* strRest, SString* strExt );
    SString         ExtractPath                     ( const SString& strPathFilename );
    SString         ExtractFilename                 ( const SString& strPathFilename );
    SString         ExtractExtension                ( const SString& strPathFilename );
    SString         ExtractBeforeExtension          ( const SString& strPathFilename );

    bool            FileDelete                      ( const SString& strFilename, bool bForce = true );
    bool            FileRename                      ( const SString& strFilenameOld, const SString& strFilenameNew );
    bool            DelTree                         ( const SString& strPath, const SString& strInsideHere );
    bool            MkDir                           ( const SString& strInPath, bool bTree = true );
    bool            FileCopy                        ( const SString& strSrc, const SString& strDest, bool bForce = true );
    SString         GetCurrentWorkingDirectory      ( void );
    SString         GetCurrentDirectory             ( void );
    SString         GetWindowsDirectory             ( void );
    std::vector < SString > FindFiles               ( const SString& strMatch, bool bFiles, bool bDirectories );
    SString         MakeUniquePath                  ( const SString& strPathFilename );
    SString         ConformPathForSorting           ( const SString& strPathFilename );

    SString         GetSystemLocalAppDataPath       ( void );
    SString         GetSystemCommonAppDataPath      ( void );
    SString         GetSystemWindowsPath            ( void );
    SString         GetSystemSystemPath             ( void );
    SString         GetSystemTempPath               ( void );
    SString         GetMTADataPath                  ( void );
    SString         GetMTATempPath                  ( void );
}

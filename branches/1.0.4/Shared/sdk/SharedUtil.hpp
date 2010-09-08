/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.hpp
*  PURPOSE:     Solution wide utility functions.
*               Include this file once per dll.
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*               Cecill Etheredge <ijsf@gmx-topmail.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <assert.h>
#ifdef WIN32
    #include <direct.h>
#endif

#ifdef WIN32
//
// Get startup directory as saved in the registry by the launcher
// Used in the Win32 Client only
//
SString SharedUtil::GetMTASABaseDir()
{
    static TCHAR szInstallRoot[MAX_PATH]=TEXT("");
    if( !szInstallRoot[0] )
    {
        memset ( szInstallRoot, 0, MAX_PATH );

        HKEY hkey = NULL;
        DWORD dwBufferSize = MAX_PATH;
        DWORD dwType = 0;
        if ( RegOpenKeyEx ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", 0, KEY_READ, &hkey ) == ERROR_SUCCESS )
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
SString SharedUtil::CalcMTASAPath ( const SString& strPath )
{
    SString strNewPath = GetMTASABaseDir();
    strNewPath += '\\';
    strNewPath += strPath;
    return strNewPath;
}


//
// Write a registry string value
//
static void WriteRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, const SString& strBuffer )
{
    HKEY hkTemp;
    RegCreateKeyEx ( hkRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkTemp, NULL );
    if ( hkTemp )
    {
        RegSetValueEx ( hkTemp, szValue, NULL, REG_SZ, (LPBYTE)strBuffer.c_str (), strBuffer.length () + 1 );
        RegCloseKey ( hkTemp );
    }
}

//
// Run ShellExecute with these parameters after exit
//
void SharedUtil::SetOnQuitCommand ( const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory, const SString& strShowCmd )
{
    // Encode into a string and set a registry key
    SString strValue ( "%s\t%s\t%s\t%s\t%s", strOperation.c_str (), strFile.c_str (), strParameters.c_str (), strDirectory.c_str (), strShowCmd.c_str () );
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnQuitCommand", strValue );
}

//
// What to do on next restart
//
void SharedUtil::SetOnRestartCommand ( const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory, const SString& strShowCmd )
{
    // Encode into a string and set a registry key
    SString strValue ( "%s\t%s\t%s\t%s\t%s", strOperation.c_str (), strFile.c_str (), strParameters.c_str (), strDirectory.c_str (), strShowCmd.c_str () );
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnRestartCommand", strValue );
}

//
// For tracking results of new features
//
void SharedUtil::AddReportLog ( const SString& strText )
{
    SString strPathFilename = CalcMTASAPath ( std::string ( "\\mta\\report.log" )  );
    MakeSureDirExists ( strPathFilename );

    SString strMessage = GetLocalTimeString ( true, false ) + " - " + strText + "\n";
    FileAppend ( strPathFilename, &strMessage.at ( 0 ), strMessage.length () );
}

#else
    unsigned long   GetTickCount                ( void );
    #include <sys/time.h>

#endif


//
// Retrieves the number of milliseconds that have elapsed since the system was started.
//
// GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
// an __int64 and will effectively never wrap. This is an emulated version for XP and down.
// Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
//
long long SharedUtil::GetTickCount64_ ( void )
{
    static CCriticalSection criticalSection;
    criticalSection.Lock ();

    static long          lHightPart = 0;
    static unsigned long ulWas      = GetTickCount ();
    unsigned long        ulNow      = GetTickCount ();
    unsigned long        ulDelta    = ulNow - ulWas;

    // Detect wrap around
    if( ulDelta > 0x80000000 )
        lHightPart++;

    ulWas = ulNow;

    long long Result = ( ( ( ( long long ) lHightPart ) << 32 ) | ( ( long long ) ulNow ) );

    criticalSection.Unlock ();
    return Result;
}


//
// Retrieves the number of seconds that have elapsed since the system was started
//
double SharedUtil::GetSecondCount ( void )
{
    return GetTickCount64_ () * ( 1 / 1000.0 );
}



//
// Cross platform critical section
//
#ifdef WIN32

    SharedUtil::CCriticalSection::CCriticalSection ( void )
    {
        m_pCriticalSection = new CRITICAL_SECTION;
        InitializeCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
    }

    SharedUtil::CCriticalSection::~CCriticalSection ( void )
    {
        DeleteCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
        delete ( CRITICAL_SECTION* ) m_pCriticalSection;
    }

    void SharedUtil::CCriticalSection::Lock ( void )
    {
        if ( m_pCriticalSection )
            EnterCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
    }

    void SharedUtil::CCriticalSection::Unlock ( void )
    {
        if ( m_pCriticalSection )
            LeaveCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
    }

#else
    #include <pthread.h>

    SharedUtil::CCriticalSection::CCriticalSection ( void )
    {
        m_pCriticalSection = new pthread_mutex_t;
        pthread_mutex_init ( ( pthread_mutex_t* ) m_pCriticalSection, NULL );
    }

    SharedUtil::CCriticalSection::~CCriticalSection ( void )
    {
        pthread_mutex_destroy ( ( pthread_mutex_t* ) m_pCriticalSection );
        delete ( pthread_mutex_t* ) m_pCriticalSection;
    }

    void SharedUtil::CCriticalSection::Lock ( void )
    {
        pthread_mutex_lock ( ( pthread_mutex_t* ) m_pCriticalSection );
    }

    void SharedUtil::CCriticalSection::Unlock ( void )
    {
        pthread_mutex_unlock ( ( pthread_mutex_t* ) m_pCriticalSection );
    }

#endif


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
        *pstrRight = substr ( ulPos + 1, length () - ( ulPos + 1 ) );
 
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
// Remove szOlds from the start of the string.
//
SString SString::TrimStart ( const char* szOld ) const
{
    int iOldLength = strlen ( szOld );
    SString strResult = *this;
    while ( strResult.substr ( 0, iOldLength ) == szOld )
        strResult = strResult.substr ( iOldLength );
    return strResult;
}

//
// Remove szOlds from the end of the string.
//
SString SString::TrimEnd ( const char* szOld ) const
{
    int iOldLength = strlen ( szOld );
    SString strResult = *this;
    while ( strResult.substr ( strResult.length () - iOldLength ) == szOld )
        strResult = strResult.substr ( 0, strResult.length () - iOldLength );
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
// Cross-platform GetTickCount() implementations
//   Returns the number of milliseconds since some fixed point in time.
//   Wraps every 49.71 days and should always increase monotonically.
//
// ACHTUNG: This function should be scrapped and replaced by a cross-platform time class
//
#if defined(__APPLE__) && defined(__MACH__)

// Apple / Darwin platforms with Mach monotonic clock support
#include <mach/mach_time.h>
unsigned long GetTickCount ( void )
{
    mach_timebase_info_data_t info;

    // Get the absolute time
    uint64_t nAbsTime = mach_absolute_time();

    // Get the timebase info (could be done once, statically?)
    mach_timebase_info( &info );

    // Calculate the time in milliseconds
    uint64_t nNanoTime = nAbsTime * ( info.numer / info.denom );
    return ( nNanoTime / 1000000 );
}

#elif !defined(WIN32)

// BSD / Linux platforms with POSIX monotonic clock support
unsigned long GetTickCount ( void )
{
    #if !defined(CLOCK_MONOTONIC)
    #error "This platform does not have monotonic clock support."
    #endif

    /*
    ** clock_gettime() is granted to be increased monotonically when the
    ** monotonic clock is queried. Time starting point is unspecified, it
    ** could be the system start-up time, the Epoch, or something else,
    ** in any case the time starting point does not change once that the
    ** system has started up.
    */
    struct timeval now;
    struct timespec tsnow;
    if(0 == clock_gettime(CLOCK_MONOTONIC, &tsnow)) {
        now.tv_sec = tsnow.tv_sec;
        now.tv_usec = tsnow.tv_nsec / 1000;
    }
    /*
    ** Even when the configure process has truly detected monotonic clock
    ** availability, it might happen that it is not actually available at
    ** run-time. When this occurs simply fallback to other time source.
    */
    else
        (void)gettimeofday(&now, NULL);

    // ACHTUNG: Note that the above gettimeofday fallback is dangerous because it is a wall clock
    // and thus not guaranteed to be monotonic. Ideally, this function should throw a fatal error
    // or assertion instead of using a fallback method.

    long long llMilliseconds = ( ( long long ) now.tv_sec ) * 1000 + now.tv_usec / 1000;
    return llMilliseconds;
}
#endif


//
// Expiry stuff
//
#ifdef WIN32
#if defined(MTA_DM_EXPIRE_DAYS)
    #include <time.h>

    #define YEAR ((((__DATE__ [7]-'0')*10+(__DATE__ [8]-'0'))*10+(__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))

    /* Month: 0 - 11 */
    #define MONTH (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 0 : 5) \
                  : __DATE__ [2] == 'b' ? 1 \
                  : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M'? 2 : 3) \
                  : __DATE__ [2] == 'y' ? 4 \
                  : __DATE__ [2] == 'l' ? 6 \
                  : __DATE__ [2] == 'g' ? 7 \
                  : __DATE__ [2] == 'p' ? 8 \
                  : __DATE__ [2] == 't' ? 9 \
                  : __DATE__ [2] == 'v' ? 10 : 11)

    #define DAY ((__DATE__ [4]==' ' ? 0 : __DATE__[4]-'0')*10+(__DATE__[5]-'0'))

    int SharedUtil::GetDaysUntilExpire ( void )
    {
        tm when;
        memset ( &when, 0, sizeof ( when ) );
        when.tm_year = YEAR - 1900;
        when.tm_mon = MONTH;
        when.tm_mday = DAY + MTA_DM_EXPIRE_DAYS;
        return ( int )( mktime( &when ) - time ( NULL ) ) / ( 60 * 60 * 24 );
    }

#endif
#endif


// Copied from CChatLine::RemoveColorCode()
std::string SharedUtil::RemoveColorCode ( const char* szString )
{
    std::string strOut;
    const char* szStart = szString;
    const char* szEnd = szString;

    while ( true )
    {
        if ( *szEnd == '\0' )
        {
            strOut.append ( szStart, szEnd - szStart );
            break;
        }
        else
        {
            bool bIsColorCode = false;
            if ( *szEnd == '#' )
            {
                bIsColorCode = true;
                for ( int i = 0; i < 6; i++ )
                {
                    char c = szEnd [ 1 + i ];
                    if ( !isdigit ( (unsigned char)c ) && (c < 'A' || c > 'F') && (c < 'a' || c > 'f') )
                    {
                        bIsColorCode = false;
                        break;
                    }
                }
            }

            if ( bIsColorCode )
            {
                strOut.append ( szStart, szEnd - szStart );
                szStart = szEnd + 7;
                szEnd = szStart;
            }
            else
            {
                szEnd++;
            }
        }
    }

    return strOut;
}


//
// Get the local time in a string.
// Set bDate to include the date, bMs to include milliseconds
//
SString SharedUtil::GetLocalTimeString ( bool bDate, bool bMilliseconds )
{
#ifdef _WIN32
    SYSTEMTIME s;
    GetLocalTime( &s );

    SString strResult = SString ( "%02d:%02d:%02d", s.wHour, s.wMinute, s.wSecond );
    if ( bMilliseconds )
        strResult += SString ( ":%04d", s.wMilliseconds );
    if ( bDate )
        strResult = SString ( "%02d-%02d-%02d ", s.wYear, s.wMonth, s.wDay  ) + strResult;
    return strResult;
#else
    timeval now;
    gettimeofday ( &now, NULL );
    time_t t = now.tv_sec;
    tm* tmp = localtime ( &t );
    assert ( tmp );

    char outstr[200] = { 0 };
    if ( bDate )
        strftime ( outstr, sizeof ( outstr ), "%y-%m-%d %H:%M:%S", tmp );
    else
        strftime ( outstr, sizeof ( outstr ), "%H:%M:%S", tmp );

    if ( !bMilliseconds )
        return outstr;

    return SString ( "%s.%03d", outstr, now.tv_usec / 1000 );
#endif
}


//
// Output timestamped line into the debugger
//
void SharedUtil::OutputDebugLine ( const char* szMessage )
{
    SString strMessage = GetLocalTimeString ( false, true ) + " - " + szMessage;
    if ( strMessage.length () > 0 && strMessage[ strMessage.length () - 1 ] != '\n' )
        strMessage += "\n";
#ifdef _WIN32
    OutputDebugString ( strMessage );
#else
    // Other platforms here
#endif
}


//
// Returns true if the file exists
//
bool SharedUtil::FileExists ( const SString& strFilename )
{
    FILE* fh = fopen ( strFilename, "rb" );
    if ( !fh )
        return false;
    fclose ( fh );
    return true;
}


//
// Load binary data from a file into an array
//
bool SharedUtil::FileLoad ( const SString& strFilename, std::vector < char >& buffer )
{
    buffer.clear ();
    // Open
    FILE* fh = fopen ( strFilename, "rb" );
    if ( !fh )
        return false;
    // Get size
    fseek ( fh, 0, SEEK_END );
    int size = ftell ( fh );
    rewind ( fh );

    int bytesRead = 0;
    if ( size > 0 && size < 1e9 )
    {
        // Allocate space
        buffer.assign ( size, 0 );
        // Read into buffer
        bytesRead = fread ( &buffer.at ( 0 ), 1, size, fh );
    }
    // Close
    fclose ( fh );
    return bytesRead == size;
}


//
// Save binary data to a file
//
bool SharedUtil::FileSave ( const SString& strFilename, void* pBuffer, unsigned long ulSize )
{
    FILE* fh = fopen ( strFilename, "wb" );
    if ( !fh )
        return false;

    bool bSaveOk = true;
    if ( ulSize )
        bSaveOk = ( fwrite ( pBuffer, 1, ulSize, fh ) == ulSize );
    fclose ( fh );
    return bSaveOk;
}


//
// Append binary data to a file
//
bool SharedUtil::FileAppend ( const SString& strFilename, void* pBuffer, unsigned long ulSize )
{
    FILE* fh = fopen ( strFilename, "ab" );
    if ( !fh )
        return false;

    bool bSaveOk = true;
    if ( ulSize )
        bSaveOk = ( fwrite ( pBuffer, 1, ulSize, fh ) == ulSize );
    fclose ( fh );
    return bSaveOk;
}


//
// Ensure all directories exist to the file
//
void SharedUtil::MakeSureDirExists ( const char* szPath )
{
    // Copy the path
    char szCopy [MAX_PATH];
    strncpy ( szCopy, szPath, MAX_PATH );

    // Begin from the start
    char cChar = 0;
    char* szIter = szCopy;
    while ( *szIter != 0 )
    {
        // Met a slash?
        cChar = *szIter;
        if ( cChar == '\\' ||
             cChar == '/' )
        {
            // Replace it temprarily with 0
            *szIter = 0;

            // Call mkdir on this path
            #ifdef WIN32
                mkdir ( szCopy );
            #else
                mkdir ( szCopy ,0775 );
            #endif

            // Make it a slash again
            *szIter = cChar;
        }

        // Increment iterator
        ++szIter;
    }
}


//
// Return true if supplied string adheres to the new version format
//
bool SharedUtil::IsValidVersionString ( const SString& strVersion )
{
    SString strCheck = "0.0.0-0-00000.0";
    if ( strCheck.length () != strVersion.length () )
        return false;
    for ( unsigned int i = 0 ; i < strVersion.length () ; i++ )
    {
        char c = strVersion[i];
        char d = strCheck[i];
        if ( c != d && isdigit( c ) != isdigit( d ) )
            return false;
    }
    return true;
}


//
// Try to make a path relative to the 'resources/' directory
//
SString SharedUtil::ConformResourcePath ( const char* szRes )
{
    // Remove up to first '/resources/'
    // else
    // remove up to first '/resource-cache/unzipped/'
    // else
    // remove up to first '/deathmatch/'
    // else
    // if starts with '...'
    //  remove up to first '/'

    SString strDelimList[] = { "/resources/", "/resource-cache/unzipped/", "/deathmatch/" };
    SString strText = szRes ? szRes : "";
#ifdef WIN32
    char cPathSep = '\\';
    for ( unsigned int i = 0 ; i < NUMELMS ( strDelimList ) ; i++ )
        strDelimList[i] = strDelimList[i].Replace ( "/", "\\" );
    strText = strText.Replace ( "/", "\\" );
#else
    char cPathSep = '/';
    for ( unsigned int i = 0 ; i < NUMELMS ( strDelimList ) ; i++ )
        strDelimList[i] = strDelimList[i].Replace ( "\\", "/" );
    strText = strText.Replace ( "\\", "/" );
#endif

    for ( unsigned int i = 0 ; i < NUMELMS ( strDelimList ) ; i++ )
    {
        // Remove up to first occurrence
        int iPos = strText.find ( strDelimList[i] );
        if ( iPos >= 0 )
            return strText.substr ( iPos + strDelimList[i].length () );
    }

    if ( strText.substr ( 0, 3 ) == "..." )
    {
        // Remove up to first '/'
        int iPos = strText.find ( cPathSep );
        if ( iPos >= 0 )
            return strText.substr ( iPos + 1 );
    }

    return strText;
}





//
//
// Memory usage tracking
//
// Don't look!
//
//
#if WITH_ALLOC_TRACKING

#undef map
#ifdef WIN32
    #undef malloc
    #undef realloc
    #undef calloc
    #undef free
#endif

using namespace std;
namespace
{
    //#define maptype hash_map
    #define maptype map
    // Update or add a value for a key
    template < class T, class V, class TR, class T2 >
    void xMapSet ( maptype < T, V, TR >& collection, const T2& key, const V& value )
    {
        collection[ key ] = value;
    }

    // Returns true if the item is in the collection
    template < class T, class V, class TR, class T2 >
    bool xMapContains ( const maptype < T, V, TR >& collection, const T2& key )
    {
        return collection.find ( key ) != collection.end ();
    }

    // Remove key from collection
    template < class T, class V, class TR, class T2 >
    void xMapRemove ( maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::iterator it = collection.find ( key );
        if ( it != collection.end () )
            collection.erase ( it );
    }

    // Find value in collection
    template < class T, class V, class TR, class T2 >
    V* xMapFind ( maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class TR, class T2 >
    const V* xMapFind ( const maptype < T, V, TR >& collection, const T2& key )
    {
        typename maptype < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }
}

struct CAllocInfo
{
    int elementSize;
    int size;
    int countRealloc;
    int allocIndex;
    char tag[28];
};


static int stats_ready = 1;
#ifdef ALLOC_STATS_MODULE_NAME
    #ifndef ALLOC_STATS_PRE_COUNT
        #define ALLOC_STATS_PRE_COUNT 200     // Increase if crash at startup
    #endif
    static int pre_count = ALLOC_STATS_PRE_COUNT;
#else
    #define ALLOC_STATS_MODULE_NAME "none"
    static int pre_count = -1;
#endif

static int no_stuff = 0;

typedef unsigned char BYTE;

class CAllocStats
{
public:
    maptype < BYTE*, CAllocInfo >   allocMap;
    int                             allocIndex;
    CCriticalSection                cs;

    unsigned long TotalMem;
    unsigned long TotalMemMax;
    unsigned long ActiveAllocs;
    unsigned long DupeAllocs;
    unsigned long UniqueAllocs;
    unsigned long ReAllocs;
    unsigned long Frees;
    unsigned long UnmatchedFrees;
    unsigned long DupeMem;

    CAllocStats()
    {
        TotalMem = 0;
        TotalMemMax = 0;
        allocIndex = 0;
        ActiveAllocs = 0;
        DupeAllocs = 0;
        UniqueAllocs = 0;
        ReAllocs = 0;
        Frees = 0;
        UnmatchedFrees = 0;
        DupeAllocs = 0;
        DupeMem = 0;
    }

    void AddAllocInfo( BYTE* pData, size_t Count, const char* Tag, int ElementSize )
    {
        cs.Lock();
        no_stuff++;
        if( !xMapContains( allocMap, pData ) )
        {
            CAllocInfo info;
            info.elementSize = ElementSize;
            info.size = Count;
            info.countRealloc = 0;
            info.allocIndex = allocIndex++;
            STRNCPY( info.tag, Tag ? Tag : "", 28 );

            xMapSet( allocMap, pData, info );

            ActiveAllocs++;
            UniqueAllocs++;
            TotalMem += Count;
            TotalMemMax = Max ( TotalMemMax, TotalMem );
        }
        else
        {
            DupeAllocs++;
            DupeMem += Count;
        }
        no_stuff--;
        cs.Unlock();
    }

    void MoveAllocInfo( BYTE* pOrig, BYTE* pNew, size_t Count, const char* Tag, int ElementSize )
    {
        cs.Lock();
        no_stuff++;
        CAllocInfo info;
        int SizeDif = Count;
        if( xMapContains( allocMap, pOrig ) )
        {
            info = *xMapFind ( allocMap, pOrig );
            xMapRemove( allocMap, pOrig );
            SizeDif = Count - info.size;
        }

        info.elementSize = ElementSize;
        info.size = Count;
        info.countRealloc++;
        info.allocIndex = allocIndex++;
        ReAllocs++;

        xMapSet( allocMap, pNew, info );

        TotalMem += SizeDif;
        TotalMemMax = Max ( TotalMemMax, TotalMem );
        no_stuff--;
        cs.Unlock();
    }

    void RemoveAllocInfo( BYTE* pOrig )
    {
        cs.Lock();
        no_stuff++;
        if( xMapContains( allocMap, pOrig ) )
        {
            CAllocInfo* pInfo = xMapFind ( allocMap, pOrig );
            TotalMem -= pInfo->size;
            xMapRemove( allocMap, pOrig );
            ActiveAllocs--;
            Frees++;
        }
        else
        {
            UnmatchedFrees++;
        }
        no_stuff--;
        cs.Unlock();
    }
};

static CAllocStats* pAllocStats = NULL;

static CAllocStats* GetAllocStats()
{
    if ( !pAllocStats )
    {
        no_stuff++;
        pAllocStats = new CAllocStats();
        no_stuff--;
    }
    return pAllocStats;
}


typedef unsigned char BYTE;

void* myMalloc ( size_t Count, const char* Tag, int ElementSize )
{
    if ( pre_count > 0 )
        pre_count--;

    if ( no_stuff || pre_count )
        return malloc ( Count );

    BYTE* pData = (BYTE*)malloc ( Count );

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->AddAllocInfo( pData, Count, Tag, ElementSize );
    }

    return pData;
}

void* myCalloc ( size_t Count, size_t elsize, const char* Tag, int ElementSize )
{
    void* pData = myMalloc ( Count * elsize, Tag, ElementSize );
    memset ( pData, 0, Count * elsize );
    return pData;
}

void* myRealloc ( void* Original, size_t Count, const char* Tag, int ElementSize )
{
    if ( no_stuff || pre_count )
        return realloc ( Original, Count );

    BYTE* pData = (BYTE*)realloc ( Original, Count );

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->MoveAllocInfo( (BYTE*)Original, pData, Count, Tag, ElementSize );
    }

    return pData;
}

void myFree ( void* Original )
{
    if ( no_stuff || pre_count )
        return free ( Original );

    if ( !Original )
        return;

    if ( stats_ready )
    {
        CAllocStats* pAllocStats = GetAllocStats();
        pAllocStats->RemoveAllocInfo( (BYTE*)Original );
    }

    free ( Original );
}

void* myNew ( std::size_t size, const char* Tag, int ElementSize )
{
    return myMalloc ( size, Tag, ElementSize );
}

void myDelete (void* ptr)
{
    myFree ( ptr );
}

#define map CMap
#ifdef WIN32
    #define malloc _malloc_
    #define realloc _realloc_
    #define calloc _calloc_
    #define free _free_
#endif

// Set up export type definition for Win32
#ifdef WIN32
    #define MTAEXPORT extern "C" __declspec(dllexport)
#else
    #define MTAEXPORT extern "C"
#endif

//
// Get memory stats from this dll
//
// Returns number of stats copied
//
MTAEXPORT unsigned long GetAllocStats( unsigned long* pOutStats, unsigned long ulNumStats )
{
    CAllocStats* pAllocStats = GetAllocStats();

    if ( ulNumStats > 0 )   pOutStats[0] = pAllocStats->TotalMem;
    if ( ulNumStats > 1 )   pOutStats[1] = pAllocStats->TotalMemMax;
    if ( ulNumStats > 2 )   pOutStats[2] = pAllocStats->ActiveAllocs;
    if ( ulNumStats > 3 )   pOutStats[3] = pAllocStats->DupeAllocs;
    if ( ulNumStats > 4 )   pOutStats[4] = pAllocStats->UniqueAllocs;
    if ( ulNumStats > 5 )   pOutStats[5] = pAllocStats->ReAllocs;
    if ( ulNumStats > 6 )   pOutStats[6] = pAllocStats->Frees;
    if ( ulNumStats > 7 )   pOutStats[7] = pAllocStats->UnmatchedFrees;
    if ( ulNumStats > 8 )   pOutStats[8] = pAllocStats->DupeMem;

    return Min < unsigned long > ( ulNumStats, 9 );
}

#endif  // WITH_ALLOC_TRACKING
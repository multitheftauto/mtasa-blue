/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Misc.hpp
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*               Cecill Etheredge <ijsf@gmx-topmail.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

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
// Read a registry string value
//
static SString ReadRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, int* iResult )
{
    // Clear output
    SString strOutResult = "";

    bool bResult = false;
    HKEY hkTemp = NULL;
    if ( RegOpenKeyEx ( hkRoot, szSubKey, 0, KEY_READ, &hkTemp ) == ERROR_SUCCESS ) 
    {
        DWORD dwBufferSize;
        if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, NULL, &dwBufferSize ) == ERROR_SUCCESS )
        {
            char *szBuffer = static_cast < char* > ( alloca ( dwBufferSize + 1 ) );
            if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize ) == ERROR_SUCCESS )
            {
                strOutResult = szBuffer;
                bResult = true;
            }
        }
        RegCloseKey ( hkTemp );
    }
    if ( iResult )
        *iResult = bResult;
    return strOutResult;
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


#ifdef MTASA_VERSION_MAJOR
//
// What to do on next restart
//
void SharedUtil::SetOnRestartCommand ( const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory, const SString& strShowCmd )
{
    // Encode into a string and set a registry key
    SString strVersion ( "%d.%d.%d-%s.%05d" ,MTASA_VERSION_MAJOR ,MTASA_VERSION_MINOR ,MTASA_VERSION_MAINTENANCE ,MTA_DM_BUILDTYPE ,MTASA_VERSION_BUILD );
    SString strValue ( "%s\t%s\t%s\t%s\t%s\t%s", strOperation.c_str (), strFile.c_str (), strParameters.c_str (), strDirectory.c_str (), strShowCmd.c_str (), strVersion.c_str () );
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnRestartCommand", strValue );
}


//
// What to do on next restart
//
bool SharedUtil::GetOnRestartCommand ( SString& strOperation, SString& strFile, SString& strParameters, SString& strDirectory, SString& strShowCmd )
{
    SString strOnRestartCommand = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnRestartCommand", NULL );
    SetOnRestartCommand ( "" );

    std::vector < SString > vecParts;
    strOnRestartCommand.Split ( "\t", vecParts );
    if ( vecParts.size () > 5 && vecParts[0].length () )
    {
        SString strVersion ( "%d.%d.%d-%s.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTA_DM_BUILDTYPE, MTASA_VERSION_BUILD );
        if ( vecParts[5] == strVersion )
        {
            strOperation = vecParts[0];
            strFile = vecParts[1];
            strParameters = vecParts[2];
            strDirectory = vecParts[3];
            strShowCmd = vecParts[4];
            return true;
        }
        AddReportLog( 4000, SString ( "OnRestartCommand disregarded due to version change %s -> %s", vecParts[5].c_str (), strVersion.c_str () ) );
    }
    return false;
}
#endif


//
// For tracking results of new features
//
void SharedUtil::AddReportLog ( uint uiId, const SString& strText )
{
    SString strPathFilename = PathJoin ( GetMTAAppDataPath (), "report.log" );
    MakeSureDirExists ( strPathFilename );

    SString strMessage ( "%u: %s - %s\n", uiId, GetTimeString ( true, false ).c_str (), strText.c_str () );
    FileAppend ( strPathFilename, &strMessage.at ( 0 ), strMessage.length () );
}

void SharedUtil::SetReportLogContents ( const SString& strText )
{
    SString strPathFilename = PathJoin ( GetMTAAppDataPath (), "report.log" );
    MakeSureDirExists ( strPathFilename );
    FileSave ( strPathFilename, strText.length () ? &strText.at ( 0 ) : NULL, strText.length () );
}

SString SharedUtil::GetReportLogContents ( void )
{
    SString strReportFilename = PathJoin ( GetMTAAppDataPath (), "report.log" );
    // Load file into a string
    std::vector < char > buffer;
    FileLoad ( strReportFilename, buffer );
    buffer.push_back ( 0 );
    return &buffer[0];
}
#endif


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
// Expiry stuff
//
#ifdef WIN32
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

    int SharedUtil::GetBuildAge ( void )
    {
        tm when;
        memset ( &when, 0, sizeof ( when ) );
        when.tm_year = YEAR - 1900;
        when.tm_mon = MONTH;
        when.tm_mday = DAY;
        return ( int )( time ( NULL ) - mktime( &when ) ) / ( 60 * 60 * 24 );
    }

#if defined(MTA_DM_EXPIRE_DAYS)
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


#ifdef MTA_DEBUG
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
#endif


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

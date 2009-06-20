/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.hpp
*  PURPOSE:     Solution wide utility functions.
*               Include this file once per dll.
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


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
#endif


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
// Retrieves the number of milliseconds that have elapsed since the system was started.
//
// GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
// an __int64 and will effectively never wrap. This is an emulated version for XP and down.
// Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
//
long long SharedUtil::GetTickCount64_ ( void )
{
    static long lHightPart = 0;
    static DWORD dwWas = GetTickCount ();
    DWORD dwNow = GetTickCount ();
    DWORD dwDelta = dwNow - dwWas;

    // Detect wrap around
    if( dwDelta > 0x80000000 )
        lHightPart++;

    dwWas = dwNow;

    long long Result = ( ( ( ( long long ) lHightPart ) << 32 ) | ( ( long long ) dwNow ) );
    return Result;
}


//
// Retrieves the number of seconds that have elapsed since the system was started
//
double SharedUtil::GetSecondCount ( void )
{
    return GetTickCount64_ () * ( 1 / 1000.0 );
}


// Split into parts
void SString::Split ( const SString& strDelim, std::vector < SString >& outResult ) const
{
    outResult.clear ();
    unsigned long ulStartPoint = 0;

    while ( true )
    {
        unsigned long ulPos = find ( strDelim, ulStartPoint );

        if ( ulPos == npos )
        {
            if ( ulStartPoint < length () )
                outResult.push_back ( substr ( ulStartPoint ) );
            return;
        }

        outResult.push_back ( substr ( ulStartPoint, ulPos - ulStartPoint ) );

        ulStartPoint = ulPos + strDelim.length ();
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        UtilUtil.hpp
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// Simpler, safer, slower sprintf
//
SString SString::Printf( const char* format, ... )
{
    static char buffer[8192];
    int count = sizeof(buffer) / sizeof(buffer[0]);

    va_list argptr;
    va_start( argptr, format );

    _VSNPRINTF ( buffer, count, format, argptr );

    va_end( argptr );

    return buffer;
}


//
// Get startup directory as saved in the registry by the launcher
//
SString GetMTASABaseDir()
{
	static TCHAR szInstallRoot[MAX_PATH]=TEXT("");
	if( !szInstallRoot[0] )
	{
        memset ( szInstallRoot, 0, MAX_PATH );

        HKEY hkey = NULL;
        DWORD dwBufferSize = MAX_PATH;
        DWORD dwType = 0;
        if ( RegOpenKeyEx ( HKEY_LOCAL_MACHINE, "Software\\Multi Theft Auto: San Andreas", 0, KEY_READ, &hkey ) == ERROR_SUCCESS ) 
        {
            // Read out the MTA installpath
            if ( RegQueryValueEx ( hkey, "temp", NULL, &dwType, (LPBYTE)szInstallRoot, &dwBufferSize ) != ERROR_SUCCESS ||
                strlen ( szInstallRoot ) == 0 )
            {
                MessageBox ( 0, "Multi Theft Auto has not been installed properly, please reinstall.", "Error", MB_OK );
                RegCloseKey ( hkey );
                TerminateProcess ( GetCurrentProcess (), 9 );
            }
        }
	}
	return szInstallRoot;
}


//
// Turns a relative MTASA path i.e. "MTA\file.dat"
// into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
//
SString CalcMTASAPath ( const SString& strPath )
{
    SString strNewPath = GetMTASABaseDir();
    strNewPath += '\\';
    strNewPath += strPath;
    return strNewPath;
}



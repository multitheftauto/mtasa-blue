/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "SString.h"

//
// _vsnprintf with buffer full check
//
#define _VSNPRINTF( buffer, count, format, argptr ) \
    { \
        int iResult = _vsnprintf ( buffer, count, format, argptr ); \
        if( iResult == -1 || iResult == (count) ) \
            (buffer)[(count)-1] = 0; \
    }

#ifndef WIN32
    #define _vsnprintf vsnprintf
#endif


namespace SharedUtil
{
    //
    // Get startup directory as saved in the registry by the launcher
    // Used in the Win32 Client only
    //
    static SString GetMTASABaseDir()
    {
	#ifdef WIN32
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
	#endif
    }

    //
    // Turns a relative MTASA path i.e. "MTA\file.dat"
    // into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
    //
    static SString CalcMTASAPath ( const SString& strPath )
    {
        SString strNewPath = GetMTASABaseDir();
        strNewPath += '\\';
        strNewPath += strPath;
        return strNewPath;
    }
};

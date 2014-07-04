/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        launcher/Main.cpp
*  PURPOSE:     Launcher entry
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CDynamicLibrary.h"
#include "../version.h"
#include "../../Shared/sdk/SharedUtil.Defines.h"
#include <cstdio>
#include <iostream>

using namespace std;

#ifdef WIN32
    #include <windows.h>
    #include <direct.h>
    #include "Shlwapi.h"
    #pragma comment(lib, "Shlwapi.lib")
#else
    #include <string.h>
    #include <alloca.h>
#endif

#ifdef WIN32
    #ifdef MTA_DEBUG
        #define LIB_CORE SERVER_BIN_PATH "core_d.dll"
    #else
        #define LIB_CORE SERVER_BIN_PATH "core.dll"
    #endif
#else
    #ifdef MTA_DEBUG
        #define LIB_CORE "./" SERVER_BIN_PATH "core_d.so"
    #else
        #define LIB_CORE "./" SERVER_BIN_PATH "core.so"
    #endif
#endif

int main ( int argc, char* argv [] )
{
    // Work out the launched directory and filename
    int iLength = strlen ( argv[0] );
    char *szLaunchDirectory = static_cast < char* > ( alloca ( iLength + 1 ) );
    char *szLaunchFile = NULL;

    strncpy ( szLaunchDirectory, argv[0], iLength + 1 );

    for ( int i = 0 ; i < iLength ; i++ )
        if ( szLaunchDirectory[i] == '\\' )
            szLaunchDirectory[i] = '/';

    if ( char* cpPos = strrchr ( szLaunchDirectory, '/' ) )
    {
        *cpPos = 0;
        szLaunchFile = cpPos + 1;
    }

    if ( argc > 1 )
    {
        if ( strcmp ( argv[1], "/?" ) == 0 || strcmp ( argv[1], "--help" ) == 0 || strcmp ( argv[1], "-h" ) == 0 )
        {
            printf ( "Usage: %s [OPTION]\n\n", szLaunchFile ? szLaunchFile : "mtaserver" );
            printf ( "  -v                   Shows the program version\n" );
            printf ( "  -s                   Run server in silent mode\n" );
#ifndef WIN32
            printf ( "  -d                   Run server daemonized\n" );
#endif
            printf ( "  -t                   Run server with a simple console\n" );
            printf ( "  -f                   Run server with a standard console (Default)\n" );
            printf ( "  -n                   Disable the usage of ncurses (For screenlog)\n" );
            printf ( "  -D [PATH]            Use as base directory\n" );
            printf ( "  --config [FILE]      Alternate mtaserver.conf file\n" );
            printf ( "  --ip [ADDR]          Set IP address\n" );
            printf ( "  --port [PORT]        Set port\n" );
            printf ( "  --httpport [PORT]    Set http port\n" );
            printf ( "  --maxplayers [max]   Set maxplayers\n" );
            printf ( "  --novoice            Disable voice communication\n" );
            return 1;
        }
        else if ( strcmp ( argv[1], "--version" ) == 0 || strcmp ( argv[1], "-v" ) == 0 )
        {
            printf ( MTA_DM_FULL_STRING " v" MTA_DM_BUILDTAG_LONG "\n" );
            return 1;
        }
    }

    // If we are unable to access the core module, try changing to the directory of the launched file
    FILE* fh = fopen ( LIB_CORE, "r" );
    if ( !fh )
    {
        #ifdef WIN32
            wchar_t szBuffer[64000];
            GetModuleFileNameW( NULL, szBuffer, 64000 );
            PathRemoveFileSpecW ( szBuffer );
            SetCurrentDirectoryW( szBuffer );
        #else
            chdir ( szLaunchDirectory );
        #endif
    }
    else
        fclose ( fh );

    // Load the core library
    CDynamicLibrary Core;
    if ( Core.Load ( LIB_CORE ) )
    {
        // Grab the entrypoint
        typedef int ( Main_t )( int, char* [] );
        Main_t* pfnEntryPoint = reinterpret_cast < Main_t* > ( (long long)(Core.GetProcedureAddress ( "Run" )) );
        if ( pfnEntryPoint )
        {
            // Call it and return what it returns
            return pfnEntryPoint ( argc, argv );
        }
        else
        {
            printf ( "ERROR: Bad file: %s\n", LIB_CORE );
        }
    }
    else
    {
        printf ( "ERROR: Could not load %s\n", LIB_CORE );
        printf ( "* Check installed data files.\n" );
#ifdef WIN32
        printf ( "* Check installed Microsoft Visual C++ 2008 SP1 Redistributable Package (x86).\n" );
#endif
    }

    // Wait for a key then exit
    printf ( "Press enter to continue...\n" );
    cin.get ();
    return 1;
}

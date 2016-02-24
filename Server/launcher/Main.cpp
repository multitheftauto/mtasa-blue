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

/*
    IMPORTANT (Windows only)

    If this project changes, a new release build should be copied into
    the launcher/output diectory.

    The 'MTA Server.exe' in launcher/output will be used by the installer and updater.

    (set flag.new_server_exe on the build server to generate a new 'MTA Server.exe')
*/

#include "CDynamicLibrary.h"
#include <iostream>
#include "MTAPlatform.h"
#include "SharedUtil.h"
#ifdef WIN32
    // Linux gcc 4.4.5 memory corruption on destruction of g_StatEvents (Reason unknown)
    #include "SharedUtil.hpp"
#else
    bool SString::Contains ( const SString& strOther ) const
    {
        return find ( strOther ) != std::string::npos;
    }
    SString SharedUtil::GetSystemCurrentDirectory ( void )
    {
        char szBuffer[ MAX_PATH ];
        getcwd ( szBuffer, MAX_PATH - 1 );
        return szBuffer;
    }
    void HandleLinuxLibs( const SString& strLaunchDirectory, int argc, char* argv [] );
#endif

using namespace std;

#ifdef WIN32
    #ifdef MTA_DEBUG
        #define LIB_CORE SERVER_BIN_PATH "core_d.dll"
        #define LIB_NET SERVER_BIN_PATH "net_d.dll"
    #else
        #define LIB_CORE SERVER_BIN_PATH "core.dll"
        #define LIB_NET SERVER_BIN_PATH "net.dll"
    #endif
#else
    #ifdef MTA_DEBUG
        #define LIB_CORE "./" SERVER_BIN_PATH "core_d.so"
        #define LIB_NET "./" SERVER_BIN_PATH "net_d.so"
    #else
        #define LIB_CORE "./" SERVER_BIN_PATH "core.so"
        #define LIB_NET "./" SERVER_BIN_PATH "net.so"
    #endif
    #ifdef ANY_x86
        #define LINUX_LIBS_PATH     "x86/linux-libs"
    #else
        #define LINUX_LIBS_PATH     "x64/linux-libs"
    #endif
#endif

int main ( int argc, char* argv [] )
{
    // Work out the launched directory and filename
    int iLength = strlen ( argv[0] );
    char *szLaunchDirectory = static_cast < char* > ( alloca ( iLength + 1 ) );

    strncpy ( szLaunchDirectory, argv[0], iLength + 1 );

    for ( int i = 0 ; i < iLength ; i++ )
        if ( szLaunchDirectory[i] == '\\' )
            szLaunchDirectory[i] = '/';

    SString strLaunchFile, strLaunchDirectory;
    if ( char* cpPos = strrchr ( szLaunchDirectory, '/' ) )
    {
        *cpPos = 0;
        strLaunchFile = cpPos + 1;
        strLaunchDirectory = szLaunchDirectory;
    }
    else
    {
        strLaunchFile = szLaunchDirectory;
        strLaunchDirectory = "";
    }

    if ( argc > 1 )
    {
        if ( strcmp ( argv[1], "/?" ) == 0 || strcmp ( argv[1], "--help" ) == 0 || strcmp ( argv[1], "-h" ) == 0 )
        {
            printf ( "Usage: %s [OPTION]\n\n", *strLaunchFile );
            printf ( "  -v                   Shows the program version\n" );
            printf ( "  -s                   Run server in silent mode\n" );
#ifndef WIN32
            printf ( "  -d                   Run server daemonized\n" );
#endif
            printf ( "  -t                   Run server with a simple console\n" );
            printf ( "  -f                   Run server with a standard console (Default)\n" );
            printf ( "  -n                   Disable the usage of ncurses (For screenlog)\n" );
#ifndef WIN32
            printf ( "  -x                   Disable simplified crash reports (To allow core dumps)\n" );
            printf ( "  -q                   Do not add " LINUX_LIBS_PATH " directory to library search path\n" );
#endif
            printf ( "  -D [PATH]            Use as base directory\n" );
            printf ( "  --config [FILE]      Alternate mtaserver.conf file\n" );
            printf ( "  --ip [ADDR]          Set IP address\n" );
            printf ( "  --port [PORT]        Set port\n" );
            printf ( "  --httpport [PORT]    Set http port\n" );
            printf ( "  --maxplayers [max]   Set maxplayers\n" );
            printf ( "  --novoice            Disable voice communication\n" );
            return 1;
        }
    }

#ifdef WIN32
    if ( !IsWindowsXPSP3OrGreater() )
    {
        printf ( "This version of MTA requires Windows XP SP3 or later\n" );

        // Wait for a key then exit
        printf ( "Press enter to continue...\n" );
        cin.get ();
        return 1;
    }
#else
    HandleLinuxLibs( strLaunchDirectory, argc, argv );
#endif

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
            chdir ( strLaunchDirectory );
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
        printf ( "* Check installed Microsoft Visual C++ 2013 Redistributable Package (x86).\n" );
#endif
    }

    // Wait for a key then exit
    printf ( "Press enter to continue...\n" );
    cin.get ();
    return 1;
}


#ifndef WIN32
//
// Add linux-libs to library search path if:
//  1. Options don't forbid it (-q)
//  2. linux-libs is not already in the library search path
//
void HandleLinuxLibs( const SString& strLaunchDirectory, int argc, char* argv [] )
{
    // Check for linux-libs forbidden option
    for ( int i = 1 ; i < argc ; i++ )
    {
        if ( strcmp( argv[i], "-q" ) == 0 )
            return;
    }

    // Calculate absolute path to MTA directory
    SString strSavedDir = GetSystemCurrentDirectory();
    chdir( strLaunchDirectory );
    SString strAbsLaunchDirectory = GetSystemCurrentDirectory();
    chdir( strSavedDir );

    SString strLdLibraryPath = getenv( "LD_LIBRARY_PATH" );
    SString strLinuxLibsPath = strAbsLaunchDirectory + "/" LINUX_LIBS_PATH;

    // Check that linux-libs is not already in library path
    if ( !strLdLibraryPath.Contains( strLinuxLibsPath ) )
    {
        // Add linux-libs to search path
        if ( !strLdLibraryPath.empty() )
            strLdLibraryPath += ";";
        strLdLibraryPath += strLinuxLibsPath;
        SString strEnvString = SStringX( "LD_LIBRARY_PATH=" ) + strLdLibraryPath;
        putenv( (char*)*strEnvString );

        // Add -q to ensure linux-libs don't get added again
        char** pArgArray = new char*[argc + 2];
        for ( int i = 0 ; i <= argc ; i++ )
        {
            pArgArray[i] = argv[i];
        }
        char newArg[] = "-q";
        pArgArray[argc] = newArg;
        pArgArray[argc + 1] = nullptr;

        // Go for launch #2
        execv( argv[0], pArgArray );
    }
}
#endif

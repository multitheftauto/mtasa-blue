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
#include <cstdio>
#include <iostream>

using namespace std;

#ifdef WIN32
    #include <windows.h>
#else
    #include <string.h>
#endif

#ifdef WIN32
    #ifdef MTA_DEBUG
        #define LIB_CORE "core_d.dll"
    #else
        #define LIB_CORE "core.dll"
    #endif
#else
    #ifdef MTA_DEBUG
        #define LIB_CORE "./core_d.so"
    #else
        #define LIB_CORE "./core.so"
    #endif
#endif

int main ( int argc, char* argv [] )
{
    if ( argc == 2 )
    {
        if ( strcmp ( argv[1], "/?" ) == 0 || strcmp ( argv[1], "--help" ) == 0 )
        {
            printf ( "Usage: mtaserver [OPTION]\n\n" );
            printf ( "  -s                   Run server in silent mode\n" );
            printf ( "  -D [PATH]            Use as base directory\n" );
            printf ( "  --config [FILE]      Alternate mtaserver.conf file\n" );
            printf ( "  --ip [ADDR]          Set IP address\n" );
            printf ( "  --port [PORT]        Set port\n" );
            printf ( "  --httpport [PORT]    Set http port\n" );
            printf ( "  --maxplayers [2-128] Set maxplayers\n" );
            return 1;
        }
    }
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

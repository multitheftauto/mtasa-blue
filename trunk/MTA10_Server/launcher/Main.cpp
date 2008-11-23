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
    }

    // Wait for a key then exit
    printf ( "Press enter to continue...\n" );
    cin.get ();
    return 1;
}

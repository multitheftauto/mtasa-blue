/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/Server.cpp
*  PURPOSE:     Server core module entry
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CServerImpl.h"
#define ALLOC_STATS_MODULE_NAME "core"
#define ALLOC_STATS_PRE_COUNT 0     // Increase if crashing at startup
#include "SharedUtil.hpp"
#include "SharedUtil.Tests.hpp"

#if WIN32
    #define MTAEXPORT extern "C" __declspec(dllexport)
#else
    #define MTAEXPORT extern "C"
#endif

#ifdef WIN32
CThreadCommandQueue g_CommandQueue;
#endif

MTAEXPORT int Run ( int iArgumentCount, char* szArguments [] )
{
    SharedUtil_Tests ();

    #ifdef WIN32
        // Disable critical error message boxes
        SetErrorMode ( SEM_FAILCRITICALERRORS );
    #endif

    // Create the server
    #ifdef WIN32
        CServerImpl Server ( &g_CommandQueue );
    #else
        CServerImpl Server;
    #endif

    // Run the main func
    int iReturn;
    do
    {
        iReturn = Server.Run ( iArgumentCount, szArguments );
    }
    while ( iReturn == SERVER_RESET_RETURN );

    // Done
    return iReturn;
}

// Threadsafe way to tell the server to run a command (like the GUI would run)
#ifdef WIN32

MTAEXPORT bool SendServerCommand ( const char* szString )
{
    g_CommandQueue.Add ( szString );
    return true;
}

#endif

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/Server.cpp
 *  PURPOSE:     Server core module entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerImpl.h"
#include "version.h"
#define ALLOC_STATS_MODULE_NAME "core"
#include "SharedUtil.hpp"
#include "ErrorCodes.h"
#ifdef WIN_x86
// TODO - 64 bit file hooks
    #include "SharedUtil.Win32Utf8FileHooks.hpp"
#endif

#ifdef WIN32
CThreadCommandQueue g_CommandQueue;
#endif

MTAEXPORT int Run(int iArgumentCount, char* szArguments[])
{
    if (iArgumentCount > 1)
    {
        if (strcmp(szArguments[1], "--version") == 0 || strcmp(szArguments[1], "-v") == 0)
        {
            printf(MTA_DM_FULL_STRING " v" MTA_DM_BUILDTAG_LONG "\n");
            return 1;
        }
    }

    #ifdef WIN32
    // Disable critical error message boxes
    SetErrorMode(SEM_FAILCRITICALERRORS);
    #endif

    #ifdef WIN_x86
    // Apply file hooks if not already done by the client
    bool bSkipFileHooks = false;
    for (int i = 1; i < iArgumentCount; i++)
        bSkipFileHooks |= SStringX(szArguments[i]).Contains("--clientfeedback");
    if (!bSkipFileHooks)
        AddUtf8FileHooks();
    #endif

        // Create the server
    #ifdef WIN32
    CServerImpl Server(&g_CommandQueue);
    #else
    CServerImpl Server;
    #endif

    // Run the main func
    int iReturn;
    do
    {
        iReturn = Server.Run(iArgumentCount, szArguments);
    } while (iReturn == SERVER_RESET_RETURN);

    // Done
    #ifdef WIN_x86
    RemoveUtf8FileHooks();
    #endif

    // Overwrite the exit code, if we are stopping gracefully without errors
    if (iReturn == ERROR_NO_ERROR)
        iReturn = Server.GetExitCode();

    return iReturn;
}

// Threadsafe way to tell the server to run a command (like the GUI would run)
#ifdef WIN32

MTAEXPORT bool SendServerCommand(const char* szString)
{
    g_CommandQueue.Add(szString);
    return true;
}

#endif

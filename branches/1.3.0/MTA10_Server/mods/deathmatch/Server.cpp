/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/Server.cpp
*  PURPOSE:     Deathmatch module entry point
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.Profiling.hpp"
CStatEvents g_StatEvents;

CServer* g_pServer = NULL;

MTAEXPORT CServerBase* InitServer ( void )
{
    // Create a server and return it
    if ( !g_pServer )
    {
        g_pServer = new CServer;
    }

    return g_pServer;
}


MTAEXPORT void ReleaseServer ( void )
{
    // Destroy the server
    if ( g_pServer )
    {
        delete g_pServer;
        g_pServer = NULL;
    }
}


#ifdef WIN32    /* Win32 DllMain entry */

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    if ( dwReason == DLL_PROCESS_DETACH )
    {
        ReleaseServer ();
    }

    return TRUE;
}

#else           /* POSIX library destructor */

void __attribute__ ((destructor)) ReleaseServer(void);

#endif

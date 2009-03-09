/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/Client.cpp
*  PURPOSE:     Module entry point
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "ClientSharedUtil.hpp"

CClient* g_pClient = NULL;

MTAEXPORT CClientBase* __cdecl InitClient ( void )
{
    // Eventually create a client base interface
    if ( !g_pClient )
    { 
        g_pClient = new CClient;
    }
    return g_pClient;
}

int WINAPI DllMain ( HINSTANCE hModule, DWORD dwReason, PVOID pvNothing )
{
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
        {
            return TRUE;  
        }

        case DLL_PROCESS_DETACH:
        {
            // Make sure the client is destroyed upon destruction
            if ( g_pClient )
            {
                delete g_pClient;
                g_pClient = NULL;
            }

            // Dump any memory leaks we might've had and return
            DumpUnfreed ();
            return TRUE;
        }
    }

    return FALSE;
}

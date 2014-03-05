/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CIPLSectorSA.cpp
*  PURPOSE:     IPL sector management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

/*=========================================================
    _IPLSector_Init

    Purpose:
        Initializes the IPL sector pool and allocates
        a quad tree node used for quick culling.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00405EC0
=========================================================*/
void __cdecl _IPLSector_Init( void )
{
    Streaming::GetIPLEnvironment().Init();
}

void IPLSector_Init( void )
{
    // Install our hooks.
    HookInstall( 0x00405EC0, (DWORD)_IPLSector_Init, 5 );
}

void IPLSector_Shutdown( void )
{
}

// IPL files have to be placed in their pool.
void* CIPLFileSA::operator new ( size_t )
{
    return Streaming::GetIPLEnvironment().m_pool->Allocate();
}

void CIPLFileSA::operator delete ( void *ptr )
{
    Streaming::GetIPLEnvironment().m_pool->Free( (CIPLFileSA*)ptr );
}
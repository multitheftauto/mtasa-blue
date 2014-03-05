/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.ipl.cpp
*  PURPOSE:     IPL sector streaming
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Instantiate single copies of IPL sector manager members.
unsigned int CIPLSectorManagerSA::m_loadAreaId = 0;

/*=========================================================
    _SetIPLLoadPosition

    Arguments:
        pos - position to load IPL sectors around
    Purpose:
        Sets the loading position that is used to request
        IPL sectors. By doing that, the IPL sector does another
        marking by position.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004045B0
=========================================================*/
static void __cdecl _SetIPLLoadPosition( const CVector& pos )
{
    Streaming::GetIPLEnvironment().SetLoadPosition( pos );
}

/*=========================================================
    _StreamIPLSectors

    Arguments:
        requestPos - position to request around
        calcVelocity - weird position modificator
    Purpose:
        Manages all present IPL instances. IPL sectors that
        are pending to be loaded are requested. Unused IPL
        sectors are unloaded. IPL sectors are loaded for all
        mission vehicles and peds.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00405170
=========================================================*/
static void __cdecl _StreamIPLSectors( CVector requestPos, bool calcVelocity )
{
    Streaming::GetIPLEnvironment().StreamSectors( requestPos, calcVelocity );
}

/*=========================================================
    _PrioritizeIPLStreaming

    Arguments:
        requestPos - position to request around
    Purpose:
        Loops through all active IPL sectors and loads
        the ones intersecting with the given position.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004053F0
=========================================================*/
static void __cdecl _PrioritizeIPLStreaming( const CVector& requestPos )
{
    Streaming::GetIPLEnvironment().PrioritizeLocalStreaming( requestPos );
}

void StreamingIPL_Init( void )
{
    // Hook stuff.
    HookInstall( 0x004045B0, (DWORD)_SetIPLLoadPosition, 5 );
    HookInstall( 0x00405170, (DWORD)_StreamIPLSectors, 5 );
    HookInstall( 0x004053F0, (DWORD)_PrioritizeIPLStreaming, 5 );
}

void StreamingIPL_Shutdown( void )
{
}
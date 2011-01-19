/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_init.cpp
*  PURPOSE:     Game initialization interface
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CGameSA* pGame = NULL;
CNet* g_pNet = NULL;

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.

extern "C" _declspec(dllexport)
CGame * GetGameInterface( CCoreInterface* pCore )
{
    DEBUG_TRACE("CGame * GetGameInterface()");

    g_pNet = pCore->GetNetwork ();
    assert ( g_pNet );

    pGame = new CGameSA;

    return (CGame *)pGame;
}

//-----------------------------------------------------------


void MemSet8 ( void* dwDest, int cValue, uint uiAmount )
{
    g_pNet->ResetStub ( 'MSet', dwDest, cValue, uiAmount );
}

void MemCpy8 ( void* dwDest, const void* dwSrc, uint uiAmount )
{
    g_pNet->ResetStub ( 'MCpy', dwDest, dwSrc, uiAmount );
}

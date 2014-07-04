/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayersa_init.cpp
*  PURPOSE:     Multiplayer module entry
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.Misc.h"

CGame* pGameInterface = 0;
CMultiplayerSA* pMultiplayer = 0;
CNet* g_pNet = NULL;

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.

extern "C" _declspec(dllexport)
CMultiplayer* InitMultiplayerInterface(CCoreInterface* pCore)
{   
    // set the internal pointer to the game class
    pGameInterface = pCore->GetGame ();
    g_pNet = pCore->GetNetwork ();
    assert ( pGameInterface );
    assert ( g_pNet );

    // create an instance of our multiplayer class
    pMultiplayer = new CMultiplayerSA;
    pMultiplayer->InitHooks();

    // return the multiplayer class ptr
    return (CMultiplayer*) pMultiplayer;
}

//-----------------------------------------------------------


void MemSet ( void* dwDest, int cValue, uint uiAmount )
{
    memset ( dwDest, cValue, uiAmount );
}

void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount )
{
    memcpy ( dwDest, dwSrc, uiAmount );
}

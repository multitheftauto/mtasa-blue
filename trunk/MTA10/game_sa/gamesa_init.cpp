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

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.

extern "C" _declspec(dllexport)
CGame * GetGameInterface()
{
	DEBUG_TRACE("CGame * GetGameInterface()");
	pGame = new CGameSA;

	return (CGame *)pGame;
}

//-----------------------------------------------------------

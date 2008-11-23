/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayersa_init.h
*  PURPOSE:     Multiplayer module entry
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __MULTIPLAYERSA_INIT
#define __MULTIPLAYERSA_INIT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Common.h"

#include <game/CGame.h>
#include "../game_sa/CGameSA.h"

#include "multiplayer_keysync.h"
#include "multiplayer_shotsync.h"

#include "CMultiplayerSA.h"

extern CGame* pGameInterface;
//extern CMultiplayerSA* pMultiplayer;

/** Buffer overrun trace - attach debugger and watch out for EXCEPTION_GUARD_PAGE (0x80000001) **/
#ifdef IJSIFY
	#pragma message(__LOC__ "YOU HAVE ENABLED THE BOUNDS CHECKER. This may cause performance and/or stability issues!")

	#include <windows.h>
	#include <math.h>

	inline void* __cdecl operator new ( size_t size )
	{
		DWORD dwOld;
		DWORD dwPageSpan = ceil ( size / 4096.0f ) * 4096;

		DWORD dwPage = 0;
		while ( dwPage == NULL ) {
			dwPage = (DWORD)VirtualAlloc ( NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		}
		VirtualProtect ( (LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld );
		dwPage += ( dwPageSpan - size );

		return (LPVOID)dwPage;
	};

	inline void* __cdecl operator new [] ( size_t size )
	{
		DWORD dwOld;
		DWORD dwPageSpan = ceil ( size / 4096.0f ) * 4096;

		DWORD dwPage = 0;
		while ( dwPage == NULL ) {
			dwPage = (DWORD)VirtualAlloc ( NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		}
		VirtualProtect ( (LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld );
		dwPage += ( dwPageSpan - size );

		return (LPVOID)dwPage;
	};

	inline void __cdecl operator delete ( LPVOID pPointer )
	{
		VirtualFree ( pPointer, NULL, MEM_RELEASE );
	};

	inline void __cdecl operator delete [] ( LPVOID pPointer )
	{
		VirtualFree ( pPointer, NULL, MEM_RELEASE );
	};
#endif

#endif
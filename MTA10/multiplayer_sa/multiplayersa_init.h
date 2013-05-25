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


//
// Use MemSet/Cpy/Put for the following memory regions:
//
// 0x401000 - 0x4C02FF
// 0x4C0400 - 0x5022FF
// 0x502400 - 0x50AAFF
// 0x50AC00 - 0x50BEFF
// 0x50C000 - 0x5331FF
// 0x533300 - 0x60D7FF
// 0x60D900 - 0x609BFF
// 0x609D00 - 0x60F1FF
// 0x60F300 - 0x686FFF
// 0x687100 - 0x6AE9FF
// 0x6AEB00 - 0x742AFF
// 0x742C00 - 0x729AFF
// 0x729C00 - 0x8A4000
//

void MemSet ( void* dwDest, int cValue, uint uiAmount );
void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount );

template < class T, class U >
void MemPut ( U ptr, const T value )
{
    if ( *(T*)ptr != value )
        MemCpy ( (void*)ptr, &value, sizeof ( T ) );
}


//
// Use Mem*Fast for the remaining memory regions
//

inline
void MemCpyFast ( void* dwDest, const void* dwSrc, uint uiAmount )
{
    memcpy ( dwDest, dwSrc, uiAmount );
}

inline
void MemSetFast ( void* dwDest, int cValue, uint uiAmount )
{
    memset ( dwDest, cValue, uiAmount );
}

template < class T, class U >
void MemPutFast ( U ptr, const T value )
{
    *(T*)ptr = value;
}

template < class T, class U >
void MemSubFast ( U ptr, const T value )
{
    *(T*)ptr -= value;
}

bool GetDebugIdEnabled ( uint uiDebugId );
void LogEvent ( uint uiDebugId, const char* szType, const char* szContext, const char* szBody );
void CallGameEntityRenderHandler( CEntitySAInterface* pEntity );
extern GameEntityRenderHandler* pGameEntityRenderHandler;

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_init.h
*  PURPOSE:     Global game module header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CGameSA.h"
#include <multiplayer/CMultiplayer.h>
#include <core/CCoreInterface.h>

#ifndef __CGAMESA_INIT
#define __CGAMESA_INIT

extern CGameSA * pGame;
extern CCoreInterface * g_pCore;

//
// Use MemSet/Cpy/Put/Add for the following memory regions:
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

template < class T, class U >
void MemAdd ( U ptr, const T value )
{
    T current;
    memcpy ( &current, (void*)ptr, sizeof ( T ) );
    current += value;
    MemCpy ( (void*)ptr, &current, sizeof ( T ) );
}


//
// Use Mem*Fast for everything else
//
inline void MemCpyFast ( void* dwDest, const void* dwSrc, uint uiAmount )
{
    memcpy ( dwDest, dwSrc, uiAmount );
}

inline void MemSetFast ( void* dwDest, int cValue, uint uiAmount )
{
    memset ( dwDest, cValue, uiAmount );
}

template < class T, class U >
void MemPutFast ( U ptr, const T value )
{
    *(T*)ptr = value;
}

template < class T, class U >
void MemAddFast ( U ptr, const T value )
{
    *(T*)ptr += value;
}

template < class T, class U >
void MemSubFast ( U ptr, const T value )
{
    *(T*)ptr -= value;
}

template < class T, class U >
void MemAndFast ( U ptr, const T value )
{
    *(T*)ptr &= value;
}

template < class T, class U >
void MemOrFast ( U ptr, const T value )
{
    *(T*)ptr |= value;
}

bool GetDebugIdEnabled ( uint uiDebugId );
void LogEvent ( uint uiDebugId, const char* szType, const char* szContext, const char* szBody );

#endif

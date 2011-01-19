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

#ifndef __CGAMESA_INIT
#define __CGAMESA_INIT

extern CGameSA * pGame;


void MemSet8 ( void* dwDest, int cValue, uint uiAmount );
void MemCpy8 ( void* dwDest, const void* dwSrc, uint uiAmount );

template < class T, class U >
void MemPut ( U ptr, const T value )
{
    MemCpy8 ( (void*)ptr, &value, sizeof ( T ) );
}

template < class T, class U >
void MemAdd ( U ptr, const T value )
{
    T current;
    MemCpy8 ( &current, (void*)ptr, sizeof ( T ) );
    current += value;
    MemCpy8 ( (void*)ptr, &current, sizeof ( T ) );
}

template < class T, class U >
void MemSub ( U ptr, const T value )
{
    T current;
    MemCpy8 ( &current, (void*)ptr, sizeof ( T ) );
    current -= value;
    MemCpy8 ( (void*)ptr, &current, sizeof ( T ) );
}

template < class T, class U >
void MemAnd ( U ptr, const T value )
{
    T current;
    MemCpy8 ( &current, (void*)ptr, sizeof ( T ) );
    current &= value;
    MemCpy8 ( (void*)ptr, &current, sizeof ( T ) );
}

template < class T, class U >
void MemOr ( U ptr, const T value )
{
    T current;
    MemCpy8 ( &current, (void*)ptr, sizeof ( T ) );
    current |= value;
    MemCpy8 ( (void*)ptr, &current, sizeof ( T ) );
}

#endif
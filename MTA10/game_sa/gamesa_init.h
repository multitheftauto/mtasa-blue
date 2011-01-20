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


void MemSet ( void* dwDest, int cValue, uint uiAmount );
void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount );

template < class T, class U >
void MemPut ( U ptr, const T value )
{
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

template < class T, class U >
void MemSub ( U ptr, const T value )
{
    T current;
    memcpy ( &current, (void*)ptr, sizeof ( T ) );
    current -= value;
    MemCpy ( (void*)ptr, &current, sizeof ( T ) );
}

template < class T, class U >
void MemAnd ( U ptr, const T value )
{
    T current;
    memcpy ( &current, (void*)ptr, sizeof ( T ) );
    current &= value;
    MemCpy ( (void*)ptr, &current, sizeof ( T ) );
}

template < class T, class U >
void MemOr ( U ptr, const T value )
{
    T current;
    memcpy ( &current, (void*)ptr, sizeof ( T ) );
    current |= value;
    MemCpy ( (void*)ptr, &current, sizeof ( T ) );
}


//
// http://msdn.microsoft.com/en-us/library/4d12973a.aspx
// Prolog/epilog should be inserted in _declspec(naked) functions where locals may be used.
//
#define _FUNCTION_PROLOG \
        _asm push    ebp \
        _asm mov     ebp, esp \
        _asm sub     esp, __LOCAL_SIZE \

#define FUNCTION_PROLOG \
    _asm \
    { \
        _FUNCTION_PROLOG \
    }

#define _FUNCTION_EPILOG \
        _asm mov     esp, ebp \
        _asm pop     ebp \

#define FUNCTION_EPILOG \
    _asm \
    { \
        _FUNCTION_EPILOG \
    }

#endif
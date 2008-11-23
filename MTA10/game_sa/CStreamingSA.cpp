/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cpp
*  PURPOSE:     Data streamer
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CStreamingSA::RequestAnimations ( int i, int j )
{
    DWORD dwFunc = FUNC_CStreaming_RequestAnimations;
    _asm
    {
        push    j
        push    i
        call    dwFunc
        add     esp, 0x8
    }
}
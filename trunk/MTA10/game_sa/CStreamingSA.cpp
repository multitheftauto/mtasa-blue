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

void CStreamingSA::RequestAnimations ( int iBlockIndex )
{
    DWORD dwFunc = FUNC_CStreaming_RequestAnimations;
    _asm
    {
        push    8
        push    iBlockIndex
        call    dwFunc
        add     esp, 0x8
    }
}


void CStreamingSA::RemoveAnimations ( int iBlockIndex )
{
    DWORD dwFunc = FUNC_CStreaming_RemoveAnimations;
    _asm
    {
        push    iBlockIndex
        call    dwFunc
        add     esp, 0x4
    }
}
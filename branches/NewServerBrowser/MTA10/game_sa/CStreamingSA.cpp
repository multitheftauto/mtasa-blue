/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cpp
*  PURPOSE:     Data streamer
*  DEVELOPERS:  Jax <>
*               jenksta <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CStreamingSA::RequestModel( DWORD dwModelID, DWORD dwFlags )
{
    DWORD dwFunction = FUNC_CStreaming__RequestModel;
    _asm
    {
        push    dwFlags
        push    dwModelID
        call    dwFunction
        add     esp, 8
    }
}

void CStreamingSA::LoadAllRequestedModels ( BOOL bOnlyPriorityModels )
{
    DWORD dwFunction = FUNC_LoadAllRequestedModels;
    DWORD dwOnlyPriorityModels = bOnlyPriorityModels;
    _asm
    {
        push    dwOnlyPriorityModels
        call    dwFunction
        add     esp, 4
    }
}

BOOL CStreamingSA::HasModelLoaded ( DWORD dwModelID )
{
    DWORD dwFunc = FUNC_CStreaming__HasModelLoaded;

    BOOL bReturn = 0;
    _asm
    {
        push    dwModelID
        call    dwFunc
        movzx   eax, al
        mov     bReturn, eax
        pop     eax
    }

    return bReturn;
}

void CStreamingSA::RequestAnimations ( int iAnimationLibraryBlock, DWORD dwFlags )
{
    iAnimationLibraryBlock += 25575;
    RequestModel( iAnimationLibraryBlock, dwFlags );
}

BOOL CStreamingSA::HaveAnimationsLoaded ( int iAnimationLibraryBlock )
{
    iAnimationLibraryBlock += 25575;
    return HasModelLoaded( iAnimationLibraryBlock );
}

bool CStreamingSA::HasVehicleUpgradeLoaded ( int model )
{
    bool bReturn;
    DWORD dwFunc = FUNC_CStreaming__HasVehicleUpgradeLoaded;
    _asm
    {
        push    model
        call    dwFunc
        add     esp, 0x4
        mov     bReturn, al
    }
    return bReturn;
}

void CStreamingSA::RequestSpecialModel ( DWORD model, const char * szTexture, DWORD channel )
{
    DWORD dwFunc = FUNC_CStreaming_RequestSpecialModel;
    _asm
    {
        push    channel
        push    szTexture
        push    model
        call    dwFunc
        add     esp, 0xC
    }
}
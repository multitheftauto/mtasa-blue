/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVisibilityPluginsSA.cpp
*  PURPOSE:     RenderWare entity visibility plugin
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CVisibilityPluginsSA::SetClumpAlpha ( RpClump * pClump, int iAlpha )
{
    DWORD dwFunc = FUNC_CVisiblityPlugins_SetClumpAlpha;
    _asm
    {
        push    iAlpha
        push    pClump
        call    dwFunc
        add     esp, 0x8
    }
}

// Some AtomicId bits are:
//      0x0001 - Has two versions, and this is the undamaged one
//      0x0002 - Has two versions, and this is the damaged one
//
// Some AtomicId bits might be:
//      0x0004 - Left door?
//      0x0008 - Right door? (or mirrored?)
//      0x0010 - Front part?
//      0x0020 - Rear part?
//      0x0040 - Has alpha?
//      0x0080 - Some door flag?
//      0x0100 - Front door flag?
//      0x0200 - Rear door flag?
//      0x8000 - Some door flag?
int CVisibilityPluginsSA::GetAtomicId ( RwObject * pAtomic )
{
    DWORD dwFunc = FUNC_CVisibilityPlugins_GetAtomicId;
    int iResult = 0;
    _asm
    {
        push    pAtomic
        call    dwFunc
        add     esp, 0x4
        mov     iResult, eax
    }
    return iResult;
}

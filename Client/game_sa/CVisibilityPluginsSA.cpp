/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVisibilityPluginsSA.cpp
 *  PURPOSE:     RenderWare entity visibility plugin
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CVisibilityPluginsSA::SetClumpAlpha(RpClump* pClump, int iAlpha)
{
    // CVisibilityPlugins::SetClumpAlpha
    ((void(__cdecl*)(RpClump*, int))FUNC_CVisiblityPlugins_SetClumpAlpha)(pClump, iAlpha);
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
int CVisibilityPluginsSA::GetAtomicId(RwObject* pAtomic)
{
    // CVisibilityPlugins::GetAtomicId
    return ((int(__cdecl*)(RwObject*))FUNC_CVisibilityPlugins_GetAtomicId)(pAtomic);
}

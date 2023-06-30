/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAnimBlendStaticAssociationSA.cpp
 *  PURPOSE:     Animation blend static association
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendStaticAssociationSA.h"

void CAnimBlendStaticAssociationSA::Initialize(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimBlendHierarchyInterface)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4CEC20;
    _asm
    {
        push    pAnimBlendHierarchyInterface
        push    pClump
        mov     ecx, dwThis
        call    dwFunc
    }
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendStaticAssociationSA.cpp
 *  PURPOSE:     Animation blend static association
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendStaticAssociationSA.h"

void CAnimBlendStaticAssociationSA::Initialize(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimBlendHierarchyInterface)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendStaticAssociation_Initialize;
    _asm
    {
        push    pAnimBlendHierarchyInterface
        push    pClump
        mov     ecx, dwThis
        call    dwFunc
    }
}

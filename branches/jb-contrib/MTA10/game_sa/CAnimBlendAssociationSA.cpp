/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlendAssociationSA.cpp
*  PURPOSE:     Animation blend association
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;

CAnimBlendHierarchy * CAnimBlendAssociationSA::GetAnimHierarchy ( void )
{
    return pGame->GetAnimManager ()->GetAnimBlendHierarchy ( m_pInterface->pAnimHierarchy );
}

void CAnimBlendAssociationSA::SetCurrentProgress ( float fProgress )
{
    float fTime = m_pInterface->pAnimHierarchy->fTotalTime * fProgress;

    DWORD DwFunc = 0x4CEA80;
    DWORD DwThisInterface = reinterpret_cast<DWORD> (m_pInterface);
    _asm
    {
        mov     ecx, DwThisInterface
        push    fTime
        call    DwFunc
    };
}

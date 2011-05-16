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

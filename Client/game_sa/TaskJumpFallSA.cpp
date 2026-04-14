/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskJumpFallSA.cpp
 *  PURPOSE:     Jump and fall game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskJumpFallSA.h"

CTaskSimpleClimbSA::CTaskSimpleClimbSA(CEntitySAInterface* pClimbEnt, const CVector& vecTarget, float fHeading, unsigned char nSurfaceType,
                                       eClimbHeights nHeight, const bool bForceClimb)
{
    CreateTaskInterface(sizeof(CTaskSimpleClimbSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleClimb__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb);
    // clang-format on
}

// ##############################################################################
// ## Name:    CTaskSimpleJetPack
// ## Purpose: Allows the player to use a jetpack to 'fly' around
// ##############################################################################

CTaskSimpleJetPackSA::CTaskSimpleJetPackSA(const CVector* pVecTargetPos, float fCruiseHeight, int nHoverTime)
{
    CreateTaskInterface(sizeof(CTaskSimpleJetPackSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleJetPack__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, pVecTargetPos, fCruiseHeight, nHoverTime, 0);
    // clang-format on
}

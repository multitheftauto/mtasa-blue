/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskJumpFallSA.cpp
 *  PURPOSE:     Jump and fall game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CTaskSimpleClimbSA::CTaskSimpleClimbSA(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight,
                                       const bool bForceClimb)
{
    DEBUG_TRACE(
        "CTaskSimpleClimbSA::CTaskSimpleClimbSA ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, "
        "const bool bForceClimb )");

    this->CreateTaskInterface(sizeof(CTaskSimpleClimbSAInterface));
    if (!IsValid())
        return;
 
    // CTaskSimpleClimb::CTaskSimpleClimb
    ((CTaskSAInterface*(__thiscall*)(CTaskSAInterface*, CEntity*, const CVector&, float, unsigned char, char, const bool))
         FUNC_CTaskSimpleClimb__Constructor)(this->GetInterface(), pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb);
}

// ##############################################################################
// ## Name:    CTaskSimpleJetPack
// ## Purpose: Allows the player to use a jetpack to 'fly' around
// ##############################################################################

CTaskSimpleJetPackSA::CTaskSimpleJetPackSA(const CVector* pVecTargetPos, float fCruiseHeight, int nHoverTime)
{
    DEBUG_TRACE("CTaskSimpleJetPackSA::CTaskSimpleJetPackSA(const CVector *pVecTargetPos = NULL, float fCruiseHeight = 10.0f, int nHoverTime = 0)");
    this->CreateTaskInterface(sizeof(CTaskSimpleJetPackSAInterface));
    if (!IsValid())
        return;

    // CTaskSimpleJetPack::CTaskSimpleJetPack
    ((CTaskSAInterface*(__thiscall*)(CTaskSAInterface*, const CVector*, float, int, CEntitySAInterface*))FUNC_CTaskSimpleJetPack__Constructor)(
        this->GetInterface(), pVecTargetPos, fCruiseHeight, nHoverTime, nullptr);
}

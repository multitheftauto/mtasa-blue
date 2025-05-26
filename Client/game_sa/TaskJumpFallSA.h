/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskJumpFallSA.h
 *  PURPOSE:     Jump and fall game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/TaskJumpFall.h>
#include <game/CTasks.h>
#include "TaskSA.h"

// temporary
class CAnimBlendAssociation;
class FxSystem_c;

#define FUNC_CTaskSimpleClimb__Constructor                          0x67A110
#define FUNC_CTaskSimpleJetPack__Constructor                        0x67B4E0

class CTaskSimpleClimbSAInterface : public CTaskSimpleSAInterface
{
public:
    bool          m_bIsFinished;
    bool          m_bChangeAnimation;
    bool          m_bChangePosition;
    bool          m_bForceClimb;
    bool          m_bInvalidClimb;
    char          m_nHeightForAnim;
    char          m_nHeightForPos;
    unsigned char m_nSurfaceType;
    char          m_nFallAfterVault;
    float         m_fHandholdHeading;
    CVector       m_vecHandholdPos;
    CEntity*      m_pClimbEnt;

    short                  m_nGetToPosCounter;
    CAnimBlendAssociation* m_pAnim;
};

class CTaskSimpleClimbSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleClimb
{
public:
    CTaskSimpleClimbSA(){};
    CTaskSimpleClimbSA(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight = CLIMB_GRAB,
                       const bool bForceClimb = false);
};

// ##############################################################################
// ## Name:    CTaskSimpleJetPack
// ## Purpose: Allows the player to use a jetpack to 'fly' around
// ##############################################################################

class CTaskSimpleJetPackSAInterface : public CTaskSimpleSAInterface
{
public:
    bool          m_bIsFinished;
    unsigned char m_bAddedIdleAnim;
    unsigned char m_bAnimsReferenced;
    unsigned char m_bAttackButtonPressed;
    unsigned char m_bSwitchedWeapons;

    char  m_nThrustStop;
    char  m_nThrustFwd;
    float m_fThrustStrafe;
    float m_fThrustAngle;

    float m_fLegSwingFwd;
    float m_fLegSwingSide;
    float m_fLegTwist;

    float m_fLegSwingFwdSpeed;
    float m_fLegSwingSideSpeed;
    float m_fLegTwistSpeed;

    CVector m_vecOldSpeed;
    float   m_fOldHeading;

    RpClump*               m_pJetPackClump;
    CAnimBlendAssociation* m_pAnim;

    CVector      m_vecTargetPos;
    float        m_fCruiseHeight;
    int          m_nHoverTime;
    unsigned int m_nStartHover;
    CEntity*     m_pTargetEnt;

    FxSystem_c* m_pFxSysL;
    FxSystem_c* m_pFxSysR;
    float       m_fxKeyTime;
};

class CTaskSimpleJetPackSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleJetPack
{
public:
    CTaskSimpleJetPackSA(){};
    CTaskSimpleJetPackSA(const CVector* pVecTargetPos, float fCruiseHeight = 10.0f, int nHoverTime = 0);

    bool IsFinished() const override { return static_cast<const CTaskSimpleJetPackSAInterface*>(GetInterface())->m_bIsFinished; }
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/TaskAttackSA.h
 *  PURPOSE:     Attack game tasks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector2D.h>
#include <game/TaskAttack.h>
#include "TaskSA.h"

class CWeaponInfo;

// temporary
class CAnimBlendAssociation;
class CAnimBlendHierarchy;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

///////////////////////
// Do a gang driveby
///////////////////////

class CTaskSimpleGangDriveBySAInterface : public CTaskSimpleSAInterface
{
public:
    bool m_bIsFinished;
    bool m_bAnimsReferenced;
    bool m_bSeatRHS;
    bool m_bInRangeToShoot;
    bool m_bInWeaponRange;
    bool m_bReachedAbortRange;
    bool m_bFromScriptCommand;

    char  m_nNextCommand;
    char  m_nLastCommand;            // active command
    char  m_nBurstShots;
    char  m_nDrivebyStyle;            // what type of driveby are we doing
    char  m_nFrequencyPercentage;
    char  m_nFakeShootDirn;
    short m_nAttackTimer;

    uint m_nLOSCheckTime;
    bool m_nLOSBlocked;

    float m_fAbortRange;            // range from target at which this task will be aborted

    AnimationId            m_nRequiredAnim;
    AssocGroupId           m_nRequiredAnimGroup;
    CAnimBlendAssociation* m_pAnim;
    CWeaponInfo*           m_pWeaponInfo;

    CEntity* m_pTargetEntity;
    CVector  m_vecCoords;
};

class CTaskSimpleGangDriveBySA : public virtual CTaskSimpleSA, public virtual CTaskSimpleGangDriveBy
{
public:
    CTaskSimpleGangDriveBySA(){};
    CTaskSimpleGangDriveBySA(CEntity* pTargetEntity, const CVector* pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS);
};

class CTaskSimpleUseGunSAInterface : public CTaskSimpleSAInterface
{
public:
    uchar m_bIsFinished;        // 0x08
    uchar m_bIsInControl;
    uchar m_bMoveControl;
    uchar m_bFiredGun;
    uchar m_bBlockedLOS;        // 0x0C
    uchar m_nFireGunThisFrame;
    uchar m_bSkipAim;           // 0x0E

    char      m_nNextCommand;
    char      m_nLastCommand;   // Active command       (2 or 3) == is firing
    CVector2D m_vecMoveCommand;

    CEntity* m_pTargetEntity;
    CVector  m_vecCoords;

    CAnimBlendAssociation* m_pAnim;

    CWeaponInfo* m_pWeaponInfo;             // 0x30
    short        m_nBurstLength;            // 0x34
    short        m_nBurstShots;

    uchar m_nCountDownFrames;
    uchar m_armIkInUse;
    uchar m_lookIkInUse;

    uchar m_bAimImmediate;
};

class CTaskSimpleUseGunSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleUseGun
{
public:
    CTaskSimpleUseGunSA(){};
    CTaskSimpleUseGunSA(CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength = 1, unsigned char bAimImmediate = false);

    bool ControlGun(CPed* pPed, CEntity* pTargetEntity, char nCommand);
    bool ControlGunMove(CVector2D* pMoveVec);
    void Reset(CPed* pPed, CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength = 1);

    int         GetTaskType();
    bool        MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent);
    bool        ProcessPed(CPed* pPed);
    bool        SetPedPosition(CPed* pPed);
    void        FireGun(CPed* pPed, bool bFlag);
    void        AbortIK(CPed* pPed);
    void        AimGun(CPed* pPed);
    void        ClearAnim(CPed* pPed);
    signed char GetCurrentCommand();
    bool        GetDoneFiring();
    bool        GetIsFinished();
    bool        IsLineOfSightBlocked();
    bool        GetIsFiring();
    bool        GetIsReloading();
    bool        GetSkipAim();
    bool        PlayerPassiveControlGun();
    void        RemoveStanceAnims(CPed* pPed, float);
    static bool RequirePistolWhip(CPed* pPed, CEntity*);
    void        SetBurstLength(short);
    void        SetMoveAnim(CPed* pPed);
    void        StartAnim(CPed* pPed);
    void        StartCountDown(unsigned char, bool);
};

class CTaskSimpleFightSAInterface : public CTaskSimpleSAInterface
{
public:
    bool m_bIsFinished;
    bool m_bIsInControl;

    bool         m_bAnimsReferenced;
    AssocGroupId m_nRequiredAnimGroup;

    unsigned short m_nIdlePeriod;
    unsigned short m_nIdleCounter;
    char           m_nContinueStrike;
    char           m_nChainCounter;

    CEntity*               m_pTargetEntity;
    CAnimBlendAssociation* m_pAnim;
    CAnimBlendAssociation* m_pIdleAnim;

    char m_nComboSet;
    char m_nCurrentMove;
    char m_nNextCommand;
    char m_nLastCommand;
};

class CTaskSimpleFightSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleFight
{
public:
    CTaskSimpleFightSA(){};
    CTaskSimpleFightSA(CEntity* pTargetEntity, int nCommand, unsigned int nIdlePeriod = 10000);
};

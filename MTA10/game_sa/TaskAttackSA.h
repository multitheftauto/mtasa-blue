/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskAttackSA.h
*  PURPOSE:     Attack game tasks
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKATTACK
#define __CGAMESA_TASKATTACK

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <game/TaskAttack.h>

#include "TaskSA.h"

#define FUNC_CTaskSimpleGangDriveBy__Constructor        0x6217d0
#define FUNC_CTaskSimpleUseGun__Constructor             0x61de60
#define FUNC_CTaskSimpleUseGun_ControlGun               0x61e040
#define FUNC_CTaskSimpleUseGun_ControlGunMove           0x61e0c0
#define FUNC_CTaskSimpleUseGun_Reset                    0x624dc0
#define FUNC_CTaskSimpleFight__Constructor              0x61c470

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

    char m_nNextCommand;
    char m_nLastCommand;    // active command
    char m_nBurstShots;
    char m_nDrivebyStyle;       // what type of driveby are we doing
    char m_nFrequencyPercentage;
    char m_nFakeShootDirn;
    short m_nAttackTimer;
    
    unsigned int m_nLOSCheckTime;
    bool m_nLOSBlocked;
    
    float m_fAbortRange;        // range from target at which this task will be aborted

    AnimationId m_nRequiredAnim;
    AssocGroupId m_nRequiredAnimGroup;
    CAnimBlendAssociation *m_pAnim;
    CWeaponInfo *m_pWeaponInfo;

    CEntity *m_pTargetEntity;
    CVector m_vecCoords;
};

class CTaskSimpleGangDriveBySA : public virtual CTaskSimpleSA, public virtual CTaskSimpleGangDriveBy
{
public:
    CTaskSimpleGangDriveBySA ( void ) {};
    CTaskSimpleGangDriveBySA ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS );
};

class CTaskSimpleUseGunSAInterface : public CTaskSimpleSAInterface
{
public:         
    bool m_bIsFinished;
    bool m_bIsInControl;
    bool m_bMoveControl;
    bool m_bFiredGun;
    bool m_bBlockedLOS;
    unsigned char m_nFireGunThisFrame;
    unsigned char m_bSkipAim;

    char m_nNextCommand;
    char m_nLastCommand;    // active command
    CVector2D m_vecMoveCommand;
    
    CEntity *m_pTargetEntity;
    CVector m_vecCoords;

    CAnimBlendAssociation *m_pAnim;
    
    CWeaponInfo *m_pWeaponInfo;
    short m_nBurstLength;
    short m_nBurstShots;

    unsigned char m_nCountDownFrames;
    unsigned char m_armIkInUse;
    unsigned char m_lookIkInUse;
    
    unsigned char m_bAimImmediate;
};

class CTaskSimpleUseGunSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleUseGun
{
public:
                CTaskSimpleUseGunSA ( void ) {};
                CTaskSimpleUseGunSA ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength=1, unsigned char bAimImmediate=false );

    bool        ControlGun          ( CPed* pPed, CEntity *pTargetEntity, char nCommand );
    bool        ControlGunMove      ( CVector2D * pMoveVec );
    void        Reset               ( CPed *pPed, CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength=1 );
};

class CTaskSimpleFightSAInterface : public CTaskSimpleSAInterface
{
public:
    bool m_bIsFinished;
    bool m_bIsInControl;

    bool m_bAnimsReferenced;
    AssocGroupId m_nRequiredAnimGroup;

    unsigned short m_nIdlePeriod;
    unsigned short m_nIdleCounter;
    char m_nContinueStrike;
    char m_nChainCounter;
    
    CEntity *m_pTargetEntity;
    CAnimBlendAssociation *m_pAnim;
    CAnimBlendAssociation *m_pIdleAnim;
    
    char m_nComboSet;
    char m_nCurrentMove;
    char m_nNextCommand;
    char m_nLastCommand;
};

class CTaskSimpleFightSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleFight
{
public:
    CTaskSimpleFightSA ( void ) {};
    CTaskSimpleFightSA ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod = 10000 );
};

#endif

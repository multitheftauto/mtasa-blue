/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskBasicSA.h
 *  PURPOSE:     Basic game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CPed.h>
#include <game/TaskBasic.h>
#include "TaskSA.h"

class CVehicleSAInterface;
class CObjectSAInterface;

#define FUNC_CTaskComplexUseMobilePhone__Constructor        0x6348A0
#define FUNC_CTaskSimpleRunAnim__Constructor                0x61A900
#define FUNC_CTaskSimpleRunNamedAnim__Constructor           0x61A990
#define FUNC_CTaskComplexDie__Constructor                   0x630040
#define FUNC_CTaskSimpleStealthKill__Constructor            0x6225F0
#define FUNC_CTaskSimpleDead__Constructor                   0x630590
#define FUNC_CTaskSimpleBeHit__Constructor                  0x620780
#define FUNC_CTaskComplexSunbathe__Constructor              0x631F80
#define FUNC_CTASKSimplePlayerOnFoot__Constructor           0x685750
#define FUNC_CTASKComplexFacial__Constructor                0x690D20
#define VTBL_CTaskSimpleCarFallOut                          0x86EFD0

///////////////////////
// Use a mobile phone
///////////////////////

class CTaskComplexUseMobilePhoneSAInterface : public CTaskComplexSAInterface
{
public:
    int        m_iDuration;
    CTaskTimer m_timer;
    bool       m_bIsAborting;
    bool       m_bQuit;
};

class CTaskComplexUseMobilePhoneSA : public virtual CTaskComplexSA, public virtual CTaskComplexUseMobilePhone
{
public:
    CTaskComplexUseMobilePhoneSA(){};
    CTaskComplexUseMobilePhoneSA(const int iDuration);            // Default is -1
};

// temporary
class CAnimBlendAssociation;
class CAnimBlendHierarchy;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

class CTaskSimpleAnimSAInterface : public CTaskSimpleSAInterface
{
public:
    CAnimBlendAssociation* m_pAnim;
    char                   m_bIsFinished : 1;
    char                   m_bDontInterrupt : 1;
    char                   m_bHoldLastFrame : 1;

    // These flags are used in CTaskSimpleRunAnim only
    char m_bDontBlendOut : 1;

    // These flags are used in  CTaskSimpleRunNamedAnim only
    char m_bRunInSequence : 1;
    char m_bOffsetAtEnd : 1;
    char m_bOffsetAvailable : 1;
};

class CTaskSimpleAnimSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleAnim
{
public:
    CTaskSimpleAnimSA(){};
};

class CTaskSimpleRunAnimSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleRunAnim
{
public:
    CTaskSimpleRunAnimSA(){};
    CTaskSimpleRunAnimSA(const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType, const char* pTaskName,
                         const bool bHoldLastFrame = false);
};

#define ANIM_NAMELEN 24
#define ANIMBLOCK_NAMELEN 16

class CTaskSimpleRunNamedAnimSAInterface : public CTaskSimpleAnimSAInterface
{
public:
    char m_animName[ANIM_NAMELEN];
    char m_animGroupName[ANIMBLOCK_NAMELEN];

    float                m_fBlendDelta;
    CAnimBlendHierarchy* m_pAnimHierarchy;
    int                  m_iTime;
    CTaskTimer           m_timer;
    CVector              m_offsetAtEnd;
    int                  m_flags;
    short                m_animID;
};

class CTaskSimpleRunNamedAnimSA : public virtual CTaskSimpleAnimSA, public virtual CTaskSimpleRunNamedAnim
{
public:
    CTaskSimpleRunNamedAnimSA(){};
    CTaskSimpleRunNamedAnimSA(const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime = -1,
                              const bool bDontInterrupt = false, const bool bRunInSequence = false, const bool bOffsetPed = false,
                              const bool bHoldLastFrame = false);

    CTaskSimpleRunNamedAnimSAInterface*       GetAnimationInterface() noexcept;
    const CTaskSimpleRunNamedAnimSAInterface* GetAnimationInterface() const noexcept;

    const char* GetAnimName() const noexcept override;
    const char* GetGroupName() const noexcept override;
};

class CTaskComplexDieSAInterface : public CTaskComplexSAInterface
{
public:
};

class CTaskComplexDieSA : public virtual CTaskComplexSA, public virtual CTaskComplexDie
{
public:
    CTaskComplexDieSA(){};
    CTaskComplexDieSA(const eWeaponType eMeansOfDeath /*=WEAPONTYPE_UNARMED*/, const AssocGroupId animGroup = 0 /*ANIM_STD_PED*/,
                      const AnimationId anim = 0 /*ANIM_STD_KO_FRONT*/, const float fBlendDelta = 4.0f, const float fAnimSpeed = 0.0f,
                      const bool bBeingKilledByStealth = false, const bool bFallingToDeath = false, const int iFallToDeathDir = 0,
                      const bool bFallToDeathOverRailing = false);
};

class CTaskSimpleStealthKillInterface : public CTaskSimpleSAInterface
{
public:
};

class CTaskSimpleStealthKillSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleStealthKill
{
public:
    CTaskSimpleStealthKillSA(){};
    CTaskSimpleStealthKillSA(bool bKiller, class CPed* pPed, const AssocGroupId animGroup);
};

class CTaskSimpleDeadSAInterface : public CTaskSimpleSAInterface
{
public:
    unsigned int uiDeathTimeMS;
    bool         bUnk2;
};

class CTaskSimpleDeadSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleDead
{
public:
    CTaskSimpleDeadSA(){};
    CTaskSimpleDeadSA(unsigned int uiDeathTimeMS, bool bUnk2);
};

class CTaskSimpleBeHitSAInterface : public CTaskSimpleSAInterface
{
public:
    CPed* pPedAttacker;            // 0x08
    uchar a;                       // 0x0c
    uchar b;                       // 0x0d
    uchar c;                       // 0x0e
    uchar d;                       // 0x0f
    uint  e;                       // 0x10   inited with 0x000000BF
    uint  f;                       // 0x14   inited with 0
    uint  hitBodyPart;             // 0x18
    uint  weaponType;              // 0x1C
    uint  hitBodySide;             // 0x20
    uint  g;                       // 0x24   inited with 0
};

class CTaskSimpleBeHitSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleBeHit
{
public:
    CTaskSimpleBeHitSA(){};
    CTaskSimpleBeHitSA(CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId);
};

class CTaskSimpleCarFallOutSAInterface : public CTaskSimpleSAInterface
{
public:
    uint                 a;                    // 0x08
    uint                 b;                    // 0x0c
    CVehicleSAInterface* pVehicle;             // 0x10
    uint                 doorIndex;            // 0x14
};

class CAnimBlock;

class CTaskComplexSunbatheSAInterface : public CTaskComplexSAInterface
{
public:
    bool                m_bStartStanding;
    bool                m_bBathing;
    bool                m_bBeachAnimsReferenced;
    bool                m_bSunbatheAnimsReferenced;
    bool                m_bAborted;
    CTaskTimer          m_BathingTimer;
    eSunbatherType      m_SunbatherType;
    CAnimBlock*         m_pBeachAnimBlock;
    CAnimBlock*         m_pSunbatheAnimBlock;
    int                 m_BeachAnimBlockIndex;
    int                 m_SunbatheAnimBlockIndex;
    CObjectSAInterface* m_pTowel;
};

class CTaskComplexSunbatheSA : public virtual CTaskComplexSA, public virtual CTaskComplexSunbathe
{
public:
    CTaskComplexSunbatheSA(){};
    CTaskComplexSunbatheSA(class CObject* pTowel, const bool bStartStanding);

    void SetEndTime(DWORD dwTime);
};

////////////////////
// Player on foot //
////////////////////
class CTaskSimplePlayerOnFootSAInterface : public CTaskSimpleSAInterface
{
public:
    BYTE m_Pad[28];
};

class CTaskSimplePlayerOnFootSA : public virtual CTaskSimpleSA, public virtual CTaskSimplePlayerOnFoot
{
public:
    CTaskSimplePlayerOnFootSA();
};

////////////////////
// Complex facial //
////////////////////
class CTaskComplexFacialSAInterface : public CTaskComplexSAInterface
{
public:
    BYTE m_Pad[32];
};

class CTaskComplexFacialSA : public virtual CTaskComplexSA, public virtual CTaskComplexFacial
{
public:
    CTaskComplexFacialSA();
};

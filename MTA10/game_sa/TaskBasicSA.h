/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskBasicSA.h
*  PURPOSE:		Basic game tasks
*  DEVELOPERS:	Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKBASIC
#define __CGAMESA_TASKBASIC

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <game/TaskBasic.h>

#include "TaskSA.h"

#define FUNC_CTaskComplexUseMobilePhone__Constructor        0x6348A0
#define FUNC_CTaskSimpleRunAnim__Constructor                0x61A900
#define FUNC_CTaskSimpleRunNamedAnim__Constructor           0x61A990
#define FUNC_CTaskComplexDie__Constructor                   0x630040
#define FUNC_CTaskSimpleStealthKill__Constructor            0x6225F0
#define FUNC_CTaskComplexSunbathe__Constructor              0x631F80
#define FUNC_CTASKSimplePlayerOnFoot__Constructor           0x685750
#define FUNC_CTASKComplexFacial__Constructor                0x690D20


///////////////////////
//Use a mobile phone
///////////////////////

class CTaskComplexUseMobilePhoneSAInterface : public CTaskComplexSAInterface
{
public:
	int m_iDuration;
	CTaskTimer m_timer;
	bool m_bIsAborting;
	bool m_bQuit;
};


class CTaskComplexUseMobilePhoneSA : public virtual CTaskComplexSA, public virtual CTaskComplexUseMobilePhone
{
public:
    CTaskComplexUseMobilePhoneSA ( void ) {};
	CTaskComplexUseMobilePhoneSA ( const int iDuration ); // Default is -1
};


// temporary
class CAnimBlendAssociation;
class CAnimBlendHierarchy;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;


class CTaskSimpleAnimSAInterface: public CTaskSimpleSAInterface
{
public:
    CAnimBlendAssociation*      m_pAnim;    // 8
    char                        m_bIsFinished		:1; //12
    char                        m_bDontInterrupt	:1;
    char                        m_bHoldLastFrame	:1;

    // These flags are used in CTaskSimpleRunAnim only
    char                        m_bDontBlendOut	    :1;

    // These flags are used in 	CTaskSimpleRunNamedAnim only
    char                        m_bRunInSequence    :1;
    char                        m_bOffsetAtEnd		:1;
    char                        m_bOffsetAvailable	:1;
};



class CTaskSimpleAnimSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleAnim
{
public:
    CTaskSimpleAnimSA ( void ) {};
};




class CTaskSimpleRunAnimSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleRunAnim
{
public:
    CTaskSimpleRunAnimSA ( void ) {};
    CTaskSimpleRunAnimSA ( const AssocGroupId animGroup,
                           const AnimationId animID, 
                           const float fBlendDelta, 
                           const int iTaskType,
                           const char* pTaskName,
                           const bool bHoldLastFrame = false );
};



#define ANIM_NAMELEN 24
#define ANIMBLOCK_NAMELEN 16

class CTaskSimpleRunNamedAnimSAInterface: public CTaskSimpleAnimSAInterface
{
public:
    char                    m_animName [ANIM_NAMELEN]; //16
    char                    m_animGroupName [ANIMBLOCK_NAMELEN]; //40

    float                   m_fBlendDelta; //56
    CAnimBlendHierarchy*    m_pAnimHierarchy; //60
    int                     m_iTime; //64
    CTaskTimer              m_timer; //68
    CVector                 m_offsetAtEnd; //80
    int                     m_flags; //92  
    short                   m_animID; //96
};

class CTaskSimpleRunNamedAnimSA : public virtual CTaskSimpleAnimSA, public virtual CTaskSimpleRunNamedAnim
{
public:
    CTaskSimpleRunNamedAnimSA ( void ) {};
    CTaskSimpleRunNamedAnimSA ( const char* pAnimName,
                                const char* pAnimGroupName,
                                const int flags,
                                const float fBlendDelta,
                                const int iTime = -1,
                                const bool bDontInterrupt = false,
                                const bool bRunInSequence = false,
                                const bool bOffsetPed = false,
                                const bool bHoldLastFrame = false );
};

class CTaskComplexDieSAInterface : public CTaskComplexSAInterface
{
public:
};

class CTaskComplexDieSA : public virtual CTaskComplexSA, public virtual CTaskComplexDie
{
public:
    CTaskComplexDieSA ( void ) {};
    CTaskComplexDieSA ( const eWeaponType eMeansOfDeath/*=WEAPONTYPE_UNARMED*/,
                        const AssocGroupId animGroup=0/*ANIM_STD_PED*/,
                        const AnimationId anim=0/*ANIM_STD_KO_FRONT*/, 
                        const float fBlendDelta=4.0f,
                        const float fAnimSpeed=0.0f,
                        const bool bBeingKilledByStealth=false,
                        const bool bFallingToDeath=false,
                        const int iFallToDeathDir=0,
                        const bool bFallToDeathOverRailing=false );
};

class CTaskSimpleStealthKillInterface : public CTaskSimpleSAInterface
{
public:
};

class CTaskSimpleStealthKillSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleStealthKill
{
public:
    CTaskSimpleStealthKillSA ( void ) {};
    CTaskSimpleStealthKillSA ( bool bKiller,
                               class CPed * pPed,
                               const AssocGroupId animGroup );
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
    int               m_BeachAnimBlockIndex;
    int               m_SunbatheAnimBlockIndex;
    CObject*            m_pTowel;
};

class CTaskComplexSunbatheSA : public virtual CTaskComplexSA, public virtual CTaskComplexSunbathe
{
public:
                        CTaskComplexSunbatheSA ( void ) {};
                        CTaskComplexSunbatheSA ( class CObject* pTowel, const bool bStartStanding );

    void                SetEndTime ( DWORD dwTime );
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
    CTaskSimplePlayerOnFootSA ( void );
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
    CTaskComplexFacialSA ( void );
};

#endif

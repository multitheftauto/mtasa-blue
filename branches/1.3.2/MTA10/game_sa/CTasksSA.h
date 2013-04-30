/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTasksSA.h
*  PURPOSE:     Header file for task creation class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKS
#define __CGAMESA_TASKS

#include <game/CTasks.h>
#include "Common.h"

class CTaskManagementSystemSA;
class CVehicle;

// task headers
#include "TaskBasicSA.h"
#include "TaskCarAccessoriesSA.h"
#include "TaskGoToSA.h"
#include "TaskCarSA.h"
#include "TaskJumpFallSA.h"
#include "TaskSecondarySA.h"
#include "TaskPhysicalResponseSA.h"
#include "TaskIKSA.h"
#include "TaskAttackSA.h"

class CTasksSA : public CTasks
{
private:
    CTaskManagementSystemSA*            m_pTaskManagementSystem;

public:
                                        CTasksSA                                ( CTaskManagementSystemSA* pTaskManagementSystem );

    CTaskSimplePlayerOnFoot*            CreateTaskSimplePlayerOnFoot            ( void );
    CTaskComplexFacial*                 CreateTaskComplexFacial                 ( void );

    CTaskSimpleCarSetPedInAsDriver*     CreateTaskSimpleCarSetPedInAsDriver     ( CVehicle * pVehicle);
    CTaskSimpleCarSetPedInAsPassenger*  CreateTaskSimpleCarSetPedInAsPassenger  ( CVehicle * pVehicle, int iTargetDoor);
    CTaskSimpleCarSetPedOut*            CreateTaskSimpleCarSetPedOut            ( CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine=false);

    CTaskComplexWanderStandard*         CreateTaskComplexWanderStandard         ( const int iMoveState, const char iDir, const bool bWanderSensibly=true);
    CTaskComplexEnterCarAsDriver*       CreateTaskComplexEnterCarAsDriver       ( CVehicle * pVehicle);
    CTaskComplexEnterCarAsPassenger*    CreateTaskComplexEnterCarAsPassenger    ( CVehicle * pVehicle, const int iTargetSeat=0, const bool bCarryOnAfterFallingOff=false );
    CTaskComplexEnterBoatAsDriver*      CreateTaskComplexEnterBoatAsDriver      ( CVehicle * pVehicle );
    CTaskComplexUseMobilePhone*         CreateTaskComplexUseMobilePhone         ( const int iDuration = -1 );

    CTaskComplexLeaveCar*               CreateTaskComplexLeaveCar               ( CVehicle* pVehicle, const int iTargetDoor=0xFF, const int iDelayTime=0, const bool bSensibleLeaveCar=true, const bool bForceGetOut=false );
    CTaskSimpleDuck*                    CreateTaskSimpleDuck                    ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck=0, unsigned short nUseShotsWhizzingEvents=-1 );
    CTaskSimpleChoking*                 CreateTaskSimpleChoking                 ( CPed* pAttacker, bool bIsTearGas );

    CTaskSimpleClimb*                   CreateTaskSimpleClimb                   ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight = CLIMB_GRAB, const bool bForceClimb = false );
    CTaskSimpleJetPack*                 CreateTaskSimpleJetpack                 ( const CVector *pVecTargetPos = NULL, float fCruiseHeight = 10.0f, int nHoverTime = 0 );

    CTaskSimpleRunAnim*                 CreateTaskSimpleRunAnim                 ( const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType, const char* pTaskName, const bool bHoldLastFrame = false );
    CTaskSimpleRunNamedAnim*            CreateTaskSimpleRunNamedAnim            ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime = -1, const bool bDontInterrupt = false, const bool bRunInSequence = false, const bool bOffsetPed = false, const bool bHoldLastFrame = false );

    CTaskComplexDie*                    CreateTaskComplexDie                    ( const eWeaponType eMeansOfDeath=WEAPONTYPE_UNARMED, const AssocGroupId animGroup=0/*ANIM_STD_PED*/, const AnimationId anim=0/*ANIM_STD_KO_FRONT*/, const float fBlendDelta=4.0f, const float fAnimSpeed=0.0f, const bool bBeingKilledByStealth=false, const bool bFallingToDeath=false, const int iFallToDeathDir=0, const bool bFallToDeathOverRailing=false );
    CTaskSimpleStealthKill*             CreateTaskSimpleStealthKill             ( bool bAttacker, class CPed * pPed, const AnimationId anim );
    CTaskSimpleDead*                    CreateTaskSimpleDead                    ( unsigned int uiDeathTimeMS, bool bUnk2 );
    CTaskSimpleBeHit*                   CreateTaskSimpleBeHit                   ( CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId );

    CTaskComplexSunbathe*               CreateTaskComplexSunbathe               ( class CObject* pTowel, const bool bStartStanding );

    // IK
    CTaskSimpleIKChain*                 CreateTaskSimpleIKChain                 ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time=99999999, int blendTime=1000 );
    CTaskSimpleIKLookAt*                CreateTaskSimpleIKLookAt                ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso=false, float speed=0.25f, int blendTime=1000, int m_priority=3 );
    CTaskSimpleTriggerLookAt*           CreateTaskSimpleTriggerLookAt           ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso=false, float speed=0.25f, int blendTime=1000, int priority=3 );

    // Attack
    CTaskSimpleGangDriveBy*             CreateTaskSimpleGangDriveBy             ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS );
    CTaskSimpleUseGun*                  CreateTaskSimpleUseGun                  ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength=1, unsigned char bAimImmediate=false );
    CTaskSimpleFight*                   CreateTaskSimpleFight                   ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod = 10000 );

    static void                         StaticSetHooks                          ( void );
};

#endif

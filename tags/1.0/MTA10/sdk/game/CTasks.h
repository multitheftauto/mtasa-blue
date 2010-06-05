/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CTasks.h
*  PURPOSE:		Tasks interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKS
#define __CGAME_TASKS

#include <CVector.h>
#include "CTaskManager.h"
#include "CWeaponInfo.h"

typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;
class CEntity;

enum eClimbHeights
{
	CLIMB_NOT_READY = 0,
	CLIMB_GRAB,
	CLIMB_PULLUP,
	CLIMB_STANDUP,
	CLIMB_FINISHED,
	CLIMB_VAULT,
	CLIMB_FINISHED_V
};

class CTasks
{
public:
    virtual CTaskSimplePlayerOnFoot             * CreateTaskSimplePlayerOnFoot( void ) = 0;
    virtual CTaskComplexFacial                  * CreateTaskComplexFacial( void ) = 0;

	virtual CTaskSimpleCarSetPedInAsDriver		* CreateTaskSimpleCarSetPedInAsDriver(CVehicle * pVehicle)=0;
	virtual CTaskSimpleCarSetPedInAsPassenger	* CreateTaskSimpleCarSetPedInAsPassenger(CVehicle * pVehicle, int iTargetDoor)=0;
	virtual CTaskSimpleCarSetPedOut				* CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine=false)=0;

    virtual CTaskComplexWanderStandard          * CreateTaskComplexWanderStandard(const int iMoveState, const char iDir, const bool bWanderSensibly=true)=0;
    virtual CTaskComplexEnterCarAsDriver        * CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)=0;
    virtual CTaskComplexEnterCarAsPassenger     * CreateTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat=0, const bool bCarryOnAfterFallingOff=false )=0;
    virtual CTaskComplexEnterBoatAsDriver       * CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle )=0;
    virtual CTaskComplexLeaveCar                * CreateTaskComplexLeaveCar ( CVehicle* pVehicle, const int iTargetDoor=0, const int iDelayTime=0, const bool bSensibleLeaveCar=true, const bool bForceGetOut=false )=0;
    virtual CTaskComplexUseMobilePhone          * CreateTaskComplexUseMobilePhone ( const int iDuration = -1 ) = 0;

    virtual CTaskSimpleDuck                     * CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck=0, unsigned short nUseShotsWhizzingEvents=-1 )=0;
    virtual CTaskSimpleChoking                  * CreateTaskSimpleChoking ( CPed* pAttacker, bool bIsTearGas )=0;

    virtual CTaskSimpleClimb                    * CreateTaskSimpleClimb                   ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight = CLIMB_GRAB, const bool bForceClimb = false )=0;
    virtual CTaskSimpleJetPack                  * CreateTaskSimpleJetpack                 ( const CVector *pVecTargetPos = NULL, float fCruiseHeight = 10.0f, int nHoverTime = 0 )=0;

    virtual CTaskSimpleRunAnim                  * CreateTaskSimpleRunAnim                 ( const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType, const char* pTaskName, const bool bHoldLastFrame = false )=0;
    virtual CTaskSimpleRunNamedAnim             * CreateTaskSimpleRunNamedAnim            ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime = -1, const bool bDontInterrupt = false, const bool bRunInSequence = false, const bool bOffsetPed = false, const bool bHoldLastFrame = false )=0;

    virtual CTaskComplexDie                     * CreateTaskComplexDie                    ( const eWeaponType eMeansOfDeath=WEAPONTYPE_UNARMED, const AssocGroupId animGroup=0/*ANIM_STD_PED*/, const AnimationId anim=0/*ANIM_STD_KO_FRONT*/, const float fBlendDelta=4.0f, const float fAnimSpeed=0.0f, const bool bBeingKilledByStealth=false, const bool bFallingToDeath=false, const int iFallToDeathDir=0, const bool bFallToDeathOverRailing=false )=0;
    virtual CTaskSimpleStealthKill              * CreateTaskSimpleStealthKill             ( bool bAttacker, class CPed * pPed, const AnimationId anim )=0;

    virtual CTaskComplexSunbathe                * CreateTaskComplexSunbathe               ( class CObject* pTowel, const bool bStartStanding )=0;

    // IK
    virtual CTaskSimpleIKChain                  * CreateTaskSimpleIKChain                 ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time=99999999, int blendTime=1000 )=0;
    virtual CTaskSimpleIKLookAt                 * CreateTaskSimpleIKLookAt                ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso=false, float speed=0.25f, int blendTime=1000, int m_priority=3 )=0;
    virtual CTaskSimpleTriggerLookAt            * CreateTaskSimpleTriggerLookAt           ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso=false, float speed=0.25f, int blendTime=1000, int priority=3 )=0;

    // Attack
    virtual CTaskSimpleGangDriveBy              * CreateTaskSimpleGangDriveBy             ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )=0;
    virtual CTaskSimpleUseGun                   * CreateTaskSimpleUseGun                  ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength=1, unsigned char bAimImmediate=false )=0;
    virtual CTaskSimpleFight                    * CreateTaskSimpleFight                   ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod = 10000 )=0;
};

#endif
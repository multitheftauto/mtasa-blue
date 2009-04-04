/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTasksSA.cpp
*  PURPOSE:     Task creation
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTasksSA::CTasksSA ( CTaskManagementSystemSA* pTaskManagementSystem )
{
	DEBUG_TRACE("CTasksSA::CTasksSA ( CTaskManagementSystemSA* pTaskManagementSystem )");
    m_pTaskManagementSystem = pTaskManagementSystem;
}


CTaskSimplePlayerOnFoot* CTasksSA::CreateTaskSimplePlayerOnFoot ( void )
{
    DEBUG_TRACE ("CTaskSimplePlayerOnFoot* CTasksSA::CreateTaskSimplePlayerOnFoot ( void )");

    CTaskSimplePlayerOnFootSA * pTask = new CTaskSimplePlayerOnFootSA ( );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial ( void )
{
    DEBUG_TRACE ("CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial ( void )");

    CTaskComplexFacialSA * pTask = new CTaskComplexFacialSA ( );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskSimpleCarSetPedInAsDriver * CTasksSA::CreateTaskSimpleCarSetPedInAsDriver(CVehicle * pVehicle)
{
	DEBUG_TRACE("CTaskSimpleCarSetPedInAsDriver * CTasksSA::CreateTaskSimpleCarSetPedInAsDriver(CVehicle * pVehicle)");
	CTaskSimpleCarSetPedInAsDriverSA * pTask = new CTaskSimpleCarSetPedInAsDriverSA(pVehicle, NULL);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleCarSetPedInAsPassenger * CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger(CVehicle * pVehicle, int iTargetDoor)
{
	DEBUG_TRACE("CTaskSimpleCarSetPedInAsPassenger * CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger(CVehicle * pVehicle, int iTargetDoor)");
	CTaskSimpleCarSetPedInAsPassengerSA * pTask = new CTaskSimpleCarSetPedInAsPassengerSA(pVehicle, iTargetDoor, NULL);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleCarSetPedOut * CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
	DEBUG_TRACE("CTaskSimpleCarSetPedOut * CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)");
	CTaskSimpleCarSetPedOutSA * pTask = new CTaskSimpleCarSetPedOutSA(pVehicle, iTargetDoor, bSwitchOffEngine);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskComplexWanderStandard * CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const char iDir, const bool bWanderSensibly)
{
	DEBUG_TRACE("CTaskComplexWanderStandard * CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)");
	CTaskComplexWanderStandardSA * pTask = new CTaskComplexWanderStandardSA(iMoveState, iDir, bWanderSensibly);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterCarAsDriver * CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterCarAsDriver * CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)");
    CTaskComplexEnterCarAsDriverSA * pTask = new CTaskComplexEnterCarAsDriverSA ( pVehicle );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterCarAsPassenger * CTasksSA::CreateTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff )
{
    DEBUG_TRACE("CTaskComplexEnterCarAsPassenger * CTasksSA::CTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff)");
    CTaskComplexEnterCarAsPassengerSA * pTask = new CTaskComplexEnterCarAsPassengerSA ( pVehicle, iTargetSeat, bCarryOnAfterFallingOff );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterBoatAsDriver * CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterBoatAsDriver * CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)");
    CTaskComplexEnterBoatAsDriverSA * pTask = new CTaskComplexEnterBoatAsDriverSA ( pVehicle );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexLeaveCar * CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )
{
    DEBUG_TRACE("CTaskComplexLeaveCar * CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )");
    CTaskComplexLeaveCarSA * pTask = new CTaskComplexLeaveCarSA ( pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )
{
    DEBUG_TRACE("CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )");
    CTaskComplexUseMobilePhoneSA * pTask = new CTaskComplexUseMobilePhoneSA ( iDuration );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleDuck * CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )
{
    DEBUG_TRACE("CTaskSimpleDuck * CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )");
    CTaskSimpleDuckSA * pTask = new CTaskSimpleDuckSA ( nDuckControl, nLengthOfDuck, nUseShotsWhizzingEvents );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleChoking * CTasksSA::CreateTaskSimpleChoking ( CPed* pAttacker, bool bIsTearGas )
{
    DEBUG_TRACE("CTaskSimpleChoking * CTasksSA::CreateTaskSimpleChoking ( void )");
    CTaskSimpleChokingSA * pTask = new CTaskSimpleChokingSA ( pAttacker, bIsTearGas );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )
{
    DEBUG_TRACE("CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )");
    CTaskSimpleClimbSA * pTask = new CTaskSimpleClimbSA ( pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleJetPack * CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )
{
    DEBUG_TRACE("CTaskSimpleJetPack * CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )");
    CTaskSimpleJetPackSA * pTask = new CTaskSimpleJetPackSA ( pVecTargetPos, fCruiseHeight, nHoverTime );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleRunAnim* CTasksSA::CreateTaskSimpleRunAnim ( const AssocGroupId animGroup,
                                                        const AnimationId animID, 
                                                        const float fBlendDelta, 
                                                        const int iTaskType,
                                                        const char* pTaskName,
                                                        const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunAnim* CTasksSA::CreateTaskSimpleRunAnim ( const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta,  const int iTaskType, const char* pTaskName, const bool bHoldLastFrame = false )");

    CTaskSimpleRunAnimSA * pTask = new CTaskSimpleRunAnimSA ( animGroup, animID, fBlendDelta, iTaskType, pTaskName, bHoldLastFrame );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}



CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )");

    CTaskSimpleRunNamedAnimSA * pTask = new CTaskSimpleRunNamedAnimSA ( pAnimName, pAnimGroupName, flags, fBlendDelta, iTime, bDontInterrupt, bRunInSequence, bOffsetPed, bHoldLastFrame );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )
{
    DEBUG_TRACE("CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )");

    CTaskComplexDieSA * pTask = new CTaskComplexDieSA ( eMeansOfDeath, animGroup, anim, fBlendDelta, fAnimSpeed, bBeingKilledByStealth, bFallingToDeath, iFallToDeathDir, bFallToDeathOverRailing );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, const AnimationId animGroup )
{
    DEBUG_TRACE("CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, const AnimationId anim )");

    CTaskSimpleStealthKillSA * pTask = new CTaskSimpleStealthKillSA ( bKiller, pPed, animGroup );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )
{
    DEBUG_TRACE("CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )");

    CTaskComplexSunbatheSA * pTask = new CTaskComplexSunbatheSA ( pTowel, bStartStanding );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )
{
    DEBUG_TRACE("CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )");

    CTaskSimpleIKChainSA * pTask = new CTaskSimpleIKChainSA ( idString, effectorBoneTag, effectorVec, pivotBoneTag, pEntity, offsetBoneTag, offsetPos, speed, time, blendTime );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )
{
    DEBUG_TRACE("CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )");

    CTaskSimpleIKLookAtSA * pTask = new CTaskSimpleIKLookAtSA ( idString, pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, m_priority );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )
{
    DEBUG_TRACE("CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )");

    CTaskSimpleTriggerLookAtSA * pTask = new CTaskSimpleTriggerLookAtSA ( pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, priority );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )
{
    DEBUG_TRACE("CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )");

    CTaskSimpleGangDriveBySA * pTask = new CTaskSimpleGangDriveBySA ( pTargetEntity, pVecTarget, fAbortRange, FrequencyPercentage, nDrivebyStyle, bSeatRHS );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )
{
    DEBUG_TRACE("CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )");

    CTaskSimpleUseGunSA * pTask = new CTaskSimpleUseGunSA ( pTargetEntity, vecTarget, nCommand, nBurstLength, bAimImmediate );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )
{
    DEBUG_TRACE ("CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )");

    CTaskSimpleFightSA * pTask = new CTaskSimpleFightSA ( pTargetEntity, nCommand, nIdlePeriod );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}
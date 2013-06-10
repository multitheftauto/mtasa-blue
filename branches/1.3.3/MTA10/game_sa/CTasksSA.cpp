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

    CTaskSimplePlayerOnFootSA * pTask = NewTask < CTaskSimplePlayerOnFootSA > ( );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial ( void )
{
    DEBUG_TRACE ("CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial ( void )");

    CTaskComplexFacialSA * pTask = NewTask < CTaskComplexFacialSA > ( );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskSimpleCarSetPedInAsDriver * CTasksSA::CreateTaskSimpleCarSetPedInAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsDriver * CTasksSA::CreateTaskSimpleCarSetPedInAsDriver(CVehicle * pVehicle)");
    CTaskSimpleCarSetPedInAsDriverSA * pTask = NewTask < CTaskSimpleCarSetPedInAsDriverSA > (pVehicle, (CTaskUtilityLineUpPedWithCar*)NULL);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleCarSetPedInAsPassenger * CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger(CVehicle * pVehicle, int iTargetDoor)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsPassenger * CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger(CVehicle * pVehicle, int iTargetDoor)");
    CTaskSimpleCarSetPedInAsPassengerSA * pTask = NewTask < CTaskSimpleCarSetPedInAsPassengerSA > (pVehicle, iTargetDoor, (CTaskUtilityLineUpPedWithCar*)NULL);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleCarSetPedOut * CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedOut * CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)");
    CTaskSimpleCarSetPedOutSA * pTask = NewTask < CTaskSimpleCarSetPedOutSA > (pVehicle, iTargetDoor, bSwitchOffEngine);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskComplexWanderStandard * CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const char iDir, const bool bWanderSensibly)
{
    DEBUG_TRACE("CTaskComplexWanderStandard * CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)");
    CTaskComplexWanderStandardSA * pTask = NewTask < CTaskComplexWanderStandardSA > (iMoveState, iDir, bWanderSensibly);
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterCarAsDriver * CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterCarAsDriver * CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)");
    CTaskComplexEnterCarAsDriverSA * pTask = NewTask < CTaskComplexEnterCarAsDriverSA > ( pVehicle );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterCarAsPassenger * CTasksSA::CreateTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff )
{
    DEBUG_TRACE("CTaskComplexEnterCarAsPassenger * CTasksSA::CTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff)");
    CTaskComplexEnterCarAsPassengerSA * pTask = NewTask < CTaskComplexEnterCarAsPassengerSA > ( pVehicle, iTargetSeat, bCarryOnAfterFallingOff );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexEnterBoatAsDriver * CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterBoatAsDriver * CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)");
    CTaskComplexEnterBoatAsDriverSA * pTask = NewTask < CTaskComplexEnterBoatAsDriverSA > ( pVehicle );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexLeaveCar * CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )
{
    DEBUG_TRACE("CTaskComplexLeaveCar * CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )");
    CTaskComplexLeaveCarSA * pTask = NewTask < CTaskComplexLeaveCarSA > ( pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )
{
    DEBUG_TRACE("CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )");
    CTaskComplexUseMobilePhoneSA * pTask = NewTask < CTaskComplexUseMobilePhoneSA > ( iDuration );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleDuck * CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )
{
    DEBUG_TRACE("CTaskSimpleDuck * CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )");
    CTaskSimpleDuckSA * pTask = NewTask < CTaskSimpleDuckSA > ( nDuckControl, nLengthOfDuck, nUseShotsWhizzingEvents );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleChoking * CTasksSA::CreateTaskSimpleChoking ( CPed* pAttacker, bool bIsTearGas )
{
    DEBUG_TRACE("CTaskSimpleChoking * CTasksSA::CreateTaskSimpleChoking ( void )");
    CTaskSimpleChokingSA * pTask = NewTask < CTaskSimpleChokingSA > ( pAttacker, bIsTearGas );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )
{
    DEBUG_TRACE("CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )");
    CTaskSimpleClimbSA * pTask = NewTask < CTaskSimpleClimbSA > ( pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleJetPack * CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )
{
    DEBUG_TRACE("CTaskSimpleJetPack * CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )");
    CTaskSimpleJetPackSA * pTask = NewTask < CTaskSimpleJetPackSA > ( pVecTargetPos, fCruiseHeight, nHoverTime );
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

    CTaskSimpleRunAnimSA * pTask = NewTask < CTaskSimpleRunAnimSA > ( animGroup, animID, fBlendDelta, iTaskType, pTaskName, bHoldLastFrame );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}



CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )");

    CTaskSimpleRunNamedAnimSA * pTask = NewTask < CTaskSimpleRunNamedAnimSA > ( pAnimName, pAnimGroupName, flags, fBlendDelta, iTime, bDontInterrupt, bRunInSequence, bOffsetPed, bHoldLastFrame );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )
{
    DEBUG_TRACE("CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )");

    CTaskComplexDieSA * pTask = NewTask < CTaskComplexDieSA > ( eMeansOfDeath, animGroup, anim, fBlendDelta, fAnimSpeed, bBeingKilledByStealth, bFallingToDeath, iFallToDeathDir, bFallToDeathOverRailing );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, const AnimationId animGroup )
{
    DEBUG_TRACE("CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, const AnimationId anim )");

    CTaskSimpleStealthKillSA * pTask = NewTask < CTaskSimpleStealthKillSA > ( bKiller, pPed, animGroup );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskSimpleDead* CTasksSA::CreateTaskSimpleDead ( unsigned int uiDeathTimeMS, bool bUnk )
{
    DEBUG_TRACE("CTaskSimpleDead* CTasksSA::CreateTaskSimpleDead ( unsigned int uiDeathTimeMS, bool bUnk )");

    CTaskSimpleDeadSA * pTask = NewTask < CTaskSimpleDeadSA > ( uiDeathTimeMS, bUnk );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}

CTaskSimpleBeHit* CTasksSA::CreateTaskSimpleBeHit ( CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId )
{
    DEBUG_TRACE("CTaskSimpleBeHit* CTasksSA::CreateTaskSimpleBeHit ( CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId )");

    CTaskSimpleBeHitSA * pTask = NewTask < CTaskSimpleBeHitSA > ( pPedAttacker, hitBodyPart, hitBodySide, weaponId );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )
{
    DEBUG_TRACE("CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )");

    CTaskComplexSunbatheSA * pTask = NewTask < CTaskComplexSunbatheSA > ( pTowel, bStartStanding );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )
{
    DEBUG_TRACE("CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )");

    CTaskSimpleIKChainSA * pTask = NewTask < CTaskSimpleIKChainSA > ( idString, effectorBoneTag, effectorVec, pivotBoneTag, pEntity, offsetBoneTag, offsetPos, speed, time, blendTime );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )
{
    DEBUG_TRACE("CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )");

    CTaskSimpleIKLookAtSA * pTask = NewTask < CTaskSimpleIKLookAtSA > ( idString, pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, m_priority );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )
{
    DEBUG_TRACE("CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )");

    CTaskSimpleTriggerLookAtSA * pTask = NewTask < CTaskSimpleTriggerLookAtSA > ( pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, priority );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )
{
    DEBUG_TRACE("CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )");

    CTaskSimpleGangDriveBySA * pTask = NewTask < CTaskSimpleGangDriveBySA > ( pTargetEntity, pVecTarget, fAbortRange, FrequencyPercentage, nDrivebyStyle, bSeatRHS );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )
{
    DEBUG_TRACE("CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )");

    CTaskSimpleUseGunSA * pTask = NewTask < CTaskSimpleUseGunSA > ( pTargetEntity, vecTarget, nCommand, nBurstLength, bAimImmediate );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )
{
    DEBUG_TRACE ("CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )");

    CTaskSimpleFightSA * pTask = NewTask < CTaskSimpleFightSA > ( pTargetEntity, nCommand, nIdlePeriod );
    m_pTaskManagementSystem->AddTask ( pTask );
    return pTask;
}


////////////////////////////////////////////////////////////////
//
// CEventHandler_ComputeDamageResponse_Mid
//
// Detect when GTA will start the 'be hit' task
//
////////////////////////////////////////////////////////////////
void _cdecl OnCEventHandler_ComputeDamageResponse_Mid( CPedSAInterface* pPedVictim, CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId )
{
    // Make sure victim is local player
    CPedSAInterface* pLocalPlayer = ((CPoolsSA *)pGame->GetPools())->GetPedInterface( (DWORD)1 );
    if ( pPedVictim != pLocalPlayer )
        return;

    if ( pGame->m_pTaskSimpleBeHitHandler  )
        pGame->m_pTaskSimpleBeHitHandler( pPedAttacker, hitBodyPart, hitBodySide, weaponId );

}

// Hook info
#define HOOKPOS_CEventHandler_ComputeDamageResponse_Mid        0x4C0593
#define HOOKSIZE_CEventHandler_ComputeDamageResponse_Mid       5
DWORD RETURN_CEventHandler_ComputeDamageResponse_Mid =         0x4C0598;
DWORD CTaskSimpleBeHit_constructor = FUNC_CTaskSimpleBeHit__Constructor;
void _declspec(naked) HOOK_CEventHandler_ComputeDamageResponse_Mid()
{
    _asm
    {
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [edi]
        call    OnCEventHandler_ComputeDamageResponse_Mid
        add     esp, 4*4+4
        popad

        // Replaced code
        call    CTaskSimpleBeHit_constructor
        jmp     RETURN_CEventHandler_ComputeDamageResponse_Mid
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CTasksSA::StaticSetHooks( void )
{
   EZHookInstall( CEventHandler_ComputeDamageResponse_Mid );
}

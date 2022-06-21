/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskBasicSA.cpp
 *  PURPOSE:     Basic game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA(const int iDuration)
{
    DEBUG_TRACE("CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA(const int iDuration)");

    this->CreateTaskInterface(sizeof(CTaskComplexUseMobilePhoneSAInterface));
    if (!IsValid())
        return;

    // CTaskComplexUseMobilePhone::CTaskComplexUseMobilePhone
    ((void*(__thiscall*)(void*, int))FUNC_CTaskComplexUseMobilePhone__Constructor)(GetInterface(), iDuration);
}

CTaskSimpleRunAnimSA::CTaskSimpleRunAnimSA(const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType,
                                           const char* pTaskName, const bool bHoldLastFrame)
{
    DEBUG_TRACE(
        "CTaskSimpleRunAnim::CTaskSimpleRunAnim (  const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType, const "
        "char* pTaskName, const bool bHoldLastFrame );");

    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    // CTaskSimpleRunAnim::CTaskSimpleRunAnim
    ((void*(__thiscall*)(void*, AssocGroupId, AnimationId, float, int, const char*, bool))FUNC_CTaskSimpleRunAnim__Constructor)(
        GetInterface(), animGroup, animID, fBlendDelta, iTaskType, pTaskName, bHoldLastFrame);
}

CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA(const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta,
                                                     const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed,
                                                     const bool bHoldLastFrame)
{
    DEBUG_TRACE(
        "CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA (  const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, "
        "const int iTime = -1, const bool bDontInterrupt = false, const bool bRunInSequence = false, const bool bOffsetPed = false, const bool bHoldLastFrame "
        "= false )");

    // TODO: Find out the real size
    this->CreateTaskInterface(sizeof(CTaskSimpleRunNamedAnimSAInterface));
    if (!IsValid())
        return;

    // CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim
    ((void*(__thiscall*)(void*, const char*, const char*, int, float, int, bool, bool, bool, bool))FUNC_CTaskSimpleRunNamedAnim__Constructor)(
        GetInterface(), pAnimName, pAnimGroupName, flags, fBlendDelta, iTime, bDontInterrupt, bRunInSequence, bOffsetPed, bHoldLastFrame);
}

CTaskComplexDieSA::CTaskComplexDieSA(const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta,
                                     const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir,
                                     const bool bFallToDeathOverRailing)
{
    DEBUG_TRACE(
        "CTaskComplexDieSA::CTaskComplexDieSA ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float "
        "fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool "
        "bFallToDeathOverRailing )");

    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    // CTaskComplexDie::CTaskComplexDie
    ((void*(__thiscall*)(void*, eWeaponType, AssocGroupId, AnimationId, float, float, bool, bool, int, bool))FUNC_CTaskComplexDie__Constructor)(
        GetInterface(), eMeansOfDeath, animGroup, anim, fBlendDelta, fAnimSpeed, bBeingKilledByStealth, bFallingToDeath, iFallToDeathDir,
        bFallToDeathOverRailing);
}

CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA(bool bKiller, CPed* pPed, const AssocGroupId animGroup)
{
    DEBUG_TRACE("CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA ( bool bKiller, class CPed * pPed, const AssocGroupId animGroup )");

    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    CPedSAInterface* pPedInterface = pPed->GetPedInterface();

    // CTaskSimpleStealthKill::CTaskSimpleStealthKill
    ((void*(__thiscall*)(void*, bool, CPedSAInterface*, AssocGroupId))FUNC_CTaskSimpleStealthKill__Constructor)(GetInterface(), bKiller, pPedInterface,
                                                                                                                animGroup);
}

CTaskSimpleDeadSA::CTaskSimpleDeadSA(unsigned int uiDeathTimeMS, bool bUnk2)
{
    DEBUG_TRACE("CTaskSimpleDeadSA::CTaskSimpleDeadSA ( int iUnk1, bool bUnk2 )");

    this->CreateTaskInterface(sizeof(CTaskSimpleDeadSAInterface));
    if (!IsValid())
        return;

    // CTaskSimpleDead::CTaskSimpleDead
    ((void*(__thiscall*)(void*, unsigned int, bool))FUNC_CTaskSimpleDead__Constructor)(GetInterface(), uiDeathTimeMS, bUnk2);
}

CTaskSimpleBeHitSA::CTaskSimpleBeHitSA(CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId)
{
    DEBUG_TRACE("CTaskSimpleBeHitSA::CTaskSimpleBeHitSA ( CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId )");

    this->CreateTaskInterface(sizeof(CTaskSimpleBeHitSAInterface));
    if (!IsValid())
        return;

    CPedSAInterface* pPedInterface = pPedAttacker->GetPedInterface();

    // CTaskSimpleBeHit::CTaskSimpleBeHit
    ((void*(__thiscall*)(void*, CPedSAInterface*, ePedPieceTypes, int, int))FUNC_CTaskSimpleBeHit__Constructor)(GetInterface(), pPedInterface, hitBodyPart,
                                                                                                                hitBodySide, weaponId);
}

CTaskComplexSunbatheSA::CTaskComplexSunbatheSA(CObject* pTowel, const bool bStartStanding)
{
    DEBUG_TRACE("CTaskComplexSunbatheSA::CTaskComplexSunbatheSA ( CObject* pTowel, const bool bStartStanding )");

    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    CObjectSAInterface* pTowelInterface = pTowel ? pTowel->GetObjectInterface() : nullptr;

    // CTaskComplexSunbathe::CTaskComplexSunbathe
    ((void*(__thiscall*)(void*, CObjectSAInterface*, bool))FUNC_CTaskComplexSunbathe__Constructor)(GetInterface(), pTowelInterface, bStartStanding);
}

void CTaskComplexSunbatheSA::SetEndTime(DWORD dwTime)
{
    CTaskComplexSunbatheSAInterface* thisInterface = (CTaskComplexSunbatheSAInterface*)this->GetInterface();
    thisInterface->m_BathingTimer.dwTimeEnd = dwTime;
}

////////////////////
// Player on foot //
////////////////////
CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA()
{
    DEBUG_TRACE("CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA");
    this->CreateTaskInterface(sizeof(CTaskSimplePlayerOnFootSAInterface));
    if (!IsValid())
        return;

    // CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot
    ((void*(__thiscall*)(void*))FUNC_CTASKSimplePlayerOnFoot__Constructor)(GetInterface());
}

////////////////////
// Complex facial //
////////////////////
CTaskComplexFacialSA::CTaskComplexFacialSA()
{
    DEBUG_TRACE("CTaskComplexFacialSA::CTaskComplexFacialSA");
    this->CreateTaskInterface(sizeof(CTaskComplexFacialSAInterface));
    if (!IsValid())
        return;

    // CTaskComplexFacial::CTaskComplexFacial
    ((void*(__thiscall*)(void*))FUNC_CTASKComplexFacial__Constructor)(GetInterface());
}

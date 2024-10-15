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
#include "TaskBasicSA.h"
#include "CPedSA.h"


CTaskSimpleRunNamedAnimSAInterface* CTaskSimpleRunNamedAnimSA::GetAnimationInterface() noexcept
{
    return reinterpret_cast<CTaskSimpleRunNamedAnimSAInterface*>(this->GetInterface());
}

const CTaskSimpleRunNamedAnimSAInterface* CTaskSimpleRunNamedAnimSA::GetAnimationInterface() const noexcept
{
    return reinterpret_cast<const CTaskSimpleRunNamedAnimSAInterface*>(this->GetInterface());
}

const char* CTaskSimpleRunNamedAnimSA::GetAnimName() const noexcept
{
    return GetAnimationInterface()->m_animName;
}

const char* CTaskSimpleRunNamedAnimSA::GetGroupName() const noexcept
{
    return GetAnimationInterface()->m_animGroupName;
}

CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA(const int iDuration)
{
    CreateTaskInterface(sizeof(CTaskComplexUseMobilePhoneSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskComplexUseMobilePhone__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    iDuration
        call    dwFunc
    }
}

CTaskSimpleRunAnimSA::CTaskSimpleRunAnimSA(const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType,
                                           const char* pTaskName, const bool bHoldLastFrame)
{
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleRunAnim__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bHoldLastFrame
        push    pTaskName
        push    iTaskType
        push    fBlendDelta
        push    animID
        push    animGroup
        call    dwFunc
    }
}

CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA(const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta,
                                                     const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed,
                                                     const bool bHoldLastFrame)
{
    // TODO: Find out the real size
    CreateTaskInterface(sizeof(CTaskSimpleRunNamedAnimSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleRunNamedAnim__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bHoldLastFrame
        push    bOffsetPed
        push    bRunInSequence
        push    bDontInterrupt
        push    iTime
        push    fBlendDelta
        push    flags
        push    pAnimGroupName
        push    pAnimName
        call    dwFunc
    }
}

CTaskComplexDieSA::CTaskComplexDieSA(const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta,
                                     const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir,
                                     const bool bFallToDeathOverRailing)
{
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskComplexDie__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bFallToDeathOverRailing
        push    iFallToDeathDir
        push    bFallingToDeath
        push    bBeingKilledByStealth
        push    fAnimSpeed
        push    fBlendDelta
        push    anim
        push    animGroup
        push    eMeansOfDeath
        call    dwFunc
    }
}

CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA(bool bKiller, CPed* pPed, const AssocGroupId animGroup)
{
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleStealthKill__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetPedInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    animGroup
        push    dwPedInterface
        push    bKiller
        call    dwFunc
    }
}

CTaskSimpleDeadSA::CTaskSimpleDeadSA(unsigned int uiDeathTimeMS, bool bUnk2)
{
    CreateTaskInterface(sizeof(CTaskSimpleDeadSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleDead__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bUnk2
        push    uiDeathTimeMS
        call    dwFunc
    }
}

CTaskSimpleBeHitSA::CTaskSimpleBeHitSA(CPed* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId)
{
    CreateTaskInterface(sizeof(CTaskSimpleBeHitSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleBeHit__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPedAttacker->GetPedInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    weaponId
        push    hitBodySide
        push    hitBodyPart
        push    dwPedInterface
        call    dwFunc
    }
}

CTaskComplexSunbatheSA::CTaskComplexSunbatheSA(CObject* pTowel, const bool bStartStanding)
{
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskComplexSunbathe__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwObjectInterface = 0;
    if (pTowel)
        dwObjectInterface = (DWORD)pTowel->GetObjectInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bStartStanding
        push    dwObjectInterface;
        call    dwFunc
    }
}

void CTaskComplexSunbatheSA::SetEndTime(DWORD dwTime)
{
    CTaskComplexSunbatheSAInterface* thisInterface = (CTaskComplexSunbatheSAInterface*)GetInterface();
    thisInterface->m_BathingTimer.dwTimeEnd = dwTime;
}

////////////////////
// Player on foot //
////////////////////
CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA()
{
    CreateTaskInterface(sizeof(CTaskSimplePlayerOnFootSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = (DWORD)FUNC_CTASKSimplePlayerOnFoot__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
    }
}

////////////////////
// Complex facial //
////////////////////
CTaskComplexFacialSA::CTaskComplexFacialSA()
{
    CreateTaskInterface(sizeof(CTaskComplexFacialSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = (DWORD)FUNC_CTASKComplexFacial__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
    }
}

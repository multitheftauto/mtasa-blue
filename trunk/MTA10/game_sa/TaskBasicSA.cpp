/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskBasicSA.cpp
*  PURPOSE:     Basic game tasks
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA ( const int iDuration )
{
    DEBUG_TRACE("CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA(const int iDuration)");

    this->CreateTaskInterface ( sizeof(CTaskComplexUseMobilePhoneSAInterface ) );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskComplexUseMobilePhone__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    iDuration
        call    dwFunc
    }
}


CTaskSimpleRunAnimSA::CTaskSimpleRunAnimSA (    const AssocGroupId animGroup,
                                                const AnimationId animID, 
                                                const float fBlendDelta, 
                                                const int iTaskType,
                                                const char* pTaskName,
                                                const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunAnim::CTaskSimpleRunAnim (  const AssocGroupId animGroup, const AnimationId animID, const float fBlendDelta, const int iTaskType, const char* pTaskName, const bool bHoldLastFrame );");

    // TODO: Find out the real size
    this->CreateTaskInterface ( 1024 );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskSimpleRunAnim__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

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


CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA (  const char* pAnimName,
                                                        const char* pAnimGroupName,
                                                        const int flags,
                                                        const float fBlendDelta,
                                                        const int iTime,
                                                        const bool bDontInterrupt,
                                                        const bool bRunInSequence,
                                                        const bool bOffsetPed,
                                                        const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA (  const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime = -1, const bool bDontInterrupt = false, const bool bRunInSequence = false, const bool bOffsetPed = false, const bool bHoldLastFrame = false )");

    // TODO: Find out the real size
    this->CreateTaskInterface ( sizeof ( CTaskSimpleRunNamedAnimSAInterface ) );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskSimpleRunNamedAnim__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

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


CTaskComplexDieSA::CTaskComplexDieSA ( const eWeaponType eMeansOfDeath,
                                       const AssocGroupId animGroup,
                                       const AnimationId anim, 
                                       const float fBlendDelta,
                                       const float fAnimSpeed,
                                       const bool bBeingKilledByStealth,
                                       const bool bFallingToDeath,
                                       const int iFallToDeathDir,
                                       const bool bFallToDeathOverRailing )
{
    DEBUG_TRACE("CTaskComplexDieSA::CTaskComplexDieSA ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )");

    // TODO: Find out the real size
    this->CreateTaskInterface ( 1024 );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskComplexDie__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

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



CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA ( bool bKiller, CPed * pPed, const AssocGroupId animGroup )
{
    DEBUG_TRACE("CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA ( bool bKiller, class CPed * pPed, const AssocGroupId animGroup )");

    // TODO: Find out the real size
    this->CreateTaskInterface ( 1024 );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskSimpleStealthKill__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwPedInterface = (DWORD)pPed->GetPedInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    animGroup
        push    dwPedInterface
        push    bKiller
        call    dwFunc
    }
}


CTaskComplexSunbatheSA::CTaskComplexSunbatheSA ( CObject* pTowel, const bool bStartStanding )
{
    DEBUG_TRACE("CTaskComplexSunbatheSA::CTaskComplexSunbatheSA ( CObject* pTowel, const bool bStartStanding )");

    // TODO: Find out the real size
    this->CreateTaskInterface ( 1024 );
    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskComplexSunbathe__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwObjectInterface = 0;
    if ( pTowel ) dwObjectInterface = (DWORD)pTowel->GetObjectInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bStartStanding
        push    dwObjectInterface;
        call    dwFunc
    }
}


void CTaskComplexSunbatheSA::SetEndTime ( DWORD dwTime )
{
    CTaskComplexSunbatheSAInterface * thisInterface = (CTaskComplexSunbatheSAInterface*)this->GetInterface();
    thisInterface->m_BathingTimer.dwTimeEnd = dwTime;
}


////////////////////
// Player on foot //
////////////////////
CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA()
{
    DEBUG_TRACE("CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA");
    this->CreateTaskInterface(sizeof(CTaskSimplePlayerOnFootSAInterface));
    if ( !IsValid () ) return;
    DWORD dwFunc = (DWORD)FUNC_CTASKSimplePlayerOnFoot__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface();

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
    DEBUG_TRACE("CTaskComplexFacialSA::CTaskComplexFacialSA");
    this->CreateTaskInterface(sizeof(CTaskComplexFacialSAInterface));
    if ( !IsValid () ) return;
    DWORD dwFunc = (DWORD)FUNC_CTASKComplexFacial__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
    }
}

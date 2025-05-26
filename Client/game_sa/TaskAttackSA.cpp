/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskAttackSA.cpp
 *  PURPOSE:     Attack game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskAttackSA.h"
#include "CEntitySA.h"
#include "CPedSA.h"

CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA(CEntity* pTargetEntity, const CVector* pVecTarget, float fAbortRange, char FrequencyPercentage,
                                                   char nDrivebyStyle, bool bSeatRHS)
{
    CreateTaskInterface(sizeof(CTaskSimpleGangDriveBySAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleGangDriveBy__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    _asm
    {
        mov     ecx, dwThisInterface
        push    bSeatRHS
        push    nDrivebyStyle
        push    FrequencyPercentage
        push    fAbortRange
        push    pVecTarget
        push    dwTargetEntity
        call    dwFunc
    }
}

CTaskSimpleUseGunSA::CTaskSimpleUseGunSA(CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate)
{
    CreateTaskInterface(sizeof(CTaskSimpleUseGunSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    float fTargetX = vecTarget.fX, fTargetY = vecTarget.fY, fTargetZ = vecTarget.fZ;
    DWORD dwBurstLength = nBurstLength;
    _asm
    {
        mov     ecx, dwThisInterface
        push    bAimImmediate
        push    dwBurstLength
        push    nCommand
        push    fTargetZ
        push    fTargetY
        push    fTargetX
        push    dwTargetEntity
        call    dwFunc
    }
}

bool CTaskSimpleUseGunSA::SetPedPosition(CPed* pPed)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetPedPosition;
    DWORD dwThisInterface = (DWORD)GetInterface();

    BYTE* ptr = (BYTE*)dwThisInterface;
    ptr[0x0d] = 2;

    CPedSAInterface* dwPedInterface = (CPedSAInterface*)pPed->GetInterface();

    BYTE currentWeaponSlot = dwPedInterface->bCurrentWeaponSlot;

    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskSimpleUseGunSA::FireGun(CPed* pPed, bool bFlag)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_FireGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    bFlag
        push    dwPedInterface
        call    dwFunc
    }
}

bool CTaskSimpleUseGunSA::ControlGun(CPed* pPed, CEntity* pTargetEntity, char nCommand)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    _asm
    {
        mov     ecx, dwThisInterface
        push    nCommand
        push    dwTargetEntity
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::ControlGunMove(CVector2D* pMoveVec)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGunMove;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    pMoveVec
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskSimpleUseGunSA::Reset(CPed* pPed, CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_Reset;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    float fTargetX = vecTarget.fX, fTargetY = vecTarget.fY, fTargetZ = vecTarget.fZ;
    DWORD dwBurstLength = nBurstLength;
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwBurstLength
        push    nCommand
        push    fTargetZ
        push    fTargetY
        push    fTargetX
        push    dwTargetEntity
        push    dwPedInterface
        call    dwFunc
    }
}

int CTaskSimpleUseGunSA::GetTaskType()
{
    int   iReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetTaskType;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

// If flag is 1 or 2 then do magic stop stuff else set m_nNextCommand to 6
// (pEvent not used)
bool CTaskSimpleUseGunSA::MakeAbortable(CPed* pPed, int iPriority, CEvent const* pEvent)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_MakeAbortable;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    pEvent
        push    iPriority
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::ProcessPed(CPed* pPed)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ProcessPed;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskSimpleUseGunSA::AbortIK(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_AbortIK;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
    }
}

void CTaskSimpleUseGunSA::AimGun(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_AimGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
    }
}

void CTaskSimpleUseGunSA::ClearAnim(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ClearAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
    }
}

signed char CTaskSimpleUseGunSA::GetCurrentCommand()
{
    signed char bReturn;
    DWORD       dwFunc = FUNC_CTaskSimpleUseGun_GetCurrentCommand;
    DWORD       dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetDoneFiring()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetDoneFiring;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsFinished()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsFinished;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::IsLineOfSightBlocked()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_IsLineOfSightBlocked;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsFiring()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsFiring;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsReloading()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsReloading;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetSkipAim()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetSkipAim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CTaskSimpleUseGunSA::PlayerPassiveControlGun()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_PlayerPassiveControlGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskSimpleUseGunSA::RemoveStanceAnims(CPed* pPed, float f)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_RemoveStanceAnims;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    f
        push    dwPedInterface
        call    dwFunc
    }
}

bool CTaskSimpleUseGunSA::RequirePistolWhip(CPed* pPed, CEntity* pTargetEntity)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGun;
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    _asm
    {
        push    dwTargetEntity
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskSimpleUseGunSA::SetBurstLength(short a)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetBurstLength;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    a
        call    dwFunc
    }
}

void CTaskSimpleUseGunSA::SetMoveAnim(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetMoveAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
    }
}

void CTaskSimpleUseGunSA::StartAnim(class CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_StartAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    dwPedInterface
        call    dwFunc
    }
}

void CTaskSimpleUseGunSA::StartCountDown(unsigned char a, bool b)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_StartCountDown;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    b
        push    a
        call    dwFunc
    }
}

CTaskSimpleFightSA::CTaskSimpleFightSA(CEntity* pTargetEntity, int nCommand, unsigned int nIdlePeriod)
{
    CreateTaskInterface(sizeof(CTaskSimpleFightSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleFight__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    _asm
    {
        mov     ecx, dwThisInterface
        push    nIdlePeriod
        push    nCommand
        push    dwTargetEntity
        call    dwFunc
    }
}

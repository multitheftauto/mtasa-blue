/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/TaskAttackSA.cpp
 *  PURPOSE:     Attack game tasks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskAttackSA.h"
#include "CEntitySA.h"
#include "CPedSA.h"

#define FUNC_CTaskSimpleUseGun_ControlGun 0x61e040

CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA(CEntity* pTargetEntity, const CVector* pVecTarget, float fAbortRange, char FrequencyPercentage,
                                                   char nDrivebyStyle, bool bSeatRHS)
{
    CreateTaskInterface(sizeof(CTaskSimpleGangDriveBySAInterface));

    if (!IsValid())
        return;

    DWORD dwFunc = 0x6217d0;
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

    DWORD dwFunc = 0x61de60;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x624ED0;
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
    DWORD dwFunc = 0x61EB10;
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
    bool  bReturn = 0;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x61e0c0;
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
    DWORD dwFunc = 0x624dc0;
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
    int   iReturn = 0;
    DWORD dwFunc = 0x61DF20;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x624E30;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x62A380;
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
    DWORD dwFunc = 0x61DFA0;
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
    DWORD dwFunc = 0x61ED10;
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
    DWORD dwFunc = 0x61E190;
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
    signed char bReturn = 0;
    DWORD       dwFunc = 0x4ABE30;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x61C220;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x61C240;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x61C250;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x509950;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x609340;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x588840;
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
    bool  bReturn = 0;
    DWORD dwFunc = 0x61E0A0;
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
    DWORD dwFunc = 0x61E8E0;
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
    bool  bReturn = 0;
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
    DWORD dwFunc = 0x61C210;
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
    DWORD dwFunc = 0x61E3F0;
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
    DWORD dwFunc = 0x624F30;
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
    DWORD dwFunc = 0x61E160;
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

    DWORD dwFunc = 0x61c470;
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

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
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwTargetEntity, pVecTarget, fAbortRange, FrequencyPercentage, nDrivebyStyle, bSeatRHS);
    // clang-format on
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
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwTargetEntity, fTargetX, fTargetY, fTargetZ, nCommand, dwBurstLength, bAimImmediate);
    // clang-format on
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

    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
    return bReturn;
}

void CTaskSimpleUseGunSA::FireGun(CPed* pPed, bool bFlag)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_FireGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface, bFlag);
    // clang-format on
}

bool CTaskSimpleUseGunSA::ControlGun(CPed* pPed, CEntity* pTargetEntity, char nCommand)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface, dwPedInterface, dwTargetEntity, nCommand);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::ControlGunMove(CVector2D* pMoveVec)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGunMove;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface, pMoveVec);
    // clang-format on
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
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface, dwTargetEntity, fTargetX, fTargetY, fTargetZ, nCommand, dwBurstLength);
    // clang-format on
}

int CTaskSimpleUseGunSA::GetTaskType()
{
    int   iReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetTaskType;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    iReturn = gta_thiscall_address<decltype(iReturn)>(dwFunc, dwThisInterface);
    // clang-format on
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
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface, dwPedInterface, iPriority, pEvent);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::ProcessPed(CPed* pPed)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ProcessPed;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
    return bReturn;
}

void CTaskSimpleUseGunSA::AbortIK(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_AbortIK;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
}

void CTaskSimpleUseGunSA::AimGun(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_AimGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
}

void CTaskSimpleUseGunSA::ClearAnim(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ClearAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
}

signed char CTaskSimpleUseGunSA::GetCurrentCommand()
{
    signed char bReturn;
    DWORD       dwFunc = FUNC_CTaskSimpleUseGun_GetCurrentCommand;
    DWORD       dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetDoneFiring()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetDoneFiring;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsFinished()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsFinished;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::IsLineOfSightBlocked()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_IsLineOfSightBlocked;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsFiring()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsFiring;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetIsReloading()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetIsReloading;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::GetSkipAim()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_GetSkipAim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

bool CTaskSimpleUseGunSA::PlayerPassiveControlGun()
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_PlayerPassiveControlGun;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThisInterface);
    // clang-format on
    return bReturn;
}

void CTaskSimpleUseGunSA::RemoveStanceAnims(CPed* pPed, float f)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_RemoveStanceAnims;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface, f);
    // clang-format on
}

bool CTaskSimpleUseGunSA::RequirePistolWhip(CPed* pPed, CEntity* pTargetEntity)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_ControlGun;
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    // clang-format off
    bReturn = gta_call_address<decltype(bReturn)>(dwFunc, dwPedInterface, dwTargetEntity);
    // clang-format on
    return bReturn;
}

void CTaskSimpleUseGunSA::SetBurstLength(short a)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetBurstLength;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, a);
    // clang-format on
}

void CTaskSimpleUseGunSA::SetMoveAnim(CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetMoveAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
}

void CTaskSimpleUseGunSA::StartAnim(class CPed* pPed)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_StartAnim;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwPedInterface);
    // clang-format on
}

void CTaskSimpleUseGunSA::StartCountDown(unsigned char a, bool b)
{
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_StartCountDown;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, a, b);
    // clang-format on
}

CTaskSimpleFightSA::CTaskSimpleFightSA(CEntity* pTargetEntity, int nCommand, unsigned int nIdlePeriod)
{
    CreateTaskInterface(sizeof(CTaskSimpleFightSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskSimpleFight__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwTargetEntity = (pTargetEntity) ? (DWORD)pTargetEntity->GetInterface() : 0;
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, dwTargetEntity, nCommand, nIdlePeriod);
    // clang-format on
}

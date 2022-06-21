/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskAttackSA.cpp
 *  PURPOSE:     Attack game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA(CEntity* pTargetEntity, const CVector* pVecTarget, float fAbortRange, char FrequencyPercentage,
                                                   char nDrivebyStyle, bool bSeatRHS)
{
    DEBUG_TRACE(
        "CTaskSimpleGangDriveBySA::CTaskSimpleGangDriveBySA ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, "
        "char nDrivebyStyle, bool bSeatRHS )");

    this->CreateTaskInterface(sizeof(CTaskSimpleGangDriveBySAInterface));
    if (!IsValid())
        return;

    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy
    ((void*(__thiscall*)(void*, CEntitySAInterface*, const CVector*, float, char, char, bool))FUNC_CTaskSimpleGangDriveBy__Constructor)(
        GetInterface(), pTargetEntityInterface, pVecTarget, fAbortRange, FrequencyPercentage, nDrivebyStyle, bSeatRHS);
}

CTaskSimpleUseGunSA::CTaskSimpleUseGunSA(CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate)
{
    DEBUG_TRACE(
        "CTaskSimpleUseGunSA::CTaskSimpleUseGunSA ( CEntity *pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate "
        ")");

    this->CreateTaskInterface(sizeof(CTaskSimpleUseGunSAInterface));
    if (!IsValid())
        return;

    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CTaskSimpleUseGun::CTaskSimpleUseGun
    ((void*(__thiscall*)(void*, CEntitySAInterface*, float, float, float, unsigned char, unsigned short, unsigned char))FUNC_CTaskSimpleUseGun__Constructor)(
        GetInterface(), pTargetEntityInterface, vecTarget.fX, vecTarget.fY, vecTarget.fZ, nCommand, nBurstLength, bAimImmediate);
}

bool CTaskSimpleUseGunSA::SetPedPosition(CPed* pPed)
{
    CPedSAInterface* pPedInterface = pPed->GetPedInterface();
    ((BYTE*)pPedInterface)[0x0d] = 2;

    BYTE currentWeaponSlot = pPedInterface->bCurrentWeaponSlot;

    // CTaskSimpleUseGun::SetPedPosition
    return ((bool(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_SetPedPosition)(GetInterface(), pPedInterface);
}

void CTaskSimpleUseGunSA::FireGun(CPed* pPed, bool bFlag)
{
    // CTaskSimpleUseGun::FireGun
    ((void(__thiscall*)(void*, CPedSAInterface*, bool))FUNC_CTaskSimpleUseGun_FireGun)(GetInterface(), pPed->GetPedInterface(), bFlag);
}

bool CTaskSimpleUseGunSA::ControlGun(CPed* pPed, CEntity* pTargetEntity, char nCommand)
{
    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CTaskSimpleUseGun::ControlGun
    return ((bool(__thiscall*)(void*, CPedSAInterface*, CEntitySAInterface*, char))FUNC_CTaskSimpleUseGun_ControlGun)(GetInterface(), pPed->GetPedInterface(),
                                                                                                                      pTargetEntityInterface, nCommand);
}

bool CTaskSimpleUseGunSA::ControlGunMove(CVector2D* pMoveVec)
{
    // CTaskSimpleUseGun::ControlGunMove
    return ((bool(__thiscall*)(void*, CVector2D*))FUNC_CTaskSimpleUseGun_ControlGunMove)(GetInterface(), pMoveVec);
}

void CTaskSimpleUseGunSA::Reset(CPed* pPed, CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength)
{
    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CTaskSimpleUseGun::Reset
    ((void(__thiscall*)(void*, CPedSAInterface*, CEntitySAInterface*, CVector, char, short))FUNC_CTaskSimpleUseGun_Reset)(
        GetInterface(), pPed->GetPedInterface(), pTargetEntityInterface, vecTarget, nCommand, nBurstLength);
}

int CTaskSimpleUseGunSA::GetTaskType()
{
    // CTaskSimpleUseGun::GetTaskType
    return ((int(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetTaskType)(GetInterface());
}

// If flag is 1 or 2 then do magic stop stuff else set m_nNextCommand to 6
// (pEvent not used)
bool CTaskSimpleUseGunSA::MakeAbortable(CPed* pPed, int iPriority, CEvent const* pEvent)
{
    // CTaskSimpleUseGun::MakeAbortable
    return ((bool(__thiscall*)(void*, CPedSAInterface*, int, const CEvent*))FUNC_CTaskSimpleUseGun_MakeAbortable)(GetInterface(), pPed->GetPedInterface(),
                                                                                                                  iPriority, pEvent);
}

bool CTaskSimpleUseGunSA::ProcessPed(CPed* pPed)
{
    // CTaskSimpleUseGun::ProcessPed
    return ((bool(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_ProcessPed)(GetInterface(), pPed->GetPedInterface());
}

void CTaskSimpleUseGunSA::AbortIK(CPed* pPed)
{
    // CTaskSimpleUseGun::AbortIK
    ((void(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_AbortIK)(GetInterface(), pPed->GetPedInterface());
}

void CTaskSimpleUseGunSA::AimGun(CPed* pPed)
{
    // CTaskSimpleUseGun::AimGun
    ((void(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_AimGun)(GetInterface(), pPed->GetPedInterface());
}

void CTaskSimpleUseGunSA::ClearAnim(CPed* pPed)
{
    // CTaskSimpleUseGun::ClearAnim
    ((void(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_ClearAnim)(GetInterface(), pPed->GetPedInterface());
}

signed char CTaskSimpleUseGunSA::GetCurrentCommand()
{
    // CTaskSimpleUseGun::GetCurrentCommand
    return ((signed char(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetCurrentCommand)(GetInterface());
}

bool CTaskSimpleUseGunSA::GetDoneFiring()
{
    // CTaskSimpleUseGun::GetDoneFiring
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetDoneFiring)(GetInterface());
}

bool CTaskSimpleUseGunSA::GetIsFinished()
{
    // CTaskSimpleUseGun::GetIsFinished
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetIsFinished)(GetInterface());
}

bool CTaskSimpleUseGunSA::IsLineOfSightBlocked()
{
    // CTaskSimpleUseGun::IsLineOfSightBlocked
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_IsLineOfSightBlocked)(GetInterface());
}

bool CTaskSimpleUseGunSA::GetIsFiring()
{
    // CTaskSimpleUseGun::GetIsFiring
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetIsFiring)(GetInterface());
}

bool CTaskSimpleUseGunSA::GetIsReloading()
{
    // CTaskSimpleUseGun::GetIsReloading
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetIsReloading)(GetInterface());
}

bool CTaskSimpleUseGunSA::GetSkipAim()
{
    // CTaskSimpleUseGun::GetSkipAim
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_GetSkipAim)(GetInterface());
}

bool CTaskSimpleUseGunSA::PlayerPassiveControlGun()
{
    // CTaskSimpleUseGun::PlayerPassiveControlGun
    return ((bool(__thiscall*)(void*))FUNC_CTaskSimpleUseGun_PlayerPassiveControlGun)(GetInterface());
}

void CTaskSimpleUseGunSA::RemoveStanceAnims(CPed* pPed, float f)
{
    // CTaskSimpleUseGun::RemoveStanceAnims
    ((void(__thiscall*)(void*, CPedSAInterface*, float))FUNC_CTaskSimpleUseGun_RemoveStanceAnims)(GetInterface(), pPed->GetPedInterface(), f);
}

bool CTaskSimpleUseGunSA::RequirePistolWhip(CPed* pPed, CEntity* pTargetEntity)
{
    CPedSAInterface*    pPedInterface = pPed->GetPedInterface();
    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // actually thiscall
    // CTaskSimpleUseGun::ControlGun
    return ((bool(__cdecl*)(CPedSAInterface*, CEntitySAInterface*))FUNC_CTaskSimpleUseGun_ControlGun)(pPedInterface, pTargetEntityInterface);
}

void CTaskSimpleUseGunSA::SetBurstLength(short a)
{
    // CTaskSimpleUseGun::SetBurstLength
    ((void(__thiscall*)(void*, short))FUNC_CTaskSimpleUseGun_SetBurstLength)(GetInterface(), a);
}

void CTaskSimpleUseGunSA::SetMoveAnim(CPed* pPed)
{
    // CTaskSimpleUseGun::SetMoveAnim
    ((void(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_SetMoveAnim)(GetInterface(), pPed->GetPedInterface());
}

void CTaskSimpleUseGunSA::StartAnim(class CPed* pPed)
{
    // CTaskSimpleUseGun::StartAnim
    ((void(__thiscall*)(void*, CPedSAInterface*))FUNC_CTaskSimpleUseGun_StartAnim)(GetInterface(), pPed->GetPedInterface());
}

void CTaskSimpleUseGunSA::StartCountDown(unsigned char a, bool b)
{
    // CTaskSimpleUseGun::StartCountDown
    ((void(__thiscall*)(void*, unsigned char, bool))FUNC_CTaskSimpleUseGun_StartCountDown)(GetInterface(), a, b);
}

CTaskSimpleFightSA::CTaskSimpleFightSA(CEntity* pTargetEntity, int nCommand, unsigned int nIdlePeriod)
{
    DEBUG_TRACE("CTaskSimpleFightSA::CTaskSimpleFightSA ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )");

    this->CreateTaskInterface(sizeof(CTaskSimpleFightSAInterface));
    if (!IsValid())
        return;

    CEntitySAInterface* pTargetEntityInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CTaskSimpleFight::CTaskSimpleFight
    ((void*(__thiscall*)(void*, CEntitySAInterface*, int, unsigned int))FUNC_CTaskSimpleFight__Constructor)(GetInterface(), pTargetEntityInterface, nCommand,
                                                                                                            nIdlePeriod);
}

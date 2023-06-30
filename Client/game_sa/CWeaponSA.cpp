/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CWeaponSA.cpp
 *  PURPOSE:     Weapon class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColPointSA.h"
#include "CGameSA.h"
#include "CPlayerPedSA.h"
#include "CWeaponSA.h"
#include "CWeaponStatManagerSA.h"
#include "CWeaponStatSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CWeaponSA::CWeaponSA(CWeaponSAInterface* pInterface, CPed* pOwner, eWeaponSlot weaponSlot)
    : m_pInterface{pInterface}, m_pOwner{pOwner}, m_weaponSlot{weaponSlot}
{
}

void CWeaponSA::Destroy()
{
    if (!m_pOwner)
    {
        delete m_pInterface;
        delete this;
    }
}

eWeaponType CWeaponSA::GetType()
{
    return m_pInterface->m_eWeaponType;
};

void CWeaponSA::SetType(eWeaponType type)
{
    m_pInterface->m_eWeaponType = type;
}

eWeaponState CWeaponSA::GetState()
{
    return m_pInterface->m_eState;
}

void CWeaponSA::SetState(eWeaponState state)
{
    m_pInterface->m_eState = state;
}

DWORD CWeaponSA::GetAmmoInClip()
{
    return m_pInterface->m_nAmmoInClip;
}

void CWeaponSA::SetAmmoInClip(DWORD dwAmmoInClip)
{
    m_pInterface->m_nAmmoInClip = dwAmmoInClip;
}

DWORD CWeaponSA::GetAmmoTotal()
{
    return m_pInterface->m_nAmmoTotal;
}

void CWeaponSA::SetAmmoTotal(DWORD dwAmmoTotal)
{
    m_pInterface->m_nAmmoTotal = dwAmmoTotal;
}

CPed* CWeaponSA::GetPed()
{
    return m_pOwner;
}

eWeaponSlot CWeaponSA::GetSlot()
{
    return m_weaponSlot;
}

void CWeaponSA::SetAsCurrentWeapon()
{
    m_pOwner->SetCurrentWeaponSlot(m_weaponSlot);
}

CWeaponInfo* CWeaponSA::GetInfo(eWeaponSkill skill)
{
    return pGame->GetWeaponInfo(m_pInterface->m_eWeaponType, skill);
}

void CWeaponSA::Remove()
{
    DWORD dwFunc = 0x73A380;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if (m_pOwner->GetCurrentWeaponSlot() == m_weaponSlot)
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo(m_pInterface->m_eWeaponType);
        if (pInfo)
        {
            int iModel = pInfo->GetModel();
            m_pOwner->RemoveWeaponModel(iModel);
        }
        m_pOwner->SetCurrentWeaponSlot(WEAPONSLOT_TYPE_UNARMED);
    }
}

void CWeaponSA::Initialize(eWeaponType type, unsigned int uiAmmo, CPed* pPed)
{
    DWORD dwPedInterface = 0;
    if (pPed)
        dwPedInterface = (DWORD)pPed->GetInterface();
    uint  uiType = (uint)type;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x73b4a0;
    _asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        push    uiAmmo
        push    uiType
        call    dwFunc
    }
}

void CWeaponSA::Update(CPed* pPed)
{
    // Note: CWeapon::Update is called mainly to check for reload
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x73db40;
    _asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        call    dwFunc
    }
}

bool CWeaponSA::Fire(CEntity* pFiringEntity, CVector* pvecOrigin, CVector* pvecTarget, CEntity* pTargetEntity, CVector* pvec_1, CVector* pvec_2)
{
    bool  bReturn;
    DWORD dwFiringInterface = 0;
    if (pFiringEntity)
        dwFiringInterface = (DWORD)pFiringEntity->GetInterface();
    DWORD dwTargetInterface = 0;
    if (pTargetEntity)
        dwTargetInterface = (DWORD)pTargetEntity->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x742300;
    _asm
    {
        mov     ecx, dwThis
        push    pvec_2
        push    pvec_1
        push    dwTargetInterface
        push    pvecTarget
        push    pvecOrigin
        push    dwFiringInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CWeaponSA::AddGunshell(CEntity* pFiringEntity, CVector* pvecOrigin, CVector2D* pvecDirection, float fSize)
{
    DWORD dwEntityInterface = 0;
    if (pFiringEntity)
        dwEntityInterface = (DWORD)pFiringEntity->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x73a3e0;
    _asm
    {
        mov     ecx, dwThis
        push    fSize
        push    pvecDirection
        push    pvecOrigin
        push    dwEntityInterface
        call    dwFunc
    }
}

void CWeaponSA::DoBulletImpact(CEntity* pFiringEntity, CEntitySAInterface* pEntityInterface, CVector* pvecOrigin, CVector* pvecTarget, CColPoint* pColPoint,
                               int i_1)
{
    DWORD dwEntityInterface = 0;
    if (pFiringEntity)
        dwEntityInterface = (DWORD)pFiringEntity->GetInterface();
    DWORD dwEntityInterface_2 = (DWORD)pEntityInterface;
    DWORD dwColPointInterface = 0;
    if (pColPoint)
        dwColPointInterface = (DWORD)pColPoint->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x73b550;
    _asm
    {
        mov     ecx, dwThis
        push    i_1
        push    dwColPointInterface
        push    pvecTarget
        push    pvecOrigin
        push    dwEntityInterface_2
        push    dwEntityInterface
        call    dwFunc
    }
}

unsigned char CWeaponSA::GenerateDamageEvent(CPed* pPed, CEntity* pResponsible, eWeaponType weaponType, int iDamagePerHit, ePedPieceTypes hitZone, int i_2)
{
    uint  ucReturn = 0;
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwResponsibleInterface = 0;
    if (pResponsible)
        dwResponsibleInterface = (DWORD)pResponsible->GetInterface();
    DWORD dwFunc = 0x73A530;
    _asm
    {
        push    i_2
        push    hitZone
        push    iDamagePerHit
        push    weaponType
        push    dwResponsibleInterface
        push    dwPedInterface
        call    dwFunc
        add     esp, 24
        mov     ucReturn, eax
    }
    return (uchar)ucReturn;
}

bool CWeaponSA::FireBullet(CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget)
{
    if (!pFiringEntity)
        return false;

    switch (GetType())
    {
        case WEAPONTYPE_PISTOL:
        case WEAPONTYPE_PISTOL_SILENCED:
        case WEAPONTYPE_DESERT_EAGLE:
        case WEAPONTYPE_SHOTGUN:
        case WEAPONTYPE_SAWNOFF_SHOTGUN:
        case WEAPONTYPE_SPAS12_SHOTGUN:
        case WEAPONTYPE_MICRO_UZI:
        case WEAPONTYPE_MP5:
        case WEAPONTYPE_AK47:
        case WEAPONTYPE_M4:
        case WEAPONTYPE_TEC9:
        case WEAPONTYPE_COUNTRYRIFLE:
        case WEAPONTYPE_SNIPERRIFLE:
        case WEAPONTYPE_MINIGUN:
        {
            // Don't hit shooter
            pGame->GetWorld()->IgnoreEntity(pFiringEntity);

            // Do pre shot lag compensation
            CPlayerPed* pFiringPlayerPed = dynamic_cast<CPlayerPed*>(pFiringEntity);
            if (pGame->m_pPreWeaponFireHandler && pFiringPlayerPed)
                pGame->m_pPreWeaponFireHandler(pFiringPlayerPed, false);

            // Get the gun muzzle position
            float        fSkill = 999.f;
            CWeaponStat* pCurrentWeaponInfo = pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel(GetType(), fSkill);
            CVector      vecGunMuzzle = *pCurrentWeaponInfo->GetFireOffset();
            if (pFiringPlayerPed)
                pFiringPlayerPed->GetTransformedBonePosition(BONE_RIGHTWRIST, &vecGunMuzzle);

            // Bullet trace
            FireInstantHit(pFiringEntity, &vecOrigin, &vecGunMuzzle, nullptr, &vecTarget, nullptr, false, true);

            // Fire sound
            if (pFiringPlayerPed)
                pFiringPlayerPed->AddWeaponAudioEvent(EPedWeaponAudioEvent::FIRE);

            // Do post shot lag compensation reset & script events
            if (pGame->m_pPostWeaponFireHandler && pFiringPlayerPed)
                pGame->m_pPostWeaponFireHandler();

            pGame->GetWorld()->IgnoreEntity(nullptr);

            return true;
        }

        default:
            break;
    }
    return false;
}

bool CWeaponSA::FireInstantHit(CEntity* pFiringEntity, const CVector* pvecOrigin, const CVector* pvecMuzzle, CEntity* pTargetEntity, const CVector* pvecTarget,
                               const CVector* pvec, bool bFlag1, bool bFlag2)
{
    bool  bReturn;
    DWORD dwEntityInterface = 0;
    if (pFiringEntity)
        dwEntityInterface = (DWORD)pFiringEntity->GetInterface();
    DWORD dwTargetInterface = 0;
    if (pTargetEntity)
        dwTargetInterface = (DWORD)pTargetEntity->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x73FB10;
    _asm
    {
        mov     ecx, dwThis
        push    bFlag2
        push    bFlag1
        push    pvec
        push    pvecTarget
        push    dwTargetInterface
        push    pvecMuzzle
        push    pvecOrigin
        push    dwEntityInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CWeaponSA::ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity,
                                   const SLineOfSightFlags flags, SLineOfSightBuildingResult* pBuildingResult, eWeaponType weaponType,
                                   CEntitySAInterface** pEntity)
{
    DWORD dwFunction = 0x712E40;
    _asm
    {
        push vecEnd
        push vecStart
        call dwFunction
    }
    dwFunction = 0x707550;
    _asm
    {
        push vecEnd
        push vecStart
        call dwFunction
    }
    bool bReturn = pGame->GetWorld()->ProcessLineOfSight(vecStart, vecEnd, colCollision, CollisionEntity, flags, pBuildingResult);
    _asm
    {
        add esp, 10h
    }

    if (bReturn)
    {
        if (*CollisionEntity)
        {
            *pEntity = (*CollisionEntity)->GetInterface();
        }
        else
        {
            if (pBuildingResult->bValid)
                *pEntity = pBuildingResult->pInterface;
        }
    }
    if (*pEntity && (*pEntity)->nType == ENTITY_TYPE_VEHICLE)
    {
        CEntitySAInterface*   pHitInterface = *pEntity;
        CColPointSAInterface* pColPointSAInterface = (*colCollision)->GetInterface();
        DWORD                 dwFunc = FUNC_CWeapon_CheckForShootingVehicleOccupant;
        _asm
        {
            push vecEnd
            push vecStart
            push weaponType
            push pColPointSAInterface
            lea eax, pHitInterface
            push eax
            call dwFunc
            add esp, 14h
        }
    }
    return bReturn;
}

int CWeaponSA::GetWeaponReloadTime(CWeaponStat* pWeaponStat)
{
    CWeaponStatSA* pWeaponStats = (CWeaponStatSA*)pWeaponStat;
    DWORD          dwReturn = 0;
    DWORD          dwFunction = 0x743D70;
    DWORD          dwInterface = (DWORD)pWeaponStats->GetInterface();
    _asm
    {
        mov ecx, dwInterface
        call dwFunction
        mov dwReturn, eax
    }
    return dwReturn;
}

int CWeaponSA::GetWeaponFireTime(CWeaponStat* pWeaponStat)
{
    int   iGlobalTimer = pGame->GetSystemTime();
    float fWeaponFireTime = (pWeaponStat->GetWeaponAnimLoopStop() - pWeaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return (int)fWeaponFireTime;
}

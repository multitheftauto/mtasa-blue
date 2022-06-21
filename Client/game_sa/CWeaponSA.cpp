/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponSA.cpp
 *  PURPOSE:     Weapon class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CWeaponSA::CWeaponSA(CWeaponSAInterface* weaponInterface, CPed* ped, eWeaponSlot weaponSlot)
{
    DEBUG_TRACE("CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, DWORD dwSlot )");
    this->owner = ped;
    this->m_weaponSlot = weaponSlot;
    internalInterface = weaponInterface;
}

void CWeaponSA::Destroy()
{
    if (!owner)
    {
        delete internalInterface;
        delete this;
    }
}

eWeaponType CWeaponSA::GetType()
{
    DEBUG_TRACE("eWeaponType CWeaponSA::GetType(  )");
    return this->internalInterface->m_eWeaponType;
};

VOID CWeaponSA::SetType(eWeaponType type)
{
    DEBUG_TRACE("VOID CWeaponSA::SetType( eWeaponType type )");
    this->internalInterface->m_eWeaponType = type;
}

eWeaponState CWeaponSA::GetState()
{
    DEBUG_TRACE("eWeaponState CWeaponSA::GetState(  )");
    return this->internalInterface->m_eState;
}

void CWeaponSA::SetState(eWeaponState state)
{
    DEBUG_TRACE("void CWeaponSA::SetState ( eWeaponState state )");
    this->internalInterface->m_eState = state;
}

DWORD CWeaponSA::GetAmmoInClip()
{
    DEBUG_TRACE("DWORD CWeaponSA::GetAmmoInClip(  )");
    return this->internalInterface->m_nAmmoInClip;
}

VOID CWeaponSA::SetAmmoInClip(DWORD dwAmmoInClip)
{
    DEBUG_TRACE("VOID CWeaponSA::SetAmmoInClip( DWORD dwAmmoInClip )");
    this->internalInterface->m_nAmmoInClip = dwAmmoInClip;
}

DWORD CWeaponSA::GetAmmoTotal()
{
    DEBUG_TRACE("DWORD CWeaponSA::GetAmmoTotal(  )");
    return this->internalInterface->m_nAmmoTotal;
}

VOID CWeaponSA::SetAmmoTotal(DWORD dwAmmoTotal)
{
    DEBUG_TRACE("VOID CWeaponSA::SetAmmoTotal( DWORD dwAmmoTotal )");
    this->internalInterface->m_nAmmoTotal = dwAmmoTotal;
}

CPed* CWeaponSA::GetPed()
{
    DEBUG_TRACE("CPed * CWeaponSA::GetPed()");
    return (CPed*)owner;
}

eWeaponSlot CWeaponSA::GetSlot()
{
    DEBUG_TRACE("eWeaponSlot CWeaponSA::GetSlot()");
    return m_weaponSlot;
}

VOID CWeaponSA::SetAsCurrentWeapon()
{
    DEBUG_TRACE("VOID CWeaponSA::SetAsCurrentWeapon()");
    owner->SetCurrentWeaponSlot(m_weaponSlot);
}

void CWeaponSA::Remove()
{
    DEBUG_TRACE("void CWeaponSA::Remove ()");

    // CWeapon::Shutdown
    ((void(__thiscall*)(CWeaponSAInterface*))FUNC_Shutdown)(this->internalInterface);

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if (owner->GetCurrentWeaponSlot() == m_weaponSlot)
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo(this->internalInterface->m_eWeaponType);
        if (pInfo)
        {
            int iModel = pInfo->GetModel();
            owner->RemoveWeaponModel(iModel);
        }
        owner->SetCurrentWeaponSlot(WEAPONSLOT_TYPE_UNARMED);
    }
}

void CWeaponSA::Initialize(eWeaponType type, unsigned int uiAmmo, CPed* pPed)
{
    CPedSAInterface* pPedInterface = pPed ? pPed->GetPedInterface() : nullptr;

    // CWeapon::Initialise
    ((void(__thiscall*)(CWeaponSAInterface*, eWeaponType, int, CPedSAInterface*))FUNC_CWeapon_Initialize)(this->internalInterface, type, uiAmmo, pPedInterface);
}

void CWeaponSA::Update(CPed* pPed)
{
    // Note: CWeapon::Update is called mainly to check for reload

    // CWeapon::Update
    ((void(__thiscall*)(CWeaponSAInterface*, CPedSAInterface*))FUNC_CWeapon_Update)(internalInterface, pPed->GetPedInterface());
}

bool CWeaponSA::Fire(CEntity* pFiringEntity, CVector* pvecOrigin, CVector* pvecTarget, CEntity* pTargetEntity, CVector* pvec_1, CVector* pvec_2)
{
    CEntitySAInterface* pFiringInterface = pFiringEntity ? pFiringEntity->GetInterface() : nullptr;
    CEntitySAInterface* pTargetInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CWeapon::Fire
    return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector*, CEntitySAInterface*, CVector*, CVector*))FUNC_CWeapon_Fire)(
        this->internalInterface, pFiringInterface, pvecOrigin, pvecTarget, pTargetInterface, pvec_1, pvec_2);
}

void CWeaponSA::AddGunshell(CEntity* pFiringEntity, CVector* pvecOrigin, CVector2D* pvecDirection, float fSize)
{
    CEntitySAInterface* pFiringInterface = pFiringEntity ? pFiringEntity->GetInterface() : nullptr;

    // CWeapon::AddGunshell
    ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector&, CVector2D&, float))FUNC_CWeapon_AddGunshell)(
        this->internalInterface, pFiringInterface, *pvecOrigin, *pvecDirection, fSize);
}

void CWeaponSA::DoBulletImpact(CEntity* pFiringEntity, CEntitySAInterface* pEntityInterface, CVector* pvecOrigin, CVector* pvecTarget, CColPoint* pColPoint,
                               int i_1)
{
    CEntitySAInterface*   pFiringInterface = pFiringEntity ? pFiringEntity->GetInterface() : nullptr;
    CColPointSAInterface* pColPointInterface = pColPoint ? pColPoint->GetInterface() : nullptr;

    // CWeapon::DoBulletImpact
    ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CEntitySAInterface*, CVector*, CVector*, CColPointSAInterface*,
                        int))FUNC_CWeapon_DoBulletImpact)(this->internalInterface, pFiringInterface, pEntityInterface, pvecOrigin, pvecTarget,
                                                          pColPointInterface, i_1);
}

unsigned char CWeaponSA::GenerateDamageEvent(CPed* pPed, CEntity* pResponsible, eWeaponType weaponType, int iDamagePerHit, ePedPieceTypes hitZone, int i_2)
{
    CPedSAInterface*    pPedInterface = pPed->GetPedInterface();
    CEntitySAInterface* pResponsibleInterface = pResponsible ? pResponsible->GetInterface() : nullptr;

    // CWeapon::GenerateDamageEvent
    return ((unsigned char(__cdecl*)(CPedSAInterface*, CEntitySAInterface*, eWeaponType, int, ePedPieceTypes, int))FUNC_CWeapon_GenerateDamageEvent)(
        pPedInterface, pResponsibleInterface, weaponType, iDamagePerHit, hitZone, i_2);
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
            FireInstantHit(pFiringEntity, &vecOrigin, &vecGunMuzzle, NULL, &vecTarget, NULL, false, true);

            // Fire sound
            if (pFiringPlayerPed)
                pFiringPlayerPed->AddWeaponAudioEvent(EPedWeaponAudioEvent::FIRE);

            // Do post shot lag compensation reset & script events
            if (pGame->m_pPostWeaponFireHandler && pFiringPlayerPed)
                pGame->m_pPostWeaponFireHandler();

            pGame->GetWorld()->IgnoreEntity(NULL);

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
    CEntitySAInterface* pFiringInterface = pFiringEntity ? pFiringEntity->GetInterface() : nullptr;
    CEntitySAInterface* pTargetInterface = pTargetEntity ? pTargetEntity->GetInterface() : nullptr;

    // CWeapon::FireInstantHit
    return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, const CVector*, const CVector*, CEntitySAInterface*, const CVector*, const CVector*,
                               bool, bool))FUNC_CWeapon_FireInstantHit)(internalInterface, pFiringInterface, pvecOrigin, pvecMuzzle, pTargetInterface,
                                                                        pvecTarget, pvec, bFlag1, bFlag2);
}

bool CWeaponSA::ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity,
                                   const SLineOfSightFlags flags, SLineOfSightBuildingResult* pBuildingResult, eWeaponType weaponType,
                                   CEntitySAInterface** pEntity)
{
    // CBirds::HandleGunShot
    ((void(__cdecl*)(const CVector*, const CVector*))FUNC_CBirds_CheckForHit)(vecStart, vecEnd);

    // CShadows::GunShotSetsOilOnFire
    ((void(__cdecl*)(const CVector*, const CVector*))FUNC_CShadows_CheckForHit)(vecStart, vecEnd);

    bool bReturn = g_pCore->GetGame()->GetWorld()->ProcessLineOfSight(vecStart, vecEnd, colCollision, CollisionEntity, flags, pBuildingResult);

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
        // CWeapons::CheckForShootingVehicleOccupant
        ((void(__cdecl*)(CEntitySAInterface**, CColPointSAInterface*, eWeaponType, const CVector*,
                         const CVector*))FUNC_CWeapon_CheckForShootingVehicleOccupant)(pEntity, (*colCollision)->GetInterface(), weaponType, vecStart, vecEnd);
    }
    return bReturn;
}

int CWeaponSA::GetWeaponReloadTime(CWeaponStat* pWeaponStat)
{
    CWeaponStatSA* pWeaponStats = (CWeaponStatSA*)pWeaponStat;

    // CWeaponInfo::GetWeaponReloadTime
    return ((int(__thiscall*)(CWeaponInfoSAInterface*))FUNC_CWeaponInfo_GetWeaponReloadTime)(pWeaponStats->GetInterface());
}

int CWeaponSA::GetWeaponFireTime(CWeaponStat* pWeaponStat)
{
    int   iGlobalTimer = pGame->GetSystemTime();
    float fWeaponFireTime = (pWeaponStat->GetWeaponAnimLoopStop() - pWeaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return (int)fWeaponFireTime;
}

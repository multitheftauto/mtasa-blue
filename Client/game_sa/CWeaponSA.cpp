/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponSA.cpp
 *  PURPOSE:     Weapon class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CPlayerPedSA.h"
#include "CWeaponSA.h"
#include "CWeaponStatManagerSA.h"
#include "CWeaponStatSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CWeaponInfo* CWeaponSA::GetInfo(const eWeaponSkill& skill) const
{
    return m_interface ? pGame->GetWeaponInfo(m_interface->m_eWeaponType, skill) : nullptr;
}

void CWeaponSA::SetAsCurrentWeapon()
{
    if (m_owner)
        m_owner->SetCurrentWeaponSlot(m_weaponSlot);
}

void CWeaponSA::Destroy()
{
    if (m_interface)
        delete m_interface;

    delete this;
}

void CWeaponSA::Remove()
{
    if (!m_interface)
        return;

    m_interface->Shutdown();

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if (m_owner && m_owner->GetCurrentWeaponSlot() == m_weaponSlot)
    {
        if (CWeaponInfo* info = pGame->GetWeaponInfo(m_interface->m_eWeaponType))
            m_owner->RemoveWeaponModel(info->GetModel());

        m_owner->SetCurrentWeaponSlot(WEAPONSLOT_TYPE_UNARMED);
    }
}

void CWeaponSA::Initialize(const eWeaponType& type, std::uint32_t ammo, CPed* ped)
{
    if (m_interface)
        m_interface->Initialize(type, ammo, ped);
}

void CWeaponSA::Update(CPed* ped)
{
    // Note: CWeapon::Update is called mainly to check for reload
    if (m_interface)
        m_interface->Update(ped);
}

void CWeaponSA::AddGunshell(CEntity* firingEntity, const CVector& vecOrigin, const CVector2D& vecDirection, float size) const
{
    if (m_interface && firingEntity)
        m_interface->AddGunshell(firingEntity, vecOrigin, vecDirection, size);
}

void CWeaponSA::DoBulletImpact(CEntity* firingEntity, CEntitySAInterface* hitEntityInterface, const CVector& vecOrigin, const CVector& vecTarget, const CColPoint& colPoint, int incrementalHit) const
{
    if (m_interface)
        m_interface->DoBulletImpact(firingEntity, hitEntityInterface, vecOrigin, vecTarget, colPoint, incrementalHit);
}

bool CWeaponSA::Fire(CEntity* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntity* targetEntity, CVector* vecTarget, CVector* vecAlt)
{
    if (!firingEntity)
        return false;

    return m_interface ? m_interface->Fire(firingEntity, vecOrigin, vecEffectPos, targetEntity, vecTarget, vecAlt) : false;
}

bool CWeaponSA::FireInstantHit(CEntity* firingEntity, const CVector* vecOrigin, const CVector* vecMuzzle, CEntity* targetEntity, const CVector* vecTarget, const CVector* vecForDriveby, bool crossHairGun, bool createGunFx)
{
    return m_interface ? m_interface->FireInstantHit(firingEntity, const_cast<CVector*>(vecOrigin), const_cast<CVector*>(vecMuzzle), targetEntity, const_cast<CVector*>(vecTarget), const_cast<CVector*>(vecForDriveby), crossHairGun, createGunFx) : false;
}

bool CWeaponSA::FireBullet(CEntity* firingEntity, const CVector& vecOrigin, const CVector& vecTarget)
{
    if (!firingEntity)
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
            pGame->GetWorld()->IgnoreEntity(firingEntity);

            // Do pre shot lag compensation
            CPlayerPed* firingPlayerPed = dynamic_cast<CPlayerPed*>(firingEntity);
            if (pGame->m_pPreWeaponFireHandler && firingPlayerPed)
                pGame->m_pPreWeaponFireHandler(firingPlayerPed, false);

            // Get the gun muzzle position
            CWeaponStat* currentWeaponInfo = pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel(GetType(), 999.0f);
            CVector      vecGunMuzzle = *currentWeaponInfo->GetFireOffset();
            if (firingPlayerPed)
                firingPlayerPed->GetTransformedBonePosition(BONE_RIGHTWRIST, &vecGunMuzzle);

            // Bullet trace
            FireInstantHit(firingEntity, &vecOrigin, &vecGunMuzzle, nullptr, &vecTarget, nullptr, false, true);

            // Fire sound
            if (firingPlayerPed)
                firingPlayerPed->AddWeaponAudioEvent(EPedWeaponAudioEventType::FIRE);

            // Do post shot lag compensation reset & script events
            if (pGame->m_pPostWeaponFireHandler && firingPlayerPed)
                pGame->m_pPostWeaponFireHandler();

            pGame->GetWorld()->IgnoreEntity(nullptr);
            return true;
        }

        default:
            break;
    }

    return false;
}

bool CWeaponSA::GenerateDamageEvent(CPed* ped, CEntity* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone, std::uint8_t dir) const
{
    if (!ped || !m_interface)
        return false;

    return m_interface->GenerateDamageEvent(ped, responsible, weaponType, damagePerHit, hitZone, dir);
}

bool CWeaponSA::ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** collisionEntity, const SLineOfSightFlags& flags, SLineOfSightBuildingResult* buildingResult, const eWeaponType& weaponType, CEntitySAInterface** entity)
{
    // Call CBirds::HandleGunShot
    ((void(__cdecl*)(CVector*, CVector*))FUNC_CBirds_CheckForHit)(const_cast<CVector*>(vecStart), const_cast<CVector*>(vecEnd));

    // Call CShadows::GunShotSetsOilOnFire
    ((void(__cdecl*)(CVector*, CVector*))FUNC_CShadows_CheckForHit)(const_cast<CVector*>(vecStart), const_cast<CVector*>(vecEnd));

    bool hit = pGame->GetWorld()->ProcessLineOfSight(vecStart, vecEnd, colCollision, collisionEntity, flags, buildingResult);

    if (hit)
    {
        if (*collisionEntity)
            *entity = (*collisionEntity)->GetInterface();
        else
        {
            if (buildingResult->bValid)
                *entity = buildingResult->pInterface;
        }
    }

    // Call CWeapon::CheckForShootingVehicleOccupant
    if (*entity && (*entity)->nType == ENTITY_TYPE_VEHICLE)
        ((void(__cdecl*)(CEntitySAInterface*, CColPointSAInterface*, eWeaponType, CVector*, CVector*))FUNC_CWeapon_CheckForShootingVehicleOccupant)(*entity, (*colCollision)->GetInterface(), weaponType, const_cast<CVector*>(vecStart), const_cast<CVector*>(vecEnd));

    return hit;
}

int CWeaponSA::GetWeaponReloadTime(CWeaponStat* weaponStat) const
{
    auto* weaponStats = static_cast<CWeaponStatSA*>(weaponStat);
    if (CWeaponInfoSAInterface* statInterface = weaponStats->GetInterface())
        return statInterface->GetWeaponReloadTime();

    return 0;
}

int CWeaponSA::GetWeaponFireTime(CWeaponStat* weaponStat)
{
    std::uint32_t timer = pGame->GetSystemTime();
    float weaponFireTime = (weaponStat->GetWeaponAnimLoopStop() - weaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return static_cast<int>(weaponFireTime);
}

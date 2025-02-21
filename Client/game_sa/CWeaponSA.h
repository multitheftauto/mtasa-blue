/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponSA.h
 *  PURPOSE:     Header file for weapon class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWeapon.h>
#include <game/CPed.h>

#define FUNC_CWeapon_Shutdown                           0x73A380
#define FUNC_CWeapon_CheckForShootingVehicleOccupant    0x73f480
#define FUNC_CWeapon_Initialize                         0x73b4a0
#define FUNC_CWeapon_Update                             0x73db40
#define FUNC_CWeapon_Fire                               0x742300
#define FUNC_CWeapon_AddGunshell                        0x73a3e0
#define FUNC_CWeapon_DoBulletImpact                     0x73b550
#define FUNC_CWeapon_GenerateDamageEvent                0x73a530
#define FUNC_CWeapon_FireInstantHit                     0x73FB10
#define FUNC_CBirds_CheckForHit                         0x712E40
#define FUNC_CShadows_CheckForHit                       0x707550

class CWeaponSAInterface
{
public:
    eWeaponType   m_eWeaponType{};
    eWeaponState  m_eState{};
    std::uint32_t m_ammoInClip{0};
    std::uint32_t m_ammoTotal{0};
    std::uint32_t m_timeToNextShootInMS{0};
    bool          m_firstPersonEnabled{false}; // Unused
    bool          m_dontPlaceInHand{false}; // Used in case of goggles
    FxSystem_c*   m_fxSystem{nullptr}; // Fx system (flamethrower, spraycan, extinguisher)

    void Shutdown() { ((void(__thiscall*)(CWeaponSAInterface*))FUNC_CWeapon_Shutdown)(this); }
    void Initialize(const eWeaponType& weaponType, std::uint32_t ammo, CPed* ped) { ((void(__thiscall*)(CWeaponSAInterface*, eWeaponType, std::uint32_t, CPedSAInterface*))FUNC_CWeapon_Initialize)(this, weaponType, ammo, ped ? ped->GetPedInterface() : nullptr); }
    void Update(CPed* ped) { ((void(__thiscall*)(CWeaponSAInterface*, CPedSAInterface*))FUNC_CWeapon_Update)(this, ped ? ped->GetPedInterface() : nullptr); }
    void AddGunshell(CEntity* firingEntity, CVector* vecOrigin, CVector2D* vecDirection, float size) { ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector2D*, float))FUNC_CWeapon_AddGunshell)(this, firingEntity ? firingEntity->GetInterface() : nullptr, vecOrigin, vecDirection, size); }
    void DoBulletImpact(CEntity* firingEntity, CEntitySAInterface* hitEntityInterface, CVector* vecOrigin, CVector* vecTarget, CColPoint* colPoint, int incrementalHit) { ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CEntitySAInterface*, CVector*, CVector*, CColPoint*, int))FUNC_CWeapon_DoBulletImpact)(this, firingEntity ? firingEntity->GetInterface() : nullptr, hitEntityInterface, vecOrigin, vecTarget, colPoint, incrementalHit); }
    bool Fire(CEntity* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntity* targetEntity, CVector* vecTarget, CVector* vecAlt) { return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector*, CEntitySAInterface*, CVector*, CVector*))FUNC_CWeapon_Fire)(this, firingEntity ? firingEntity->GetInterface() : nullptr, vecOrigin, vecEffectPos, targetEntity ? targetEntity->GetInterface() : nullptr, vecTarget, vecAlt); }
    bool FireInstantHit(CEntity* firingEntity, CVector* vecOrigin, CVector* vecMuzzle, CEntity* targetEntity, CVector* vecTarget, CVector* vecForDriveby, bool crossHairGun, bool createGunFx) { return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector*, CEntitySAInterface*, CVector*, CVector*, bool, bool))FUNC_CWeapon_FireInstantHit)(this, firingEntity ? firingEntity->GetInterface() : nullptr, vecOrigin, vecMuzzle, targetEntity ? targetEntity->GetInterface() : nullptr, vecTarget, vecForDriveby, crossHairGun, createGunFx); }
    bool GenerateDamageEvent(CPed* ped, CEntity* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone, std::uint8_t dir) { return ((bool(__thiscall*)(CWeaponSAInterface*, CPedSAInterface*, CEntitySAInterface*, eWeaponType, int, ePedPieceTypes, std::uint8_t))FUNC_CWeapon_GenerateDamageEvent)(this, ped ? ped->GetPedInterface() : nullptr, responsible ? responsible->GetInterface() : nullptr, weaponType, damagePerHit, hitZone, dir); }
};
static_assert(sizeof(CWeaponSAInterface) == 0x1C, "Invalid size for CWeaponSAInterface");

class CWeaponSA : public CWeapon
{
public:
    CWeaponSA(CWeaponSAInterface* weaponInterface, CPed* owner, eWeaponSlot weaponSlot) : m_interface(weaponInterface), m_owner(owner), m_weaponSlot(weaponSlot) {}
    CWeaponSAInterface* GetInterface() { return m_interface; }
    CWeaponSAInterface* GetInterface() const { return m_interface; }

    eWeaponType GetType() const override { return m_interface ? m_interface->m_eWeaponType : eWeaponType::WEAPONTYPE_UNIDENTIFIED; }
    void        SetType(const eWeaponType& type) override { if (m_interface) m_interface->m_eWeaponType = type; }

    eWeaponState GetState() const override { return m_interface ? m_interface->m_eState : eWeaponState::WEAPONSTATE_READY; }
    void         SetState(const eWeaponState& state) override { if (m_interface) m_interface->m_eState = state; }

    std::uint32_t GetAmmoInClip() const override { return m_interface ? m_interface->m_ammoInClip : 0; }
    void          SetAmmoInClip(std::uint32_t ammoInClip) override { if (m_interface) m_interface->m_ammoInClip = ammoInClip; }

    std::uint32_t GetAmmoTotal() const override { return m_interface ? m_interface->m_ammoTotal : 0; }
    void          SetAmmoTotal(std::uint32_t ammoTotal) override { if (m_interface) m_interface->m_ammoTotal = ammoTotal; }

    CPed* GetPed() const noexcept override { return m_owner; }
    eWeaponSlot GetSlot() const noexcept override { return m_weaponSlot; }
    CWeaponInfo* GetInfo(const eWeaponSkill& skill) const override;

    void SetAsCurrentWeapon() override;

    void Destroy() override;
    void Remove() override;
    void Initialize(const eWeaponType& type, std::uint32_t ammo, CPed* ped) override;
    void Update(CPed* ped) override;

    void AddGunshell(CEntity* firingEntity, CVector* vecOrigin, CVector2D* vecDirection, float size) const override;
    void DoBulletImpact(CEntity* firingEntity, CEntitySAInterface* hitEntityInterface, CVector* vecOrigin, CVector* vecTarget, CColPoint* colPoint, int incrementalHit) const override;
    bool Fire(CEntity* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntity* targetEntity, CVector* vecTarget, CVector* vecAlt) override;
    bool FireInstantHit(CEntity* firingEntity, const CVector* vecOrigin, const CVector* vecMuzzle, CEntity* targetEntity, const CVector* vecTarget, const CVector* vecForDriveby, bool crossHairGun, bool createGunFx) override;
    bool FireBullet(CEntity* firingEntity, const CVector& vecOrigin, const CVector& vecTarget) override;

    bool GenerateDamageEvent(CPed* ped, CEntity* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone, std::uint8_t dir) const override;

    bool ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** collisionEntity, const SLineOfSightFlags& flags, SLineOfSightBuildingResult* buildingResult, const eWeaponType& weaponType, CEntitySAInterface** entity) override;

    int  GetWeaponReloadTime(CWeaponStat* weaponStat) const override;
    static int GetWeaponFireTime(CWeaponStat* weaponStat);
    
private:
    CWeaponSAInterface* m_interface{nullptr};
    CPed*               m_owner{nullptr};
    eWeaponSlot         m_weaponSlot{};
};

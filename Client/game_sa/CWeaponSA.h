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

#define FUNC_CWeapon_CheckForShootingVehicleOccupant 0x73f480
#define FUNC_CBirds_CheckForHit                      0x712E40
#define FUNC_CShadows_CheckForHit                    0x707550

class CWeaponSAInterface
{
public:
    eWeaponType   m_eWeaponType{};
    eWeaponState  m_eState{};
    std::uint32_t m_ammoInClip{0};
    std::uint32_t m_ammoTotal{0};
    std::uint32_t m_timeToNextShootInMS{0};
    bool          m_firstPersonEnabled{false};            // Unused
    bool          m_dontPlaceInHand{false};               // Used in case of goggles
    FxSystem_c*   m_fxSystem{nullptr};                    // Fx system (flamethrower, spraycan, extinguisher)

    void Shutdown() { ((void(__thiscall*)(CWeaponSAInterface*))0x73A380)(this); }
    void Initialize(eWeaponType weaponType, std::uint32_t ammo, CPedSAInterface* ped)
    {
        ((void(__thiscall*)(CWeaponSAInterface*, eWeaponType, std::uint32_t, CPedSAInterface*))0x73B4A0)(this, weaponType, ammo, ped);
    }
    void AddGunshell(CEntitySAInterface* firingEntity, const CVector& vecOrigin, const CVector2D& vecDirection, float size)
    {
        ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, const CVector&, const CVector2D&, float))0x73A3E0)(this, firingEntity, vecOrigin,
                                                                                                                         vecDirection, size);
    }
    void DoBulletImpact(CEntitySAInterface* firingEntity, CEntitySAInterface* hitEntityInterface, const CVector& vecOrigin, const CVector& vecTarget,
                        const CColPointSAInterface& colPoint, int incrementalHit)
    {
        ((void(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CEntitySAInterface*, const CVector&, const CVector&, const CColPointSAInterface&,
                            int))0x73B550)(this, firingEntity, hitEntityInterface, vecOrigin, vecTarget, colPoint, incrementalHit);
    }
    bool Fire(CEntitySAInterface* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntitySAInterface* targetEntity, CVector* vecTarget,
              CVector* vecAlt)
    {
        return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector*, CEntitySAInterface*, CVector*, CVector*))0x742300)(
            this, firingEntity, vecOrigin, vecEffectPos, targetEntity, vecTarget, vecAlt);
    }
    bool FireInstantHit(CEntitySAInterface* firingEntity, CVector* vecOrigin, CVector* vecMuzzle, CEntitySAInterface* targetEntity, CVector* vecTarget,
                        CVector* vecForDriveby, bool crossHairGun, bool createGunFx)
    {
        return ((bool(__thiscall*)(CWeaponSAInterface*, CEntitySAInterface*, CVector*, CVector*, CEntitySAInterface*, CVector*, CVector*, bool, bool))0x73FB10)(
            this, firingEntity, vecOrigin, vecMuzzle, targetEntity, vecTarget, vecForDriveby, crossHairGun, createGunFx);
    }
    bool GenerateDamageEvent(CPedSAInterface* ped, CEntitySAInterface* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone,
                             std::uint8_t dir)
    {
        return ((bool(__thiscall*)(CWeaponSAInterface*, CPedSAInterface*, CEntitySAInterface*, eWeaponType, int, ePedPieceTypes, std::uint8_t))0x73A530)(
            this, ped, responsible, weaponType, damagePerHit, hitZone, dir);
    }
};
static_assert(sizeof(CWeaponSAInterface) == 0x1C, "Invalid size for CWeaponSAInterface");

class CWeaponSA : public CWeapon
{
public:
    CWeaponSA(CWeaponSAInterface* weaponInterface, CPed* owner, eWeaponSlot weaponSlot) : m_interface(weaponInterface), m_owner(owner), m_weaponSlot(weaponSlot)
    {
    }
    CWeaponSAInterface* GetInterface() { return m_interface; }
    CWeaponSAInterface* GetInterface() const { return m_interface; }

    eWeaponType GetType() const override { return m_interface ? m_interface->m_eWeaponType : eWeaponType::WEAPONTYPE_UNIDENTIFIED; }
    void        SetType(eWeaponType type) override
    {
        if (m_interface)
            m_interface->m_eWeaponType = type;
    }

    eWeaponState GetState() const override { return m_interface ? m_interface->m_eState : eWeaponState::WEAPONSTATE_READY; }
    void         SetState(eWeaponState state) override
    {
        if (m_interface)
            m_interface->m_eState = state;
    }

    std::uint32_t GetAmmoInClip() const override { return m_interface ? m_interface->m_ammoInClip : 0; }
    void          SetAmmoInClip(std::uint32_t ammoInClip) override
    {
        if (m_interface)
            m_interface->m_ammoInClip = ammoInClip;
    }

    std::uint32_t GetAmmoTotal() const override { return m_interface ? m_interface->m_ammoTotal : 0; }
    void          SetAmmoTotal(std::uint32_t ammoTotal) override
    {
        if (m_interface)
            m_interface->m_ammoTotal = ammoTotal;
    }

    CPed*        GetPed() const noexcept override { return m_owner; }
    eWeaponSlot  GetSlot() const noexcept override { return m_weaponSlot; }
    CWeaponInfo* GetInfo(eWeaponSkill skill) const override;

    void SetAsCurrentWeapon() override;

    void Destroy() override;
    void Remove() override;
    void Initialize(eWeaponType type, std::uint32_t ammo, CPed* ped) override;

    void AddGunshell(CEntity* firingEntity, const CVector& vecOrigin, const CVector2D& vecDirection, float size) const override;
    void DoBulletImpact(CEntity* firingEntity, CEntitySAInterface* hitEntityInterface, const CVector& vecOrigin, const CVector& vecTarget,
                        const CColPointSAInterface& colPoint, int incrementalHit) const override;
    bool Fire(CEntity* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntity* targetEntity, CVector* vecTarget, CVector* vecAlt) override;
    bool FireInstantHit(CEntity* firingEntity, CVector* vecOrigin, CVector* vecMuzzle, CEntity* targetEntity, CVector* vecTarget, CVector* vecForDriveby,
                        bool crossHairGun, bool createGunFx) override;
    bool FireBullet(CEntity* firingEntity, const CVector& vecOrigin, const CVector& vecTarget) override;

    bool GenerateDamageEvent(CPed* ped, CEntity* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone,
                             std::uint8_t dir) const override;

    bool ProcessLineOfSight(const CVector& vecStart, const CVector& vecEnd, CColPoint** colCollision, CEntity*& collisionEntity, const SLineOfSightFlags& flags,
                            SLineOfSightBuildingResult* buildingResult, eWeaponType weaponType, CEntitySAInterface** entity) override;

    int        GetWeaponReloadTime(CWeaponStat* weaponStat) const override;
    static int GetWeaponFireTime(CWeaponStat* weaponStat);

private:
    CWeaponSAInterface* m_interface{nullptr};
    CPed*               m_owner{nullptr};
    eWeaponSlot         m_weaponSlot{};
};

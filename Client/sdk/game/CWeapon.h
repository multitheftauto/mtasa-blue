/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeapon.h
 *  PURPOSE:     Weapon entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CColPoint;
class CColPointSAInterface;
class CPed;
class CVector;
class CVector2D;
class CWeaponInfo;
class CWeaponStat;
enum ePedPieceTypes;
struct SLineOfSightBuildingResult;
struct SLineOfSightFlags;

class CWeapon
{
public:
    virtual eWeaponType GetType() const = 0;
    virtual void        SetType(eWeaponType type) = 0;

    virtual eWeaponState GetState() const = 0;
    virtual void         SetState(eWeaponState state) = 0;

    virtual std::uint32_t GetAmmoInClip() const = 0;
    virtual void          SetAmmoInClip(std::uint32_t ammoInClip) = 0;

    virtual std::uint32_t GetAmmoTotal() const = 0;
    virtual void          SetAmmoTotal(std::uint32_t ammoTotal) = 0;

    virtual CPed*        GetPed() const noexcept = 0;
    virtual eWeaponSlot  GetSlot() const noexcept = 0;
    virtual CWeaponInfo* GetInfo(eWeaponSkill skill) const = 0;

    virtual void SetAsCurrentWeapon() = 0;

    virtual void Destroy() = 0;
    virtual void Remove() = 0;
    virtual void Initialize(eWeaponType type, std::uint32_t ammo, CPed* ped) = 0;

    virtual void AddGunshell(CEntity* firingEntity, const CVector& vecOrigin, const CVector2D& vecDirection, float size) const = 0;
    virtual void DoBulletImpact(CEntity* firingEntity, CEntitySAInterface* hitEntityInterface, const CVector& vecOrigin, const CVector& vecTarget,
                                const CColPointSAInterface& colPoint, int incrementalHit) const = 0;
    virtual bool Fire(CEntity* firingEntity, CVector* vecOrigin, CVector* vecEffectPos, CEntity* targetEntity, CVector* vecTarget, CVector* vecAlt) = 0;
    virtual bool FireInstantHit(CEntity* firingEntity, CVector* vecOrigin, CVector* vecMuzzle, CEntity* targetEntity, CVector* vecTarget,
                                CVector* vecForDriveby, bool crossHairGun, bool createGunFx) = 0;
    virtual bool FireBullet(CEntity* firingEntity, const CVector& vecOrigin, const CVector& vecTarget) = 0;

    virtual bool GenerateDamageEvent(CPed* ped, CEntity* responsible, eWeaponType weaponType, int damagePerHit, ePedPieceTypes hitZone,
                                     std::uint8_t dir) const = 0;

    virtual bool ProcessLineOfSight(const CVector& vecStart, const CVector& vecEnd, CColPoint** colCollision, CEntity*& collisionEntity,
                                    const SLineOfSightFlags& flags, SLineOfSightBuildingResult* buildingResult, eWeaponType weaponType,
                                    CEntitySAInterface** entity) = 0;

    virtual int GetWeaponReloadTime(CWeaponStat* weaponStat) const = 0;
};

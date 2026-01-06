/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectileInfo.h
 *  PURPOSE:     Projectile entity information interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEntity;
class CProjectile;
class CVector;

using ProjectileHandler = bool(*)(CEntity* creator, CProjectile* projectile, CProjectileInfo* projectileInfo, eWeaponType projectileType, CVector* pos, float force, CVector* target, CEntity* targetEntity);
using GameProjectileDestructHandler = void(CEntitySAInterface* entity);

class CProjectileInfo
{
public:
    virtual CProjectileInfo* AddProjectile(CEntity* creator, eWeaponType weapon, CVector origin, float force, CVector* target, CEntity* targetEntity) const = 0;
    virtual CProjectile*     GetProjectileObject() = 0;

    virtual void RemoveProjectile(CProjectileInfo* projectileInfo, CProjectile* projectile, bool blow = true) const = 0;

    virtual void RemoveEntityReferences(CEntity* entity) = 0;

    virtual CEntity* GetTarget() = 0;
    virtual void     SetTarget(CEntity* pEntity) = 0;

    virtual bool IsActive() = 0;

    virtual void  SetCounter(DWORD dwCounter) = 0;
    virtual DWORD GetCounter() = 0;

    virtual eWeaponType GetType() const = 0;

    virtual CProjectileInfo* GetProjectileInfo(std::uint32_t Index) const noexcept = 0;

    virtual void SetProjectileCreationHandler(ProjectileHandler handler) = 0;
    virtual void SetGameProjectileDestructHandler(GameProjectileDestructHandler* handler) = 0;
};

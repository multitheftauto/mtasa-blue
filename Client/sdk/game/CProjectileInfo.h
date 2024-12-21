/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectileInfo.h
 *  PURPOSE:     Projectile entity information interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEntity;
class CPlayerPed;
class CProjectile;
class CVector;

class CProjectileInfo
{
public:
    virtual bool             AddProjectile(CEntity* creator, eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity) = 0;
    virtual CProjectile*     GetProjectile(void* projectilePointer) = 0;                      // hack, don't use please
    virtual CProjectileInfo* GetProjectileInfo(void* projectileInfoInterface) = 0;            // don't use
    virtual void             RemoveProjectile(CProjectileInfo* pProjectileInfo, CProjectile* pProjectile, bool bBlow = true) = 0;
    virtual CProjectileInfo* GetProjectileInfo(DWORD Index) = 0;
    virtual void             RemoveEntityReferences(CEntity* entity) = 0;

    virtual CEntity* GetTarget() = 0;
    virtual void     SetTarget(CEntity* pEntity) = 0;

    virtual bool IsActive() = 0;

    virtual void  SetCounter(DWORD dwCounter) = 0;
    virtual DWORD GetCounter() = 0;
};

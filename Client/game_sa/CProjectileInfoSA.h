/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CProjectileInfoSA.h
 *  PURPOSE:     Header file for projectile type information class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectileInfo.h>
#include "CProjectileSA.h"

#define PROJECTILE_INFO_COUNT               32
#define ARRAY_CProjectileInfo               0xC891A8

// #pragma pack(push,1)
class CProjectileInfoSAInterface
{
public:
    eWeaponType         dwProjectileType;
    CEntitySAInterface* pEntProjectileOwner;
    CEntitySAInterface* pEntProjectileTarget;
    DWORD               dwCounter;
    BYTE                bProjectileActive;
    BYTE                bPad[3];
    CVector             OldCoors;
    DWORD               dwUnk;
};
// #pragma pack(pop)

class CProjectileInfoSA : public CProjectileInfo
{
private:
    CProjectileInfoSA*          projectileInfo[PROJECTILE_INFO_COUNT];
    CProjectileInfoSAInterface* internalInterface;

public:
    CProjectileInfoSA()
    {
        for (int i = 0; i < PROJECTILE_INFO_COUNT; i++)
        {
            projectileInfo[i] = new CProjectileInfoSA((CProjectileInfoSAInterface*)(ARRAY_CProjectileInfo + i * sizeof(CProjectileInfoSAInterface)));
        }
    }

    CProjectileInfoSA(CProjectileInfoSAInterface* projectileInfoInterface) { internalInterface = projectileInfoInterface; }

    void             RemoveAllProjectiles();
    void             RemoveProjectile(CProjectileInfo* pProjectileInfo, CProjectile* pProjectile, bool bBlow = true);
    CProjectileInfo* GetProjectileInfo(void* projectileInfoInterface);
    CProjectileInfo* GetProjectileInfo(DWORD dwIndex);
    bool             AddProjectile(CEntity* creator, eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity);
    CProjectile*     GetProjectile(void* projectilePointer);

    CEntity* GetTarget();
    void     SetTarget(CEntity* pEntity);

    bool IsActive();

    void  SetCounter(DWORD dwCounter);
    DWORD GetCounter();
};

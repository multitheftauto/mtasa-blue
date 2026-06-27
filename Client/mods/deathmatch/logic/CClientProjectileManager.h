/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientProjectileManager.h
 *  PURPOSE:     Projectile entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientProjectile.h"
#include <list>
#include <vector>

typedef void(ProjectileInitiateHandler)(CClientProjectile*);
class CClientManager;

class CClientProjectileManager
{
    friend class CClientProjectile;

public:
    CClientProjectileManager(CClientManager* pManager);
    ~CClientProjectileManager();

    void               DoPulse();
    void               RemoveAll();
    bool               Exists(CClientProjectile* pProjectile);
    CClientProjectile* Get(CEntitySAInterface* pProjectile);

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); }

    // * Game-layer wrapping *
    static bool Hook_StaticProjectileAllow(CEntity* pGameCreator, eWeaponType weaponType, CVector* origin, float fForce, CVector* target,
                                           CEntity* targetEntity);
    bool        Hook_ProjectileAllow(CEntity* pGameCreator, eWeaponType weaponType, CVector* origin, float fForce, CVector* target, CEntity* targetEntity);
    static void Hook_StaticProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType,
                                              CVector* origin, float fForce, CVector* target, CEntity* pGameTarget);
    void Hook_ProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType, CVector* origin,
                                 float fForce, CVector* target, CEntity* pGameTarget);
    CClientProjectile* Create(CClientEntity* pCreator, eWeaponType eWeapon, CVector& vecOrigin, float fForce, CVector* target, CClientEntity* pTargetEntity);

    // The creator's in-game ped/vehicle may not have streamed in yet (e.g. they just connected, or we only just came
    // into range of a projectile they planted earlier while out of view). Queue the creation and keep retrying for a
    // while instead of silently dropping it.
    void QueuePendingCreation(ElementID creatorID, eWeaponType eWeapon, const CVector& vecOrigin, float fForce, ElementID targetID,
                              ElementID originSourceID, const CVector& vecRotation, const CVector& vecVelocity, unsigned short usModel);

    // Pins a satchel that was just placed by a rest-position resync (zero velocity/force) in place instead of
    // letting physics run on it: the area's collision might not be streamed in yet, and if it was originally stuck
    // to a vehicle/ped it should keep following it (https://github.com/multitheftauto/mtasa-blue/issues/369, #368).
    void SettleResyncedSatchel(CClientProjectile* pProjectile, eWeaponType weaponType, float fForce, const CVector& vecVelocity,
                               CClientEntity* pOriginSource);

protected:
    void AddToList(CClientProjectile* pProjectile) { m_List.push_back(pProjectile); }
    void RemoveFromList(CClientProjectile* pProjectile);

    void TakeOutTheTrash();

private:
    void ProcessPendingCreations();

    CClientManager*               m_pManager;
    std::list<CClientProjectile*> m_List;

    bool             m_bIsLocal;
    CClientEntityPtr m_pCreator;

    bool                 m_bCreating;
    CClientProjectilePtr m_pLastCreated;

    struct SPendingProjectileCreation
    {
        ElementID      creatorID;
        eWeaponType    weaponType;
        CVector        vecOrigin;
        float          fForce;
        ElementID      targetID;
        ElementID      originSourceID;
        CVector        vecRotation;
        CVector        vecVelocity;
        unsigned short usModel;
        long long      llCreationTime;
    };
    std::vector<SPendingProjectileCreation> m_PendingCreations;
};

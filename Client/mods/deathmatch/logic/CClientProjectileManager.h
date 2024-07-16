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

    std::size_t Count() { return m_List.size(); }

    std::list<CClientProjectile*>::iterator       begin() noexcept { return m_List.begin(); }
    std::list<CClientProjectile*>::iterator       end() noexcept { return m_List.end(); }

    std::list<CClientProjectile*>::const_iterator begin() const noexcept { return m_List.cbegin(); }
    std::list<CClientProjectile*>::const_iterator end() const noexcept { return m_List.cend(); }

    std::list<CClientProjectile*>& GetProjectiles() noexcept { return m_List; }
    const std::list<CClientProjectile*>& GetProjectiles() const noexcept { return m_List; }

    // * Game-layer wrapping *
    static bool Hook_StaticProjectileAllow(CEntity* pGameCreator, eWeaponType weaponType, CVector* origin, float fForce, CVector* target,
                                           CEntity* targetEntity);
    bool        Hook_ProjectileAllow(CEntity* pGameCreator, eWeaponType weaponType, CVector* origin, float fForce, CVector* target, CEntity* targetEntity);
    static void Hook_StaticProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType,
                                              CVector* origin, float fForce, CVector* target, CEntity* pGameTarget);
    void Hook_ProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType, CVector* origin,
                                 float fForce, CVector* target, CEntity* pGameTarget);
    CClientProjectile* Create(CClientEntity* pCreator, eWeaponType eWeapon, CVector& vecOrigin, float fForce, CVector* target, CClientEntity* pTargetEntity);

protected:
    void AddToList(CClientProjectile* pProjectile) { m_List.push_back(pProjectile); }
    void RemoveFromList(CClientProjectile* pProjectile);

    void TakeOutTheTrash();

private:
    CClientManager*               m_pManager;
    std::list<CClientProjectile*> m_List;

    bool             m_bIsLocal;
    CClientEntityPtr m_pCreator;

    bool                 m_bCreating;
    CClientProjectilePtr m_pLastCreated;
};

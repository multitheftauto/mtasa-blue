/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientProjectileManager.h
*  PURPOSE:     Projectile entity manager class header
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __CCLIENTPROJECTILEMANAGER_H
#define __CCLIENTPROJECTILEMANAGER_H

#include "CClientProjectile.h"
#include <list>

typedef void ( ProjectileInitiateHandler ) ( CClientProjectile * );
class CClientManager;

class CClientProjectileManager
{
    friend class CClientProjectile;
public:
                                    CClientProjectileManager            ( CClientManager * pManager );
                                    ~CClientProjectileManager           ( void );

    void                            DoPulse                             ( void );
    void                            RemoveAll                           ( void );
    bool                            Exists                              ( CClientProjectile * pProjectile );

    inline unsigned int             Count                               ( void )                                    { return static_cast < unsigned int > ( m_List.size () ); }

    // * Game-layer wrapping *
    static bool                     Hook_StaticProjectileAllow          ( CEntity * pGameCreator, eWeaponType weaponType, CVector * origin, float fForce, CVector * target, CEntity * targetEntity );
    bool                            Hook_ProjectileAllow                ( CEntity * pGameCreator, eWeaponType weaponType, CVector * origin, float fForce, CVector * target, CEntity * targetEntity );
    static void                     Hook_StaticProjectileCreation       ( CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType, CVector * origin, float fForce, CVector * target, CEntity * pGameTarget );
    void                            Hook_ProjectileCreation             ( CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo, eWeaponType weaponType, CVector * origin, float fForce, CVector * target, CEntity * pGameTarget );
    CClientProjectile *             Create                              ( CClientEntity* pCreator, eWeaponType eWeapon, CVector & vecOrigin, float fForce, CVector * target, CClientEntity * pTargetEntity );

protected:
    inline void                     AddToList                           ( CClientProjectile * pProjectile )         { m_List.push_back ( pProjectile ); }
    void                            RemoveFromList                      ( CClientProjectile * pProjectile );

    void                            TakeOutTheTrash                     ( void );
private:
    CClientManager *                    m_pManager;
    std::list < CClientProjectile* >    m_List;

    bool                                m_bIsLocal;
    CClientEntityPtr                    m_pCreator;

    bool                                m_bCreating;
    CClientProjectilePtr                m_pLastCreated;
};

#endif
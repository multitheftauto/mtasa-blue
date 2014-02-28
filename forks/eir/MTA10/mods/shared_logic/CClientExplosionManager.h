/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientExplosionManager.h
*  PURPOSE:     Explosion event manager class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CClientExplosionManager_H
#define __CClientExplosionManager_H

#include "CClientCommon.h"

class CClientManager;
class CClientVehicle;
class CClientEntity;

class CClientExplosionManager
{
    friend class CClientVehicle;
public:
                            CClientExplosionManager         ( CClientManager * pManager );
                            ~CClientExplosionManager        ( void );

    // * Game-layer wrapping *
    static bool             Hook_StaticExplosionCreation    ( CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition, eExplosionType explosionType );
    bool                    Hook_ExplosionCreation          ( CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition, eExplosionType explosionType );
    CExplosion *            Create                          ( eExplosionType explosionType, CVector & vecPosition, CClientEntity * pCreator = NULL, bool bMakeSound = true, float fCamShake = -1.0f, bool bNoDamage = false, eWeaponType responsibleWeapon = WEAPONTYPE_UNARMED );

    eWeaponType             m_LastWeaponType;
    CClientEntityPtr        m_pLastCreator;

private:
    CClientManager *        m_pManager;
};

#endif
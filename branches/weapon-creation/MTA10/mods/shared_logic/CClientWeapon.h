/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWeapon.h
*  PURPOSE:     Weapon entity class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTWEAPON_H
#define __CCLIENTWEAPON_H

#include <game/CWeaponInfo.h>
#include "CClientObject.h"

class CClientManager;
class CObject;
class CModelInfo;
enum eTargetType
{
    TARGET_TYPE_VECTOR,
    TARGET_TYPE_ENTITY,
    TARGET_TYPE_FIXED,
};

struct SWeaponConfiguration
{
    bool bShootIfTargetOutOfRange;
    bool bShootIfTargetBlocked;
    bool bDisableWeaponModel;
    SLineOfSightFlags flags;
};
class CClientWeapon : public CClientObject
{
public:
                            CClientWeapon           ( CClientManager * pManager, ElementID ID, eWeaponType type );
                            ~CClientWeapon          ( void );

    inline eClientEntityType        GetType                 ( void ) const                      { return CCLIENTWEAPON; };

    inline eWeaponType      GetWeaponType           ( void )                { return m_Type; }
    inline eWeaponState     GetWeaponState          ( void )                { return m_State; }
    inline void             SetWeaponState          ( eWeaponState state )  { m_State = state; }
    void                    SetWeaponTarget         ( CClientEntity * pTarget, int subTarget );
    void                    SetWeaponTarget         ( CVector vecTarget );
    void                    ResetWeaponTarget       ( void );

    void                    DoPulse                 ( void );

    void                    Create                  ( void );
    void                    Destroy                 ( void );

    void                    Fire                    ( void );

#ifdef SHOTGUN_TEST
    void                    FireInstantHit          ( CVector & vecOrigin, CVector & vecTarget, CVector & vecRotation );
#else
    void                    FireInstantHit          ( CVector & vecOrigin, CVector & vecTarget );
#endif
    void                    FireShotgun             ( CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget, CVector & vecRotation );

    void                    GetDirection            ( CVector & vecDirection );
    void                    SetDirection            ( CVector & vecDirection );
    void                    SetTargetDirection      ( CVector & vecDirection );
    void                    LookAt                  ( CVector & vecPosition, bool bInterpolate = false );
    CWeaponStat *           GetWeaponStat           ( void )   { return m_pWeaponStat; }
    CClientPlayer *         GetOwner                ( void )                { return m_pOwner; }
    void                    SetOwner                ( CClientPlayer * pOwner )  { m_pOwner = pOwner; }
    void                    SetFlags                ( bool bDisableWeaponModel, bool bShootIfTargetBlocked, bool bShootIfTargetOutOfRange, const SLineOfSightFlags& flags );
    void                    SetFlags                ( bool bDisableWeaponModel, bool bShootIfTargetBlocked, bool bShootIfTargetOutOfRange );
    void                    SetFlags                ( const SLineOfSightFlags& flags );
    void                    GetFlags                ( bool &bDisableWeaponModel, bool &bShootIfTargetBlocked, bool &bShootIfTargetOutOfRange, SLineOfSightFlags& flags );
    void                    DoGunShells             ( CVector vecOrigin, CVector vecDirection );
private:
    CClientManager *        m_pManager;
    CWeapon *               m_pWeapon;
    CWeaponInfo *           m_pWeaponInfo;
    eWeaponType             m_Type;
    eWeaponState            m_State;
    DWORD                   m_dwAmmoInClip;
    CVector                 m_vecTargetDirection;
    CVector                 m_vecLastDirection;
    bool                    m_bHasTargetDirection;
    CClientMarker *         m_pMarker;
    CClientMarker *         m_pMarker2;
    short                   m_sDamage;
    CWeaponStat *           m_pWeaponStat;
    CClientEntity *         m_pTarget;
    CVector                 m_vecTarget;
    eTargetType             m_targetType;
    eBone                   m_targetBone;
    int                     m_itargetWheel;
    SWeaponConfiguration    m_weaponConfig;
    CClientPlayer *         m_pOwner;  // For lag compensation
    int                     m_nAmmoTotal;
    int                     m_nAmmoInClip;
    eWeaponState            m_PreviousState;
    CElapsedTime            m_fireTimer;
    CElapsedTime            m_reloadTimer;
    unsigned char           m_iCounter;
};

#endif
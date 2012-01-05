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

class CClientWeapon : public CClientObject
{
public:
                            CClientWeapon           ( CClientManager * pManager, ElementID ID, eWeaponType type );
                            ~CClientWeapon          ( void );

    inline eWeaponType      GetWeaponType           ( void )                { return m_Type; }
    inline eWeaponState     GetWeaponState          ( void )                { return m_State; }
    inline void             SetWeaponState          ( eWeaponState state )  { m_State = state; }

    void                    DoPulse                 ( void );

    void                    Create                  ( void );
    void                    Destroy                 ( void );

    void                    Fire                    ( void );
    void                    FireInstantHit          ( CVector & vecOrigin, CVector & vecTarget );

    void                    GetDirection            ( CVector & vecDirection );
    void                    SetDirection            ( CVector & vecDirection );
    void                    SetTargetDirection      ( CVector & vecDirection );
    void                    LookAt                  ( CVector & vecPosition, bool bInterpolate = false );

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
};

#endif
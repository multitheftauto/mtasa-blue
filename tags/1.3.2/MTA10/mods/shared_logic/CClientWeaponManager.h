/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWeaponManager.h
*  PURPOSE:     Weapon manager class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTWEAPONMANAGER_H
#define __CCLIENTWEAPONMANAGER_H

#include <list>
class CClientWeapon;

class CClientWeaponManager
{
    friend CClientWeapon;
public:
                                    CClientWeaponManager        ( class CClientManager * pManager );
                                    ~CClientWeaponManager       ( void );

    void                            DoPulse                     ( void );

private:
    void                            AddToList                   ( CClientWeapon * pWeapon )     { m_Weapons.push_back ( pWeapon ); }
    void                            RemoveFromList              ( CClientWeapon * pWeapon );     
    
    CClientManager *                m_pManager;
    std::list < CClientWeapon * >   m_Weapons;
    bool                            m_bDeleteAll;
};

#endif
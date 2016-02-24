/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomWeaponManager.h
*  PURPOSE:     Header file for the manager for all CCustomWeapons
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CCUSTOMWEAPONMANAGER_H
#define __CCUSTOMWEAPONMANAGER_H

#include <list>
#include <logic/CCommon.h>
#include "CCustomWeapon.h"

using std::list;

class CCustomWeapon;
typedef CFastList < CCustomWeapon* >   CCustomWeaponListType;

class CCustomWeaponManager
{
    friend class CCustomWeapon;
public:
            CCustomWeaponManager                ( void );
            ~CCustomWeaponManager               ( void );
    void    DoPulse                             ( void );

    CCustomWeaponListType::const_iterator     IterBegin               ( void ) const                      { return m_List.begin (); };
    CCustomWeaponListType::const_iterator     IterEnd                 ( void ) const                      { return m_List.end (); };

private:
    inline void                 AddToList                       ( CCustomWeapon* pObject )                { m_List.push_back ( pObject ); };
    void                        RemoveFromList                  ( CCustomWeapon* pObject );

    CCustomWeaponListType       m_List;
};
#endif
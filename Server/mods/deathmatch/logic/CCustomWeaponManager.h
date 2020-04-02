/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomWeaponManager.h
 *  PURPOSE:     Header file for the manager for all CCustomWeapons
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CCommon.h"
#include "CCustomWeapon.h"

using std::list;

class CCustomWeapon;
typedef CFastList<CCustomWeapon*> CCustomWeaponListType;

class CCustomWeaponManager
{
    friend class CCustomWeapon;

public:
    CCustomWeaponManager();
    ~CCustomWeaponManager();
    void DoPulse();

    CCustomWeaponListType::const_iterator IterBegin() const { return m_List.begin(); };
    CCustomWeaponListType::const_iterator IterEnd() const { return m_List.end(); };

private:
    void AddToList(CCustomWeapon* pObject) { m_List.push_back(pObject); };
    void RemoveFromList(CCustomWeapon* pObject);

    CCustomWeaponListType m_List;
};

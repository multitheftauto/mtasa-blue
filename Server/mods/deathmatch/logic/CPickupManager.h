/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPickupManager.h
 *  PURPOSE:     Pickup entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPickupManager;

#pragma once

#include "CColManager.h"
#include "CPickup.h"
#include <list>

class CPickupManager
{
    friend class CPickup;

public:
    CPickupManager(CColManager* pColManager);
    ~CPickupManager() { DeleteAll(); };

    CPickup* Create(CElement* pParent);
    CPickup* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void     DeleteAll();

    std::size_t Count() const noexcept { return m_List.size(); };
    bool        Exists(CPickup* pPickup) const noexcept;

    std::list<CPickup*>::iterator begin() { return m_List.begin(); };
    std::list<CPickup*>::iterator end() { return m_List.end(); };

    std::list<CPickup*>::const_iterator begin() const noexcept { return m_List.cbegin(); };
    std::list<CPickup*>::const_iterator end() const noexcept { return m_List.cend(); };

    static bool          IsValidWeaponID(std::uint32_t uiWeaponID) noexcept;
    static std::uint16_t GetWeaponModel(std::uint32_t uiWeaponID) noexcept;
    constexpr static std::uint16_t GetHealthModel() noexcept { return 1240; };
    constexpr static std::uint16_t GetArmorModel() noexcept { return 1242; };

private:
    void AddToList(CPickup* pPickup) { m_List.push_back(pPickup); };
    void RemoveFromList(CPickup* pPickup) { m_List.remove(pPickup); };

    CColManager*        m_pColManager;
    std::list<CPickup*> m_List;
};

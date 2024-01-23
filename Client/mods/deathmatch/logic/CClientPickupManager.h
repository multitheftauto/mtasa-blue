/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPickupManager.h
 *  PURPOSE:     Pickup entity manager class header
 *
 *****************************************************************************/

class CClientPickupManager;

#pragma once

#include "CClientManager.h"
#include "CClientPickup.h"
#include <list>

class CClientPickupManager
{
    friend class CClientManager;
    friend class CClientPickup;

public:
    std::size_t           Count() const noexcept { return m_List.size(); };
    static CClientPickup* Get(ElementID ID);

    void DeleteAll();
    bool Exists(CClientPickup* pPickup);

    std::list<CClientPickup*>::iterator begin() noexcept { return m_List.begin(); };
    std::list<CClientPickup*>::iterator end() noexcept { return m_List.end(); };

    std::list<CClientPickup*>::const_iterator begin() const noexcept { return m_List.cbegin(); };
    std::list<CClientPickup*>::const_iterator end() const noexcept { return m_List.cend(); };

    bool IsPickupProcessingDisabled() const noexcept { return m_bPickupProcessingDisabled; };
    void SetPickupProcessingDisabled(bool bDisabled);

    static bool IsValidPickupID(std::uint16_t usPickupID);
    static bool IsValidWeaponID(std::uint16_t usWeaponID);

    static std::uint16_t GetWeaponModel(std::uint32_t uiWeaponID) noexcept;
    constexpr static std::uint16_t GetHealthModel() noexcept { return 1240; };
    constexpr static std::uint16_t GetArmorModel() noexcept { return 1242; };

    static bool IsPickupLimitReached();
    void        RestreamPickups(std::uint16_t usModel);
    void        RestreamAllPickups();

private:
    CClientPickupManager(CClientManager* pManager);
    ~CClientPickupManager();

    void RemoveFromList(CClientPickup* pPickup);

    CClientManager* m_pManager;

    std::list<CClientPickup*> m_List;
    bool                      m_bDontRemoveFromList;

    bool                m_bPickupProcessingDisabled;
    static std::uint32_t m_uiPickupCount;
};

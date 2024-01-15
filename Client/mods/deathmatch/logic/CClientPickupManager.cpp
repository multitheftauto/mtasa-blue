/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPickupManager.cpp
 *  PURPOSE:     Pickup entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CPickups.h>

static const SFixedArray<std::uint16_t, 47> g_usWeaponModels = {{
    0,   331, 333, 334, 335, 336, 337, 338, 339, 341,            // 9
    321, 322, 323, 0,   325, 326, 342, 343, 344, 0,              // 19
    0,   0,   346, 347, 348, 349, 350, 351, 352, 353,            // 29
    355, 356, 372, 357, 358, 359, 360, 361, 362, 363,            // 39
    364, 365, 366, 367, 368, 369, 371                            // 46
}};

std::uint32_t CClientPickupManager::m_uiPickupCount = 0;

CClientPickupManager::CClientPickupManager(CClientManager* pManager)
{
    m_pManager = pManager;
    m_bDontRemoveFromList = false;

    m_bPickupProcessingDisabled = false;
}

CClientPickupManager::~CClientPickupManager()
{
    // Delete all the pickups
    DeleteAll();
}

CClientPickup* CClientPickupManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTPICKUP)
        return static_cast<CClientPickup*>(pEntity);

    return nullptr;
}

void CClientPickupManager::DeleteAll()
{
    // Delete each pickup
    m_bDontRemoveFromList = true;
    for (const auto& pPickup : m_List)
        delete pPickup;

    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear();
}

bool CClientPickupManager::Exists(CClientPickup* pPickup)
{
    for (const auto& pEntry : m_List)
        if (pEntry == pPickup)
            return true;

    return false;
}

void CClientPickupManager::SetPickupProcessingDisabled(bool bDisabled)
{
    g_pGame->GetPickups()->DisablePickupProcessing(bDisabled);
    m_bPickupProcessingDisabled = bDisabled;
}

bool CClientPickupManager::IsValidPickupID(std::uint16_t usPickupID)
{
    return (usPickupID > 0 && usPickupID != 13 && usPickupID != 19 && usPickupID != 20 && usPickupID != 21 && usPickupID <= 46 || usPickupID == 1240 ||
            usPickupID == 1242);
}

bool CClientPickupManager::IsValidWeaponID(std::uint16_t usWeaponID)
{
    return (usWeaponID > 0 && usWeaponID != 13 && usWeaponID != 19 && usWeaponID != 20 && usWeaponID != 21 && usWeaponID <= 46);
}

bool CClientPickupManager::IsPickupLimitReached()
{
    // Max 600 pickups
    return m_uiPickupCount >= 64;
}

std::uint16_t CClientPickupManager::GetWeaponModel(std::uint32_t uiWeaponID) noexcept
{
    return uiWeaponID <= 46 ? g_usWeaponModels[uiWeaponID] : 0;
}

void CClientPickupManager::RemoveFromList(CClientPickup* pPickup)
{
    if (m_bDontRemoveFromList)
        return;

    if (!m_List.empty())
        m_List.remove(pPickup);
}

void CClientPickupManager::RestreamPickups(std::uint16_t usModel)
{
    for (const auto& pPickup : m_List)
    {
        if (!pPickup->IsStreamedIn() || pPickup->GetModel() != usModel)
            continue;

        pPickup->StreamOutForABit();
    }
}

void CClientPickupManager::RestreamAllPickups()
{
    for (auto& pPickup : m_List)
    {
        if (!pPickup->IsStreamedIn())
            continue;

        pPickup->StreamOutForABit();
    }
}

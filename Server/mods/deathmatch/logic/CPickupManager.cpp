/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPickupManager.cpp
 *  PURPOSE:     Pickup entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPickupManager.h"
#include "Utils.h"

static SFixedArray<unsigned short, 47> g_usWeaponModels = {{
    0,   331, 333, 334, 335, 336, 337, 338, 339, 341,            // 9
    321, 322, 323, 0,   325, 326, 342, 343, 344, 0,              // 19
    0,   0,   346, 347, 348, 349, 350, 351, 352, 353,            // 29
    355, 356, 372, 357, 358, 359, 360, 361, 362, 363,            // 39
    364, 365, 366, 367, 368, 369, 371                            // 46
}};

CPickupManager::CPickupManager(CColManager* pColManager)
{
    // Init
    m_pColManager = pColManager;
}

CPickup* CPickupManager::Create(CElement* pParent)
{
    CPickup* const pPickup = new CPickup(pParent, this, m_pColManager);

    if (pPickup->GetID() == INVALID_ELEMENT_ID)
    {
        delete pPickup;
        return nullptr;
    }

    return pPickup;
}

CPickup* CPickupManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CPickup* const pPickup = new CPickup(pParent, this, m_pColManager);

    if (pPickup->GetID() == INVALID_ELEMENT_ID || !pPickup->LoadFromCustomData(pEvents, Node))
    {
        delete pPickup;
        return nullptr;
    }

    return pPickup;
}

void CPickupManager::DeleteAll()
{
    DeletePointersAndClearList(m_List);
}

bool CPickupManager::Exists(CPickup* pPickup)
{
    return ListContains(m_List, pPickup);
}

bool CPickupManager::IsValidWeaponID(unsigned int uiWeaponID)
{
    return (uiWeaponID > 0 && uiWeaponID != 13 && uiWeaponID != 19 && uiWeaponID != 20 && uiWeaponID != 21 && uiWeaponID <= 46);
}

unsigned short CPickupManager::GetWeaponModel(unsigned int uiWeaponID)
{
    if (uiWeaponID <= 46)
    {
        return g_usWeaponModels[uiWeaponID];
    }

    return 0;
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPickupRPCs.cpp
 *  PURPOSE:     Pickup remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CPickupRPCs.h"

void CPickupRPCs::LoadFunctions()
{
    AddHandler(DESTROY_ALL_PICKUPS, DestroyAllPickups, "DestroyAllPickups");
    AddHandler(SET_PICKUP_TYPE, SetPickupType, "SetPickupType");
    AddHandler(SET_PICKUP_VISIBLE, SetPickupVisible, "SetPickupVisible");
}

void CPickupRPCs::DestroyAllPickups(NetBitStreamInterface& bitStream)
{
    // Delete all pickups
    m_pPickupManager->DeleteAll();
}

void CPickupRPCs::SetPickupType(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the pickup id and the position vector
    unsigned char ucType;
    if (bitStream.Read(ucType))
    {
        // Grab the pickup
        CClientPickup* pPickup = m_pPickupManager->Get(pSource->GetID());
        if (pPickup)
        {
            if (ucType == CClientPickup::ARMOR || ucType == CClientPickup::HEALTH)
            {
                float fAmount;
                bitStream.Read(fAmount);
                if (ucType == CClientPickup::ARMOR)
                    pPickup->SetModel(CClientPickupManager::GetArmorModel());
                else
                    pPickup->SetModel(CClientPickupManager::GetHealthModel());
                pPickup->m_ucType = ucType;
                pPickup->m_fAmount = fAmount;
            }
            else if (ucType == CClientPickup::WEAPON)
            {
                // Get the weapon id
                unsigned char  ucWeaponID;
                unsigned short usAmmo;
                bitStream.Read(ucWeaponID);
                bitStream.Read(usAmmo);
                if (CClientPickupManager::IsValidWeaponID(ucWeaponID))
                {
                    pPickup->SetModel(CClientPickupManager::GetWeaponModel(ucWeaponID));
                    pPickup->m_ucType = ucType;
                    pPickup->m_ucWeaponType = ucWeaponID;
                    pPickup->m_usAmmo = usAmmo;
                }
            }
            else if (ucType == CClientPickup::CUSTOM)
            {
                unsigned short usModel;
                bitStream.Read(usModel);
                if (CClientObjectManager::IsValidModel(usModel))
                {
                    pPickup->SetModel(usModel);
                    pPickup->m_ucType = ucType;
                    pPickup->SetModel(usModel);
                }
            }
        }
    }
}

void CPickupRPCs::SetPickupVisible(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the pickup id and the is visible flag
    unsigned char ucVisible;
    if (bitStream.Read(ucVisible))
    {
        // Grab the pickup
        CClientPickup* pPickup = m_pPickupManager->Get(pSource->GetID());
        if (pPickup)
        {
            // Set whether it's visible or not
            pPickup->SetVisible(ucVisible != 0);
        }
    }
}
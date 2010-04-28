/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPickupManager.cpp
*  PURPOSE:     Pickup entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static unsigned short g_usWeaponModels [47] =
{   0, 331, 333, 334, 335, 336, 337, 338, 339, 341,     // 9
    321, 322, 323, 0, 325, 326, 342, 343, 344, 0,       // 19
    0, 0, 346, 347, 348, 349, 350, 351, 352, 353,       // 29
    355, 356, 372, 357, 358, 359, 360, 361, 362, 363,   // 39
    364, 365, 366, 367, 368, 369, 371                   // 46
};

CPickupManager::CPickupManager ( CColManager* pColManager )
{
    // Init
    m_bDontRemoveFromList = false;
    m_pColManager = pColManager;
}


CPickup* CPickupManager::Create ( CElement* pParent )
{
    // Create the pickup
    CPickup* pPickup = new CPickup ( pParent, NULL, this, m_pColManager );

    // Delete it if it's invalid
    if ( pPickup->GetID () == INVALID_ELEMENT_ID )
    {
        delete pPickup;
        return NULL;
    }

    // Return the pickup
    return pPickup;
}


CPickup* CPickupManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the pickup
    CPickup* pPickup = new CPickup ( pParent, &Node, this, m_pColManager );

    // Delete it if it's invalid
    if ( pPickup->GetID () == INVALID_ELEMENT_ID ||
         !pPickup->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pPickup;
        return NULL;
    }

    // Return the pickup
    return pPickup;
}


void CPickupManager::DeleteAll ( void )
{
    // Delete all the classes
    m_bDontRemoveFromList = true;
    list < CPickup* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}


bool CPickupManager::Exists ( CPickup* pPickup )
{
    // Return true if it exists, otherwize false
    list < CPickup* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pPickup )
        {
            return true;
        }
    }

    return false;
}


bool CPickupManager::IsValidWeaponID ( unsigned int uiWeaponID )
{
    return ( uiWeaponID > 0 &&
             uiWeaponID != 13 &&
             uiWeaponID != 19 &&
             uiWeaponID != 20 &&
             uiWeaponID != 21 &&
             uiWeaponID <= 46 );
}


unsigned short CPickupManager::GetWeaponModel ( unsigned int uiWeaponID )
{
    if ( uiWeaponID <= 46 )
    {
        return g_usWeaponModels [uiWeaponID];
    }

    return 0;
}

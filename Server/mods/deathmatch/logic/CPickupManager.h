/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPickupManager.h
 *  PURPOSE:     Pickup entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CPickup* pPickup);

    std::list<CPickup*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CPickup*>::const_iterator IterEnd() { return m_List.end(); };

    static bool           IsValidWeaponID(unsigned int uiWeaponID);
    static unsigned short GetWeaponModel(unsigned int uiWeaponID);
    static unsigned short GetHealthModel() { return 1240; };
    static unsigned short GetArmorModel() { return 1242; };

private:
    void AddToList(CPickup* pPickup) { m_List.push_back(pPickup); };
    void RemoveFromList(CPickup* pPickup) { m_List.remove(pPickup); };

    CColManager*        m_pColManager;
    std::list<CPickup*> m_List;
};

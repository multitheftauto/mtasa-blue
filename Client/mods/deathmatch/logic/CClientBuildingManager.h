/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientBuildingManager.h
 *  PURPOSE:     Building manager class
 *
 *****************************************************************************/

class CClientBuildingManager;

#pragma once

#include <list>
#include "CClientBuilding.h"

class CClientBuildingManager
{
    friend class CClientBuilding;

public:
    CClientBuildingManager(class CClientManager* pManager);
    ~CClientBuildingManager();

    void RemoveAll();
    bool Exists(CClientBuilding* pBuilding);

    std::list<CClientBuilding*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientBuilding*>::const_iterator IterEnd() { return m_List.end(); }

private:
    void AddToList(CClientBuilding* pBuilding) { m_List.push_back(pBuilding); }
    void RemoveFromList(CClientBuilding* pBuilding);

    class CClientObjectManager*  m_pObjectManager;
    class CClientVehicleManager* m_pVehicleManager;

    std::list<CClientBuilding*> m_List;
    bool                        m_bRemoveFromList;
};

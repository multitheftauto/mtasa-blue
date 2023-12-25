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

    const std::list<CClientBuilding*>& GetBuildings() { return m_List; };

private:
    void AddToList(CClientBuilding* pBuilding) { m_List.push_back(pBuilding); }
    void RemoveFromList(CClientBuilding* pBuilding);

    std::list<CClientBuilding*> m_List;
    bool                        m_bRemoveFromList;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetInstanceManager.h
 *  PURPOSE:     AssetInstance entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientAssetInstance.h"
#include <list>
// class CClientAssetModel;

class CClientAssetInstanceManager
{
    friend class CClientManager;
    friend class CClientAssetModel;
    friend class CClientSearchLight;

public:
    CClientAssetInstanceManager(CClientManager* pManager);
    ~CClientAssetInstanceManager();

    CClientAssetInstance* Create(ElementID ID);

    void Delete(CClientAssetInstance* pAssetModel);
    void DeleteAll();

    std::list<CClientAssetInstance*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientAssetInstance*>::const_iterator IterEnd() { return m_List.end(); };

    static CClientAssetInstance* Get(ElementID ID);

    void DoPulse();

    void AddToList(CClientAssetInstance* pAssetInstance) { m_List.push_back(pAssetInstance); };
    void RemoveFromList(CClientAssetInstance* pAssetInstance);

private:
    CClientManager* m_pManager;

    std::list<CClientAssetInstance*> m_List;
    bool                             m_bDontRemoveFromList;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModelManager.h
 *  PURPOSE:     PointLights entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientAssetModel.h"
#include <list>
// class CClientAssetModel;

class CClientAssetModelManager
{
    friend class CClientManager;
    friend class CClientAssetModel;
    friend class CClientSearchLight;

public:
    CClientAssetModelManager(CClientManager* pManager);
    ~CClientAssetModelManager();

    CClientAssetModel* Create(ElementID ID);

    void Delete(CClientAssetModel* pAssetModel);
    void DeleteAll();

    std::list<CClientAssetModel*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientAssetModel*>::const_iterator IterEnd() { return m_List.end(); };

    static CClientAssetModel* Get(ElementID ID);

    void DoPulse();

private:
    void AddToList(CClientAssetModel* pAssetModel) { m_List.push_back(pAssetModel); };
    void RemoveFromList(CClientAssetModel* pAssetModel);

    void AddToAssetModelList(CClientAssetModel* pAssetModel) { m_AssetModelList.push_back(pAssetModel); }
    void RemoveAssetModelFromList(CClientAssetModel* pAssetModel);

private:
    CClientManager* m_pManager;

    std::list<CClientAssetModel*> m_List;
    std::list<CClientAssetModel*> m_AssetModelList;
    bool                           m_bDontRemoveFromList;
};

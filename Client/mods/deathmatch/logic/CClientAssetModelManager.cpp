/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModelManager.cpp
 *  PURPOSE:     PointLights entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientAssetModelManager::CClientAssetModelManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}

CClientAssetModelManager::~CClientAssetModelManager()
{
    // Make sure all the lights are deleted
    DeleteAll();
}

CClientAssetModel* CClientAssetModelManager::Create(ElementID EntityID)
{
    if (!Get(EntityID))
    {
        return new CClientAssetModel(m_pManager, EntityID);
    }

    return NULL;
}

void CClientAssetModelManager::Delete(CClientAssetModel* pAssetModel)
{
    delete pAssetModel;
}

void CClientAssetModelManager::DeleteAll()
{
    // Delete all the lights
    m_bDontRemoveFromList = true;
    list<CClientAssetModel*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    for (auto pAssetModel : m_AssetModelList)
    {
        delete pAssetModel;
    }
    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear();
    m_AssetModelList.clear();
}

CClientAssetModel* CClientAssetModelManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTASSETMODEL)
    {
        return static_cast<CClientAssetModel*>(pEntity);
    }

    return NULL;
}

void CClientAssetModelManager::DoPulse()
{
    // Pulse each light
    list<CClientAssetModel*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
    {
        (*iter)->DoPulse();
    }
}

void CClientAssetModelManager::RemoveFromList(CClientAssetModel* pAssetModel)
{
    if (!m_bDontRemoveFromList)
    {
        if (!m_List.empty())
            m_List.remove(pAssetModel);
    }
}

void CClientAssetModelManager::RemoveAssetModelFromList(CClientAssetModel* pAssetModel)
{
    if (!m_bDontRemoveFromList)
    {
        m_AssetModelList.remove(pAssetModel);
    }
}

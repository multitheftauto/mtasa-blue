/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetInstanceManager.cpp
 *  PURPOSE:     AssetInstance entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientAssetInstanceManager::CClientAssetInstanceManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}

CClientAssetInstanceManager::~CClientAssetInstanceManager()
{
    // Make sure all the lights are deleted
    DeleteAll();
}

CClientAssetInstance* CClientAssetInstanceManager::Create(ElementID EntityID)
{
    if (!Get(EntityID))
    {
        return new CClientAssetInstance(m_pManager, EntityID);
    }

    return NULL;
}

void CClientAssetInstanceManager::Delete(CClientAssetInstance* pAssetModel)
{
    delete pAssetModel;
}

void CClientAssetInstanceManager::DeleteAll()
{
    // Delete all the lights
    m_bDontRemoveFromList = true;
    list<CClientAssetInstance*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear();
}

CClientAssetInstance* CClientAssetInstanceManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTASSETINSTANCE)
    {
        return static_cast<CClientAssetInstance*>(pEntity);
    }

    return NULL;
}

void CClientAssetInstanceManager::DoPulse()
{
    // Pulse each light
    list<CClientAssetInstance*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
    {
        (*iter)->DoPulse();
    }
}

void CClientAssetInstanceManager::RemoveFromList(CClientAssetInstance* pAssetInstance)
{
    if (!m_bDontRemoveFromList)
    {
        if (!m_List.empty())
            m_List.remove(pAssetInstance);
    }
}

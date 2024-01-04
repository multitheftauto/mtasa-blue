/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerManager.cpp
 *  PURPOSE:     Marker entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientEffekseerManager::CClientEffekseerManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}

CClientEffekseerManager::~CClientEffekseerManager()
{
    // Make sure all effects are deleted
    DeleteAll();
}

void CClientEffekseerManager::RemoveFromList(CClientEffekseerEffect* pEffekseerFX)
{
    m_List.remove(pEffekseerFX);
}

CClientEffekseerEffect* CClientEffekseerManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTEFFEKSEERFX)
    {
        return static_cast<CClientEffekseerEffect*>(pEntity);
    }

    return nullptr;
}

void CClientEffekseerManager::DeleteAll()
{
    // Delete each effect
    m_bCanRemoveFromList = false;

    for (CClientEffekseerEffect* fx : m_List)
    {
        delete fx;
    }

    m_bCanRemoveFromList = true;

    // Clear the list
    m_List.clear();
}

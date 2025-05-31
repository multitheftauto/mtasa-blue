/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CObjectRespawner.cpp
 *  PURPOSE:     Manager for objects to be respawned
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectRespawner.h"

CObjectRespawner::CObjectRespawner()
{
}

void CObjectRespawner::Respawn(CClientObject* pObject)
{
    // Make sure we don't try to delete it twice
    if (pObject && !IsBeingRespawned(pObject))
    {
        // Add it to our list
        if (!pObject->IsBeingRespawned())
        {
            m_List.push_back(pObject);
        }

        // Flag it as being respawned
        pObject->SetBeingRespawned(true);
    }
}

void CObjectRespawner::DoRespawnAll()
{
    for (uint i = 0; i < m_List.size(); i++)
    {
        CClientObject* pObject = m_List[i];

        // Only recreate if we're still valid and streamed in
        if (pObject && !pObject->IsBeingDeleted() && pObject->IsStreamedIn())
        {
            pObject->ReCreate();
            pObject->SetBeingRespawned(false);
        }
    }

    m_List.clear();
}

bool CObjectRespawner::IsBeingRespawned(CClientObject* pObject)
{
    return std::find(m_List.begin(), m_List.end(), pObject) != m_List.end();
}

void CObjectRespawner::Unreference(CClientObject* pObject)
{
    std::vector<CClientObject*>::iterator it = std::find(m_List.begin(), m_List.end(), pObject);
    if (it != m_List.end())
        m_List.erase(it);
}

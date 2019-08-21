/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFBXManager.cpp
 *  PURPOSE:     PointLights entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientFBXManager::CClientFBXManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
}

CClientFBXManager::~CClientFBXManager(void)
{
    // Make sure all the lights are deleted
    DeleteAll();
}

void CClientFBXManager::DeleteAll(void)
{
    list<CClientFBX*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
}

void CClientFBXManager::RemoveFromList(CClientFBX* pFBX)
{
    if (!m_List.empty())
        m_List.remove(pFBX);
}

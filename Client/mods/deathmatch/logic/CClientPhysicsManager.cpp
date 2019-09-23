/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysicsManager.cpp
 *  PURPOSE:     PointLights entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientPhysicsManager::CClientPhysicsManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
}

CClientPhysicsManager::~CClientPhysicsManager(void)
{
    // Make sure all the lights are deleted
    DeleteAll();
}

void CClientPhysicsManager::DeleteAll(void)
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
}

void CClientPhysicsManager::RemoveFromList(CClientPhysics* pLight)
{
    if (!m_List.empty())
        m_List.remove(pLight);
}

void CClientPhysicsManager::DoPulse(void)
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        (*iter)->DoPulse();
    }
}
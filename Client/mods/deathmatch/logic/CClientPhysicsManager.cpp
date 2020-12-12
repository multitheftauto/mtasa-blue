/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysicsManager.cpp
 *  PURPOSE:     Physics manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <stack>


using std::list;


CClientPhysicsManager::CClientPhysicsManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;

    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2);
}

CClientPhysicsManager::~CClientPhysicsManager()
{
    // Make sure all the physics worlds are deleted
    DeleteAll();
}

void CClientPhysicsManager::DeleteAll()
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
}

void CClientPhysicsManager::DoWork()
{

}

void CClientPhysicsManager::RemoveFromList(CClientPhysics* pPhysics)
    {
    if (!m_List.empty())
        m_List.remove(pPhysics);
}

CClientPhysics* CClientPhysicsManager::GetPhysics(btDiscreteDynamicsWorld* pDynamicsWorld)
{
    if (m_List.empty())
        return nullptr;

    list<CClientPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->GetDynamicsWorld() == pDynamicsWorld)
        {
            return *iter;
        }
    }
    return nullptr;
}

void CClientPhysicsManager::DoPulse(void)
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    std::vector<CClientPhysics*>          vecPhysics;
    for (; iter != IterEnd(); iter++)
    {
        if((*iter)->CanDoPulse())
        {
            vecPhysics.push_back(*iter);
        }
    }

    for (auto const& pPhysics : vecPhysics)
    {
        pPhysics->DoPulse();
        //m_pAsyncTaskScheduler->PushTask<bool>(
        //    [pPhysics] {
        //        //g_pCore->GetConsole()->Printf("asdf");
        //        pPhysics->DoPulse();
        //        return true;
        //    },
        //    [](bool test) { int a = 5;
        //    });
    }

}

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

void CClientPhysicsManager::WaitForSimulationsToFinish()
{
    while (isLocked)
        ;
}

void CClientPhysicsManager::DoPulse()
{
    m_pAsyncTaskScheduler->CollectResults();

    list<CClientPhysics*>::const_iterator iter = IterBegin();
    std::vector<CClientPhysics*>          vecPhysics;
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->CanDoPulse())
        {
            vecPhysics.push_back(*iter);
            (*iter)->DrawDebugLines();
        }
    }

    isLocked = true;
    m_numPhysicsLeft = vecPhysics.size();
    for (auto const& pPhysics : vecPhysics)
    {
        m_pAsyncTaskScheduler->PushTask<long>(
            [pPhysics] {
                long start = GetTickCount64_();
                pPhysics->DoPulse();
                return GetTickCount64_() - start;
            },
            [this](long tickedPassed) {
                g_pCore->GetConsole()->Printf("DoPulse: %ld", tickedPassed);
                this->m_numPhysicsLeft--;
                if (this->m_numPhysicsLeft == 0)
                {
                    isLocked = false;
                }
            });
    }
}

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
#include <pthread.h>
#include "CBulletPhysicsProfiler.h"

using std::list;

CClientPhysicsManager::CClientPhysicsManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;

    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2, 1);

    CBulletPhysicsProfiler::Start();
}

CClientPhysicsManager::~CClientPhysicsManager()
{
    WaitForSimulationsToFinish(true);
    // Make sure all the physics worlds are deleted

    CBulletPhysicsProfiler::Stop();
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
    WaitForSimulationsToFinish(true);
    if (!m_List.empty())
        m_List.remove(pPhysics);
}

void CClientPhysicsManager::WaitForSimulationsToFinish(bool bForceWait)
{
    if (m_bWaitForSimulationToFinish || bForceWait)
        while (isLocked)
        {
            m_pAsyncTaskScheduler->CollectResults();
            if (isLocked)            // still locked?
            {
                // g_pCore->GetConsole()->Printf("sleep 1");
                Sleep(1);
            }
            list<CClientPhysics*>::const_iterator iter = IterBegin();
            for (; iter != IterEnd(); iter++)
            {
                (*iter)->WaitForSimulationToFinish();
            }
        }
    else
        m_pAsyncTaskScheduler->CollectResults();
}

void CClientPhysicsManager::DrawDebug()
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->CanDoPulse())
        {
            (*iter)->DrawDebugLines();
        }
    }
}

void CClientPhysicsManager::DoPulse()
{
    list<CClientPhysics*>::const_iterator iter = IterBegin();
    std::vector<CClientPhysics*>          vecPhysics;
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->CanDoPulse())
        {
            vecPhysics.push_back(*iter);
        }
    }

    m_numPhysicsLeft = vecPhysics.size();

    if (m_numPhysicsLeft > 0)
    {
        isLocked = true;
        for (auto const& pPhysics : vecPhysics)
        {
            m_pAsyncTaskScheduler->PushTask<long>(
                [pPhysics] {
                    long start = GetTickCount64_();
                    SetThreadDescription(GetCurrentThread(), L"Physics worker");
                    if (pPhysics)
                    {
                        pPhysics->DoPulse();
                    }
                    return GetTickCount64_() - start;
                },
                [&](long ticksPassed) {
                    /* if (tickedPassed > 10)
                         g_pCore->GetConsole()->Printf("DoPulse: %ld", tickedPassed);*/
                    m_numPhysicsLeft--;
                    if (m_numPhysicsLeft == 0)
                    {
                        isLocked = false;
                    }
                });
        }
    }
}

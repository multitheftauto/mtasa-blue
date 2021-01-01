/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysicsManager.cpp
 *  PURPOSE:     Physics manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <stack>
#include <pthread.h>
#include "CBulletPhysicsProfiler.h"

using std::list;

#ifdef MTA_CLIENT


CBulletPhysicsManager::CBulletPhysicsManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;

    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2, 1);

    CBulletPhysicsProfiler::Start();
}

#else

CBulletPhysicsManager::CBulletPhysicsManager()
{
    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2, 1);

    CBulletPhysicsProfiler::Start();
}

#endif

CBulletPhysicsManager::~CBulletPhysicsManager()
{
    WaitForSimulationsToFinish(true);
    // Make sure all the physics worlds are deleted

    CBulletPhysicsProfiler::Stop();
    DeleteAll();
}

void CBulletPhysicsManager::DeleteAll()
{
    list<CBulletPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
}

void CBulletPhysicsManager::DoWork()
{
}

void CBulletPhysicsManager::RemoveFromList(CBulletPhysics* pPhysics)
{
    WaitForSimulationsToFinish(true);
    if (!m_List.empty())
        m_List.remove(pPhysics);
}

void CBulletPhysicsManager::WaitForSimulationsToFinish(bool bForceWait)
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
            list<CBulletPhysics*>::const_iterator iter = IterBegin();
            for (; iter != IterEnd(); iter++)
            {
                (*iter)->WaitForSimulationToFinish();
            }
        }
    else
        m_pAsyncTaskScheduler->CollectResults();
}

#ifdef MTA_CLIENT
void CBulletPhysicsManager::DrawDebug()
{
    list<CBulletPhysics*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->CanDoPulse())
        {
            (*iter)->DrawDebugLines();
        }
    }
}
#endif

void CBulletPhysicsManager::DoPulse()
{
    list<CBulletPhysics*>::const_iterator iter = IterBegin();
    std::vector<CBulletPhysics*>          vecPhysics;
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

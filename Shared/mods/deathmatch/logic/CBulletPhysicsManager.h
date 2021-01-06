/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysicsManager.h
 *  PURPOSE:     Physics manager class
 *
 *****************************************************************************/

#include "LinearMath/btThreads.h"

#pragma once

class btDiscreteDynamicsWorld;
class CBulletPhysics;

class CBulletPhysicsManager
{
    friend class CClientManager;
    friend class CBulletPhysics;

public:
#ifdef MTA_CLIENT
    CBulletPhysicsManager(CClientManager* pManager);
#else
    CBulletPhysicsManager();
#endif
    ~CBulletPhysicsManager();

    std::list<CBulletPhysics*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CBulletPhysics*>::const_iterator IterEnd() { return m_List.end(); };
#ifdef MTA_CLIENT
    void DrawDebug();
#endif
    void DoPulse();
    void WaitForSimulationsToFinish(bool bForceWait = false);

private:
    void AddToList(CBulletPhysics* pPhysics) { m_List.push_back(pPhysics); };
    void RemoveFromList(CBulletPhysics* pPhysics);
    void DeleteAll();
    void DoWork();

    CClientManager* m_pManager;

    std::list<CBulletPhysics*> m_List;

    std::atomic<bool>                 isLocked;
    SharedUtil::CAsyncTaskScheduler*  m_pAsyncTaskScheduler;
    std::atomic<int>                  m_numPhysicsLeft;
    bool                              m_bWaitForSimulationToFinish = true;
    std::unique_ptr<btITaskScheduler> m_pBtTaskScheduler;
};

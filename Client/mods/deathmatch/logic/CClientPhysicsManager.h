/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysicsManager.h
 *  PURPOSE:     Physics manager class
 *
 *****************************************************************************/

#pragma once

class btDiscreteDynamicsWorld;

#include "ConcurrentStack.hpp"

class CClientPhysicsManager
{
    friend class CClientManager;
    friend class CClientPhysics;

public:
    CClientPhysicsManager(CClientManager* pManager);
    ~CClientPhysicsManager();


    std::list<CClientPhysics*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientPhysics*>::const_iterator IterEnd() { return m_List.end(); };
    CClientPhysics*                            GetPhysics(btDiscreteDynamicsWorld* pDynamicsWorld);
    void DoPulse();

private:

    void AddToList(CClientPhysics* pPhysics) { m_List.push_back(pPhysics); };
    void RemoveFromList(CClientPhysics* pPhysics);
    void DeleteAll();
    void DoWork();

    CClientManager* m_pManager;

    std::list<CClientPhysics*> m_List;

    std::atomic_flag                 lock = ATOMIC_FLAG_INIT;
    SharedUtil::CAsyncTaskScheduler* m_pAsyncTaskScheduler;

};

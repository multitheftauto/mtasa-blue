/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysicsManager.h
 *  PURPOSE:     PointLights entity manager class header
 *
 *****************************************************************************/

#pragma once

class btDiscreteDynamicsWorld;

class CClientPhysicsManager
{
    friend class CClientManager;
    friend class CClientPhysics;

public:
    CClientPhysicsManager(CClientManager* pManager);
    ~CClientPhysicsManager(void);


    std::list<CClientPhysics*>::const_iterator IterBegin(void) { return m_List.begin(); };
    std::list<CClientPhysics*>::const_iterator IterEnd(void) { return m_List.end(); };
    CClientPhysics*                            GetPhysics(btDiscreteDynamicsWorld* pDynamicsWorld);
    void DoPulse();
private:
    void AddToList(CClientPhysics* pPhysics) { m_List.push_back(pPhysics); };
    void RemoveFromList(CClientPhysics* pPhysics);
    void DeleteAll();

private:
    CClientManager* m_pManager;

    std::list<CClientPhysics*> m_List;
};

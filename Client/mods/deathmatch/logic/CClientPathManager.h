/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPathManager.h
 *  PURPOSE:     Path node entity manager class
 *
 *****************************************************************************/

class CClientPathManager;

#pragma once

#include "CClientPathNode.h"
#include "CClientManager.h"
#include <list>

class CClientPathManager
{
    friend class CClientManager;
    friend class CClientPathNode;

public:
    void DeleteAll();

    void DoPulse();
    void LinkNodes();
    void ReverseNodes(CClientPathNode* pPathNode);
    bool DetachEntity(CClientEntity* pEntity);

    unsigned int            Count() { return static_cast<unsigned int>(m_PathNodeList.size()); }
    static CClientPathNode* Get(ElementID ID);

    std::list<CClientPathNode*>::iterator IterBegin() { return m_PathNodeList.begin(); }
    std::list<CClientPathNode*>::iterator IterEnd() { return m_PathNodeList.end(); }

private:
    CClientPathManager(CClientManager* pManager);
    ~CClientPathManager();

    void AddToList(CClientPathNode* pPathNode) { m_PathNodeList.push_back(pPathNode); }
    void RemoveFromList(CClientPathNode* pPathNode);

    CClientManager*             m_pManager;
    std::list<CClientPathNode*> m_PathNodeList;
    bool                        m_bRemoveFromList;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDFFManager.h
 *  PURPOSE:     .dff model manager class
 *
 *****************************************************************************/

class CClientDFFManager;

#pragma once

#include <list>
#include "CClientDFF.h"

class CClientDFFManager
{
    friend class CClientDFF;

public:
    CClientDFFManager(class CClientManager* pManager);
    ~CClientDFFManager();

    void RemoveAll();
    bool Exists(CClientDFF* pDFF);

    CClientDFF* GetElementThatReplaced(unsigned short usModel, CClientDFF* pDontSearch = NULL);

    static bool IsReplacableModel(unsigned short usModel);
    bool        RestoreModel(unsigned short usModel);

    std::list<CClientDFF*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientDFF*>::const_iterator IterEnd() { return m_List.end(); }

private:
    void AddToList(CClientDFF* pDFF) { m_List.push_back(pDFF); }
    void RemoveFromList(CClientDFF* pDFF);

    class CClientObjectManager*  m_pObjectManager;
    class CClientVehicleManager* m_pVehicleManager;

    std::list<CClientDFF*> m_List;
    bool                   m_bRemoveFromList;
};

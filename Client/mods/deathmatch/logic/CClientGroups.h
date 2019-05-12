/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientGroups.h
 *  PURPOSE:     Dummy grouping class
 *
 *****************************************************************************/

#pragma once

#include "CClientDummy.h"
#include "CClientEntity.h"
#include <list>

class CClientGroups
{
    friend class CClientDummy;

public:
    CClientGroups() { m_bDontRemoveFromList = false; };
    ~CClientGroups() { DeleteAll(); };

    void DeleteAll();

private:
    void AddToList(CClientDummy* pDummy) { m_List.push_back(pDummy); };
    void RemoveFromList(CClientDummy* pDummy);

    std::list<CClientDummy*> m_List;
    bool                     m_bDontRemoveFromList;
};

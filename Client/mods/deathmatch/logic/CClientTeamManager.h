/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTeamManager.h
 *  PURPOSE:     Team entity manager class header
 *
 *****************************************************************************/

class CClientTeamManager;

#pragma once

#include <list>
#include "CClientTeam.h"

class CClientTeamManager
{
public:
    CClientTeamManager();
    ~CClientTeamManager();

    void AddToList(CClientTeam* pTeam) { m_List.push_back(pTeam); }
    void RemoveFromList(CClientTeam* pTeam);
    void RemoveAll();
    bool Exists(CClientTeam* pTeam);

    static CClientTeam* GetTeam(ElementID ID);
    CClientTeam*        GetTeam(const char* szName);

    std::list<CClientTeam*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientTeam*>::const_iterator IterEnd() { return m_List.end(); }

private:
    std::list<CClientTeam*> m_List;
    bool                    m_bRemoveFromList;
};

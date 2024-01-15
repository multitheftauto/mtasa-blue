/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeamManager.h
 *  PURPOSE:     Team element manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CTeamManager;

#pragma once

#include <list>
#include "CTeam.h"

class CTeamManager
{
public:
    CTeamManager();
    ~CTeamManager();

    void AddToList(CTeam* pTeam) noexcept { m_List.push_back(pTeam); }
    void RemoveFromList(CTeam* pTeam);
    void RemoveAll();

    CTeam* GetTeam(const char* szName) const noexcept;
    bool   Exists(CTeam* pTeam) const noexcept;
    CTeam* Create(CElement* pParent, char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);
    CTeam* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);

    std::list<CTeam*>::iterator begin() noexcept { return m_List.begin(); }
    std::list<CTeam*>::iterator end() noexcept { return m_List.end(); }

    std::list<CTeam*>::const_iterator begin() const noexcept { return m_List.cbegin(); }
    std::list<CTeam*>::const_iterator end() const noexcept { return m_List.cend(); }

private:
    std::list<CTeam*> m_List;
};

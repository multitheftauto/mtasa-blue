/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeamManager.h
 *  PURPOSE:     Team element manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    void AddToList(CTeam* pTeam) { m_List.push_back(pTeam); }
    void RemoveFromList(CTeam* pTeam);
    void RemoveAll();

    CTeam* GetTeam(const char* szName);
    bool   Exists(CTeam* pTeam);
    CTeam* Create(CElement* pParent, char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);
    CTeam* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);

    std::list<CTeam*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CTeam*>::const_iterator IterEnd() { return m_List.end(); }

private:
    std::list<CTeam*> m_List;
};

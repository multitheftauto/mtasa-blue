/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeamManager.cpp
 *  PURPOSE:     Team element manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTeamManager.h"
#include "Utils.h"

CTeamManager::CTeamManager()
{
}

CTeamManager::~CTeamManager()
{
    RemoveAll();
}

void CTeamManager::RemoveFromList(CTeam* pTeam)
{
    m_List.remove(pTeam);
}

void CTeamManager::RemoveAll()
{
    DeletePointersAndClearList(m_List);
}

CTeam* CTeamManager::GetTeam(const char* szName)
{
    if (szName == NULL)
        return NULL;

    list<CTeam*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        if (strcmp((*iter)->GetTeamName(), szName) == 0)
            return *iter;
    }

    return NULL;
}

bool CTeamManager::Exists(CTeam* pTeam)
{
    return ListContains(m_List, pTeam);
}

CTeam* CTeamManager::Create(CElement* pParent, char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    CTeam* const pTeam = new CTeam(this, pParent, szName, ucRed, ucGreen, ucBlue);

    if (pTeam->GetID() == INVALID_ELEMENT_ID)
    {
        delete pTeam;
        return nullptr;
    }

    return pTeam;
}

CTeam* CTeamManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CTeam* const pTeam = new CTeam(this, pParent);

    if (pTeam->GetID() == INVALID_ELEMENT_ID || !pTeam->LoadFromCustomData(pEvents, Node))
    {
        delete pTeam;
        return nullptr;
    }

    return pTeam;
}

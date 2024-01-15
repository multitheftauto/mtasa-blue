/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeamManager.cpp
 *  PURPOSE:     Team element manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

CTeam* CTeamManager::GetTeam(const char* szName) const noexcept
{
    if (!szName)
        return nullptr;

    for (const auto& pTeam : m_List)
    {
        if (strcmp(pTeam->GetTeamName(), szName))
            continue;
        return pTeam;
    }

    return nullptr;
}

bool CTeamManager::Exists(CTeam* pTeam) const noexcept
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

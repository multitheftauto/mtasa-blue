/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTeam.cpp
 *  PURPOSE:     Team entity class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientTeam::CClientTeam(CClientManager* pManager, ElementID ID, const char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
    : ClassInit(this), CClientEntity(ID)
{
    m_pManager = pManager;
    m_pTeamManager = pManager->GetTeamManager();

    SetTypeName("team");

    SetTeamName(szName);
    SetColor(ucRed, ucGreen, ucBlue);
    SetFriendlyFire(true);

    m_pTeamManager->AddToList(this);
}

CClientTeam::~CClientTeam()
{
    RemoveAll();
    Unlink();
}

void CClientTeam::Unlink()
{
    m_pTeamManager->RemoveFromList(this);
}

void CClientTeam::AddPlayer(CClientPlayer* pPlayer, bool bChangePlayer)
{
    m_List.push_back(pPlayer);
    if (bChangePlayer)
        pPlayer->SetTeam(this, false);
}

void CClientTeam::RemovePlayer(CClientPlayer* pPlayer, bool bChangePlayer)
{
    if (!m_List.empty())
        m_List.remove(pPlayer);

    if (bChangePlayer)
        pPlayer->SetTeam(NULL, false);
}

void CClientTeam::RemoveAll()
{
    list<CClientPlayer*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        (*iter)->SetTeam(NULL, false);
    }
    m_List.clear();
}

std::vector<CClientPlayer*> CClientTeam::GetPlayers() const
{
    std::vector<CClientPlayer*> players;

    for (auto iter = IterBegin(); iter != IterEnd(); ++iter)
    {
        if (!(*iter)->IsBeingDeleted())
            players.push_back(*iter);
    }

    return players;
}

bool CClientTeam::Exists(CClientPlayer* pPlayer)
{
    list<CClientPlayer*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        if (*iter == pPlayer)
            return true;
    }

    return false;
}

void CClientTeam::SetTeamName(const char* szName)
{
    if (szName)
        m_strTeamName.AssignLeft(szName, MAX_TEAM_NAME_LENGTH);
    else
        m_strTeamName.clear();
}

void CClientTeam::GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue)
{
    ucRed = m_ucRed;
    ucGreen = m_ucGreen;
    ucBlue = m_ucBlue;
}

void CClientTeam::SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    m_ucRed = ucRed;
    m_ucGreen = ucGreen;
    m_ucBlue = ucBlue;
}

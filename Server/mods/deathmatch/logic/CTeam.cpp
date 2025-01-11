/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeam.cpp
 *  PURPOSE:     Team element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTeam.h"
#include "CLogger.h"
#include "Utils.h"

CTeam::CTeam(CTeamManager* pTeamManager, CElement* pParent, const char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
    : CElement(pParent)
{
    m_pTeamManager = pTeamManager;

    m_iType = CElement::TEAM;
    SetTypeName("team");

    SetTeamName(szName);
    SetColor(ucRed, ucGreen, ucBlue);
    SetFriendlyFire(true);

    m_pTeamManager->AddToList(this);
}

CTeam::~CTeam()
{
    RemoveAllPlayers();
    Unlink();
}

void CTeam::Unlink()
{
    m_pTeamManager->RemoveFromList(this);
}

bool CTeam::ReadSpecialData(const int iLine)
{
    // Grab the "name"
    char szTemp[MAX_TEAM_NAME];
    if (GetCustomDataString("name", szTemp, MAX_TEAM_NAME, true))
    {
        SetTeamName(szTemp);
    }
    else
    {
        CLogger::ErrorPrintf("Bad/missing name' attribute in <team> (line %d)\n", iLine);
        return false;
    }

    // Grab the "color" data
    int iTemp;
    if (GetCustomDataString("color", szTemp, 64, true))
    {
        // Convert it to RGBA
        unsigned char ucAlpha;
        if (!XMLColorToInt(szTemp, m_ucRed, m_ucGreen, m_ucBlue, ucAlpha))
        {
            CLogger::ErrorPrintf("Bad 'color' value specified in <team> (line %d)\n", iLine);
            return false;
        }
    }
    else
    {
        if (GetCustomDataInt("colorR", iTemp, true))
            m_ucRed = static_cast<unsigned char>(iTemp);
        if (GetCustomDataInt("colorG", iTemp, true))
            m_ucGreen = static_cast<unsigned char>(iTemp);
        if (GetCustomDataInt("colorB", iTemp, true))
            m_ucBlue = static_cast<unsigned char>(iTemp);
    }

    if (!GetCustomDataBool("friendlyfire", m_bFriendlyFire, true))
        m_bFriendlyFire = true;

    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    return true;
}

void CTeam::SetTeamName(const char* szName)
{
    if (szName)
        m_strTeamName.AssignLeft(szName, MAX_TEAM_NAME_LENGTH);
    else
        m_strTeamName = "";
}

void CTeam::AddPlayer(CPlayer* pPlayer, bool bChangePlayer)
{
    m_Players.push_back(pPlayer);
    if (bChangePlayer)
        pPlayer->SetTeam(this, false);
}

void CTeam::RemovePlayer(CPlayer* pPlayer, bool bChangePlayer)
{
    m_Players.remove(pPlayer);
    if (bChangePlayer)
        pPlayer->SetTeam(NULL, false);
}

void CTeam::RemoveAllPlayers()
{
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); ++iter)
    {
        (*iter)->SetTeam(NULL, false);
    }
    m_Players.clear();
}

std::vector<CPlayer*> CTeam::GetPlayers() const
{
    std::vector<CPlayer*> players;

    for (auto iter = m_Players.begin(); iter != m_Players.end(); ++iter)
    {
        if (!(*iter)->IsBeingDeleted())
            players.push_back(*iter);
    }

    return players;
}

void CTeam::GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue)
{
    ucRed = m_ucRed;
    ucBlue = m_ucBlue;
    ucGreen = m_ucGreen;
}

void CTeam::SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    m_ucRed = ucRed;
    m_ucBlue = ucBlue;
    m_ucGreen = ucGreen;
}

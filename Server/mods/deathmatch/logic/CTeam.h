/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTeam.h
 *  PURPOSE:     Team element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CTeam;

#pragma once

#include "CElement.h"
#include "CPlayer.h"
#include "CTeamManager.h"

#define MAX_TEAM_NAME 128

class CTeam final : public CElement
{
    friend class CTeamManager;

public:
    CTeam(CTeamManager* pTeamManager, CElement* pParent, const char* szName = NULL, std::uint8_t ucRed = 0, std::uint8_t ucGreen = 0,
          std::uint8_t ucBlue = 0);
    ~CTeam();

    void Unlink();

    const char* GetTeamName() { return m_strTeamName; }
    void        SetTeamName(const char* szName);

    void AddPlayer(CPlayer* pPlayer, bool bChangePlayer = false);
    void RemovePlayer(CPlayer* pPlayer, bool bChangePlayer = false);
    void RemoveAllPlayers();
    void GetPlayers(lua_State* luaVM);

    void GetColor(std::uint8_t& ucRed, std::uint8_t& ucGreen, std::uint8_t& ucBlue);
    void SetColor(std::uint8_t ucRed, std::uint8_t ucGreen, std::uint8_t ucBlue);

    bool GetFriendlyFire() { return m_bFriendlyFire; }
    void SetFriendlyFire(bool bFriendlyFire) { m_bFriendlyFire = bFriendlyFire; }

    std::uint32_t CountPlayers() { return static_cast<std::uint32_t>(m_Players.size()); }

    std::list<CPlayer*>::const_iterator PlayersBegin() { return m_Players.begin(); }
    std::list<CPlayer*>::const_iterator PlayersEnd() { return m_Players.end(); }

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CTeamManager* m_pTeamManager;

    SString             m_strTeamName;
    std::list<CPlayer*> m_Players;

    std::uint8_t m_ucRed;
    std::uint8_t m_ucGreen;
    std::uint8_t m_ucBlue;

    bool m_bFriendlyFire;
};

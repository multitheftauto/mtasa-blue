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
    CTeam(CTeamManager* pTeamManager, CElement* pParent, const char* szName = NULL, unsigned char ucRed = 0, unsigned char ucGreen = 0,
          unsigned char ucBlue = 0);
    ~CTeam();

    void Unlink();

    constexpr const char* GetTeamName() const noexcept { return m_strTeamName; }
    void                  SetTeamName(const char* szName);

    void AddPlayer(CPlayer* pPlayer, bool bChangePlayer = false);
    void RemovePlayer(CPlayer* pPlayer, bool bChangePlayer = false);
    void RemoveAllPlayers();
    void GetPlayers(lua_State* luaVM);

    void GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue);
    void SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);

    constexpr bool GetFriendlyFire() const noexcept { return m_bFriendlyFire; }
    constexpr void SetFriendlyFire(bool bFriendlyFire) noexcept { m_bFriendlyFire = bFriendlyFire; }

    std::size_t CountPlayers() const noexcept { return m_Players.size(); }

    std::list<CPlayer*>::iterator begin() noexcept { return m_Players.begin(); }
    std::list<CPlayer*>::iterator end() noexcept { return m_Players.end(); }
    
    std::list<CPlayer*>::const_iterator begin() const noexcept { return m_Players.cbegin(); }
    std::list<CPlayer*>::const_iterator end() const noexcept { return m_Players.cend(); }

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CTeamManager* m_pTeamManager;

    SString             m_strTeamName;
    std::list<CPlayer*> m_Players;

    unsigned char m_ucRed;
    unsigned char m_ucGreen;
    unsigned char m_ucBlue;

    bool m_bFriendlyFire;
};

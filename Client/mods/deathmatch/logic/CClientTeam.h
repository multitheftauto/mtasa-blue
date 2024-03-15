/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTeam.h
 *  PURPOSE:     Team entity class header
 *
 *****************************************************************************/

class CClientTeam;

#pragma once

#include "CClientEntity.h"
#include "CClientPlayer.h"
#include "CClientTeamManager.h"

class CClientTeam final : public CClientEntity
{
    DECLARE_CLASS(CClientTeam, CClientEntity)
    friend class CClientTeamManager;

public:
    CClientTeam(CClientManager* pManager, ElementID ID, const char* szName = NULL, std::uint8_t ucRed = 0, std::uint8_t ucGreen = 0,
                std::uint8_t ucBlue = 0);
    ~CClientTeam();

    eClientEntityType GetType() const { return CCLIENTTEAM; }

    void Unlink();

    std::uint8_t GetID() { return m_ucID; }

    const char* GetTeamName() { return m_strTeamName; }
    void        SetTeamName(const char* szName);

    void GetColor(std::uint8_t& ucRed, std::uint8_t& ucGreen, std::uint8_t& ucBlue);
    void SetColor(std::uint8_t ucRed, std::uint8_t ucGreen, std::uint8_t ucBlue);

    bool GetFriendlyFire() { return m_bFriendlyFire; }
    void SetFriendlyFire(bool bFriendlyFire) { m_bFriendlyFire = bFriendlyFire; }

    void AddPlayer(CClientPlayer* pPlayer, bool bChangePlayer = false);
    void RemovePlayer(CClientPlayer* pPlayer, bool bChangePlayer = false);
    void RemoveAll();

    bool Exists(CClientPlayer* pPlayer);

    std::list<CClientPlayer*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientPlayer*>::const_iterator IterEnd() { return m_List.end(); }

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; }
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; }

    std::uint32_t CountPlayers() { return static_cast<std::uint32_t>(m_List.size()); }

protected:
    CClientTeamManager* m_pTeamManager;

    std::uint8_t m_ucID;
    SString       m_strTeamName;

    std::uint8_t m_ucRed;
    std::uint8_t m_ucGreen;
    std::uint8_t m_ucBlue;

    bool m_bFriendlyFire;

    std::list<CClientPlayer*> m_List;

    CVector m_vecPosition;
};

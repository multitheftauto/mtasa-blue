/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerManager.cpp
 *  PURPOSE:     Player ped entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <numeric>
#include "net/SimHeaders.h"

CPlayerManager::~CPlayerManager()
{
    // Deallocate all players
    for (CPlayer* pPlayer : m_Players)
        delete pPlayer;
}

void CPlayerManager::DoPulse()
{
    PulseZombieCheck();

    for (CPlayer* pPlayer : m_Players)
        pPlayer->DoPulse();
}

void CPlayerManager::PulseZombieCheck()
{
    // Only check once a second
    if (m_ZombieCheckTimer.Get() < 1000)
        return;

    m_ZombieCheckTimer.Reset();

    for (CPlayer* pPlayer : m_Players)
    {
        if (!pPlayer->IsJoined())
        {
            // Remove any players that have been connected for very long (90 sec) but haven't reached the verifying step
            if (pPlayer->GetTimeSinceConnected() > 90000)
            {
                CLogger::LogPrintf("INFO: %s (%s) timed out during connect\n", pPlayer->GetNick(), pPlayer->GetSourceIP());
                g_pGame->QuitPlayer(*pPlayer, CClient::QUIT_QUIT, false);
            }
        }
        else
        {
            // Remove any players that are joined, but not sending sync and have incorrect connection info
            if (pPlayer->GetTimeSinceReceivedSync() > 20000)
            {
                if (!g_pRealNetServer->IsValidSocket(pPlayer->GetSocket()))
                {
                    CLogger::LogPrintf("INFO: %s (%s) connection gone away\n", pPlayer->GetNick(), pPlayer->GetSourceIP());
                    pPlayer->Send(CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::KICK, "hacky code"));
                    g_pGame->QuitPlayer(*pPlayer, CClient::QUIT_TIMEOUT);
                }
            }
        }
    }
}

CPlayer* CPlayerManager::Create(const NetServerPlayerID& PlayerSocket)
{
    // Check socket is free
    if (CPlayer* pOtherPlayer = Get(PlayerSocket))
    {
        CLogger::ErrorPrintf("Attempt to re-use existing connection for player '%s'\n", pOtherPlayer->GetName().c_str());
        return NULL;
    }

    // Create the new player
    return new CPlayer(this, m_pScriptDebugging, PlayerSocket); // Seems very safe
}

size_t CPlayerManager::CountJoined() const
{
    return std::accumulate(m_JoinedByBitStreamVer.begin(), m_JoinedByBitStreamVer.end(), 0,
        [](const auto& count, const auto& pair) { return count + pair.second.size(); }
    );
}

// Find player by fully matching nick. Todo: Partial nick
CPlayer* CPlayerManager::Get(const char* szNick, bool bCaseSensitive) const
{
    const auto iter = std::find_if(m_Players.begin(), m_Players.end(), [bCaseSensitive, szNick](CPlayer* pPlayer) {
        const char* szPlayerNick = pPlayer->GetNick();
        if (bCaseSensitive) 
            return strcmp(szNick, szPlayerNick) == 0; // Do a case sensitive compare
        else
            return stricmp(szNick, szPlayerNick) == 0; // Insensitive otherwise
    });
    return (iter == m_Players.end()) ? nullptr : *iter;
}

void CPlayerManager::BroadcastOnlyJoined(const CPacket& Packet, CPlayer* pSkip) const
{
    BroadcastJoinedIf(Packet,
        [=](CPlayer* pPlayer) { return pPlayer != pSkip; });
}

void CPlayerManager::BroadcastDimensionOnlyJoined(const CPacket& Packet, ushort usDimension, CPlayer* pSkip) const
{
    BroadcastJoinedIf(Packet,
        [=](CPlayer* pPlayer) { return pPlayer != pSkip && pPlayer->GetDimension() == usDimension; });
}

// Used for subscriber based setElementData
void CPlayerManager::BroadcastOnlySubscribed(const CPacket& Packet, CElement* pElement, const std::string& name, CPlayer* pSkip) const
{
    BroadcastJoinedIf(Packet,
        [=, &name](CPlayer* pPlayer) { return pPlayer != pSkip && pPlayer->IsSubscribed(pElement, name); });
}

    DoBroadcast(Packet, groupMap);
}

// Send one packet to a list of players
void CPlayerManager::Broadcast(const CPacket& Packet, const std::set<CPlayer*>& sendList)
{
    DoBroadcast(Packet, sendList);
}

// Send one packet to a list of players
void CPlayerManager::Broadcast(const CPacket& Packet, const std::list<CPlayer*>& sendList)
{
    DoBroadcast(Packet, sendList);
}

// Send one packet to a list of players
void CPlayerManager::Broadcast(const CPacket& Packet, const std::vector<CPlayer*>& sendList)
{
    DoBroadcast(Packet, sendList);
}

// Send one packet to a list of players
void CPlayerManager::Broadcast(const CPacket& Packet, const std::multimap<ushort, CPlayer*>& groupMap)
{
    DoBroadcast(Packet, groupMap);
}

bool CPlayerManager::IsValidPlayerModel(unsigned short usPlayerModel)
{
    return (usPlayerModel == 0 || usPlayerModel == 1 || usPlayerModel == 2 || usPlayerModel == 7 ||
            (usPlayerModel >= 9 && usPlayerModel != 208 && usPlayerModel != 149 && usPlayerModel != 119 && usPlayerModel != 86 && usPlayerModel != 74 &&
             usPlayerModel != 65 && usPlayerModel != 42 && usPlayerModel <= 272) ||
            (usPlayerModel >= 274 && usPlayerModel <= 288) || (usPlayerModel >= 290 && usPlayerModel <= 312));
}

void CPlayerManager::ClearElementData(CElement* pElement, const std::string& name)
{
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        CPlayer* pPlayer = *iter;
        pPlayer->UnsubscribeElementData(pElement, name);
    }
}

void CPlayerManager::ClearElementData(CElement* pElement)
{
    for (auto pPlayer : m_Players)
    {
        pPlayer->UnsubscribeElementData(pElement);
    }
}

void CPlayerManager::ResetAll()
{
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        (*iter)->Reset();
    }
}

void CPlayerManager::AddToList(CPlayer* pPlayer)
{
    for (std::list<CPlayer*>::const_iterator iter = m_Players.begin(); iter != m_Players.end(); iter++)
    {
        // Add other players to near/far lists
        pPlayer->AddPlayerToDistLists(*iter);

        // Add to other players near/far lists
        (*iter)->AddPlayerToDistLists(pPlayer);
    }

    assert(!m_Players.Contains(pPlayer));
    m_Players.push_back(pPlayer);
    MapSet(m_SocketPlayerMap, pPlayer->GetSocket(), pPlayer);
    assert(m_Players.size() == m_SocketPlayerMap.size());
}

void CPlayerManager::RemoveFromList(CPlayer* pPlayer)
{
    m_Players.remove(pPlayer);
    MapRemove(m_SocketPlayerMap, pPlayer->GetSocket());
    assert(!m_Players.Contains(pPlayer));
    assert(m_Players.size() == m_SocketPlayerMap.size());

    m_strLowestConnectedPlayerVersion.clear();
    for (std::list<CPlayer*>::const_iterator iter = m_Players.begin(); iter != m_Players.end(); iter++)
    {
        // Remove from other players near/far lists
        (*iter)->RemovePlayerFromDistLists(pPlayer);

        // Update lowest player version
        if ((*iter)->IsJoined() && ((*iter)->GetPlayerVersion() < m_strLowestConnectedPlayerVersion || m_strLowestConnectedPlayerVersion.empty()))
            m_strLowestConnectedPlayerVersion = (*iter)->GetPlayerVersion();
    }
    g_pGame->CalculateMinClientRequirement();
}

void CPlayerManager::OnPlayerJoin(CPlayer* pPlayer)
{
    if (pPlayer->GetPlayerVersion() < m_strLowestConnectedPlayerVersion || m_strLowestConnectedPlayerVersion.empty())
        m_strLowestConnectedPlayerVersion = pPlayer->GetPlayerVersion();
    g_pGame->CalculateMinClientRequirement();
}

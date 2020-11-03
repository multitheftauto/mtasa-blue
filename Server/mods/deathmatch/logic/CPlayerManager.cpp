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

void CPlayerManager::OnPlayerJoin(CPlayer* pPlayer)
{
    // Update min version
    if (pPlayer->GetPlayerVersion() < m_LowestJoinedPlayerVersion || m_LowestJoinedPlayerVersion.empty())
        m_LowestJoinedPlayerVersion = pPlayer->GetPlayerVersion();
    g_pGame->CalculateMinClientRequirement();

    m_JoinedPlayersMap[pPlayer->GetBitStreamVersion()].push_back(pPlayer);
}

void CPlayerManager::AddToList(CPlayer* pPlayer)
{
    dassert(m_Players.find(pPlayer) == m_Players.end()); // Make sure he's not in the set already
    m_Players.insert(pPlayer); // Before putting this into dassert, look at its definiton :-)

    dassert(!MapFind(m_SocketPlayerMap, pPlayer->GetSocket()));
    m_SocketPlayerMap[pPlayer->GetSocket()] = pPlayer;

    dassert(m_SocketPlayerMap.size() == m_Players.size());

    for (CPlayer* itPlayer : m_Players)
    {
        // Add other players to near/far lists
        pPlayer->AddPlayerToDistLists(itPlayer);

        // Add to other players near/far lists
        itPlayer->AddPlayerToDistLists(pPlayer);
    }
}

void CPlayerManager::RemoveFromList(CPlayer* pPlayer)
{
    m_Players.erase(pPlayer);
    m_SocketPlayerMap.erase(pPlayer->GetSocket());
    dassert(m_Players.size() == m_SocketPlayerMap.size());

    if (auto* players = MapFind(m_JoinedPlayersMap, pPlayer->GetBitStreamVersion()))
        ListRemoveFirst(*players, pPlayer);

    for (CPlayer* itPlayer : m_Players)
    {
        // Remove from other players near/far lists
        itPlayer->RemovePlayerFromDistLists(pPlayer);

        // Find lowest player version
        if (itPlayer->IsJoined())
        {
            if (pPlayer->GetPlayerVersion() < m_LowestJoinedPlayerVersion || m_LowestJoinedPlayerVersion.empty())
                m_LowestJoinedPlayerVersion = pPlayer->GetPlayerVersion();
        }
    }

    g_pGame->CalculateMinClientRequirement();
}

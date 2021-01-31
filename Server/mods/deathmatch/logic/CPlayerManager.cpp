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
#include <random>
#include "net/SimHeaders.h"

CPlayerManager::~CPlayerManager()
{
    // Deallocate all players
    // The code design is retarded, so the player is erased from
    // m_Players, and others thru the destructor, which means that
    // the set changes while iterating, which isn't a problem, since
    // in the case of CFastHashSet iterators aren't invalidated when
    // erase() is called.
    // TODO: Reverse manager - object relationship
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
    return new CPlayer(this, m_pScriptDebugging, PlayerSocket); // TODO: Use unique_ptr in m_Players, and return the pointer here
}

size_t CPlayerManager::CountJoined() const
{
    return m_JoinedByBitStreamVer.CountJoined();
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

CPlayer* CPlayerManager::GetRandom() const noexcept
{
    // Optimized random player selection
     std::mt19937 rndgen{ std::random_device{}() };

    // Select a random player group
    std::uniform_int_distribution<size_t> joinedDistribution(0, m_JoinedByBitStreamVer.CountUniqueVersions() - 1);
    const auto it = std::next(m_JoinedByBitStreamVer.begin(), joinedDistribution(rndgen));

    // Select a random player within it
    std::uniform_int_distribution<size_t> playerGroupDistribution(0, it->second.size() - 1);
    return it->second[playerGroupDistribution(rndgen)];
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

void CPlayerManager::ClearElementData(CElement* pElement, const std::string& name)
{
    for (CPlayer* pPlayer : m_Players)
        pPlayer->UnsubscribeElementData(pElement, name);
}

void CPlayerManager::ClearElementData(CElement* pElement)
{
    for (CPlayer* pPlayer : m_Players)
        pPlayer->UnsubscribeElementData(pElement);
}

void CPlayerManager::ResetAll()
{
    for (CPlayer* pPlayer : m_Players)
        pPlayer->Reset();
}

void CPlayerManager::OnPlayerJoin(CPlayer* pPlayer)
{
    // Update min version
    if (pPlayer->GetPlayerVersion() < m_LowestJoinedPlayerVersion || m_LowestJoinedPlayerVersion.empty())
        m_LowestJoinedPlayerVersion = pPlayer->GetPlayerVersion();
    g_pGame->CalculateMinClientRequirement();

    m_JoinedByBitStreamVer.Insert(pPlayer);
    dassert(m_JoinedByBitStreamVer.CountJoined() <= m_Players.size());
}

void CPlayerManager::AddToList(CPlayer* pPlayer)
{
    // Do this before adding the player to the list. 
    // (Otherwise it'll trigger the dassert in AddPlayerToDistLists)
    for (CPlayer* itPlayer : m_Players)
    {
        // Add other players to near/far lists
        pPlayer->AddPlayerToDistLists(itPlayer);

        // Add to other players near/far lists
        itPlayer->AddPlayerToDistLists(pPlayer);
    }

    dassert(m_Players.find(pPlayer) == m_Players.end());
    m_Players.insert(pPlayer);

    dassert(!MapFind(m_SocketPlayerMap, pPlayer->GetSocket()));
    m_SocketPlayerMap[pPlayer->GetSocket()] = pPlayer;

    dassert(m_SocketPlayerMap.size() == m_Players.size());
    dassert(m_JoinedByBitStreamVer.CountJoined() <= m_Players.size());
}

void CPlayerManager::RemoveFromList(CPlayer* pPlayer)
{
    // This is called from CPlayer::~CPlayer(), which might be called from our destructor.
    // Which means that modifying m_Players might be dangerous (in the case of google_sparse_set[AKA CFastHashSet])
    // erase operations do not invalidate iterators.

    dassert(m_Players.find(pPlayer) != m_Players.end());
    m_Players.erase(pPlayer);

    dassert(MapFind(m_SocketPlayerMap, pPlayer->GetSocket()));
    m_SocketPlayerMap.erase(pPlayer->GetSocket());

    dassert(m_Players.size() == m_SocketPlayerMap.size());

    m_JoinedByBitStreamVer.Erase(pPlayer);
    dassert(m_JoinedByBitStreamVer.CountJoined() <= m_Players.size());

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

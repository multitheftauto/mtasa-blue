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
#include "CPlayerManager.h"
#include "packets/CPlayerDisconnectedPacket.h"
#include "CGame.h"
#include "net/SimHeaders.h"

CPlayerManager::CPlayerManager()
{
    // Init
    m_pScriptDebugging = NULL;
    m_ZombieCheckTimer.SetUseModuleTickCount(true);
}

CPlayerManager::~CPlayerManager()
{
    DeleteAll();
}

void CPlayerManager::DoPulse()
{
    PulseZombieCheck();

    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        (*iter)->DoPulse();
    }
}

void CPlayerManager::PulseZombieCheck()
{
    // Only check once a second
    if (m_ZombieCheckTimer.Get() < 1000)
        return;
    m_ZombieCheckTimer.Reset();

    for (std::list<CPlayer*>::const_iterator iter = m_Players.begin(); iter != m_Players.end(); iter++)
    {
        CPlayer* pPlayer = *iter;

        if (pPlayer->IsJoined() == false)
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
    CPlayer* pOtherPlayer = MapFindRef(m_SocketPlayerMap, PlayerSocket);
    if (pOtherPlayer)
    {
        CLogger::ErrorPrintf("Attempt to re-use existing connection for player '%s'\n", pOtherPlayer->GetName().c_str());
        return NULL;
    }

    // Create the new player
    return new CPlayer(this, m_pScriptDebugging, PlayerSocket);
}

unsigned int CPlayerManager::CountJoined()
{
    // Count each ingame player
    unsigned int                   uiCount = 0;
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        if ((*iter)->IsJoined())
        {
            ++uiCount;
        }
    }

    // Return the count
    return uiCount;
}

bool CPlayerManager::Exists(CPlayer* pPlayer)
{
    return m_Players.Contains(pPlayer);
}

CPlayer* CPlayerManager::Get(const NetServerPlayerID& PlayerSocket)
{
    return MapFindRef(m_SocketPlayerMap, PlayerSocket);
}

CPlayer* CPlayerManager::Get(const char* szNick, bool bCaseSensitive)
{
    // Try to find it in our list
    const char*                    szTemp;
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        // Grab the nick pointer
        szTemp = (*iter)->GetNick();
        if (szTemp)
        {
            // Do they equal?
            if ((bCaseSensitive && strcmp(szNick, szTemp) == 0) || (!bCaseSensitive && stricmp(szNick, szTemp) == 0))
            {
                return *iter;
            }
        }
    }

    // If not, return NULL
    return NULL;
}

CPlayer* CPlayerManager::GetBySerial(const std::string_view serial) const noexcept
{
    for (const auto& player : m_Players)
    {
        if (player->GetSerial() == serial)
            return player;
    }

    return nullptr;
}

void CPlayerManager::DeleteAll()
{
    // Delete all the items in the list
    while (!m_Players.empty())
        delete *m_Players.begin();
}

size_t CPlayerManager::BroadcastOnlyJoined(const CPacket& Packet, CPlayer* pSkip)
{
    // Make a list of players to send this packet to
    CSendList sendList;

    // Send the packet to each ingame player on the server except the skipped one
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        CPlayer* pPlayer = *iter;
        if (pPlayer != pSkip && pPlayer->IsJoined())
        {
            sendList.push_back(pPlayer);
        }
    }

    CPlayerManager::Broadcast(Packet, sendList);

    return sendList.size();
}

size_t CPlayerManager::BroadcastDimensionOnlyJoined(const CPacket& Packet, ushort usDimension, CPlayer* pSkip)
{
    // Make a list of players to send this packet to
    CSendList sendList;

    // Send the packet to each ingame player on the server except the skipped one
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        CPlayer* pPlayer = *iter;
        if (pPlayer != pSkip && pPlayer->IsJoined())
        {
            if (pPlayer->GetDimension() == usDimension)
                sendList.push_back(pPlayer);
        }
    }

    CPlayerManager::Broadcast(Packet, sendList);

    return sendList.size();
}

size_t CPlayerManager::BroadcastOnlySubscribed(const CPacket& Packet, CElement* pElement, const char* szName, CPlayer* pSkip)
{
    // Make a list of players to send this packet to
    CSendList sendList;

    // Send the packet to each ingame player on the server except the skipped one
    list<CPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        CPlayer* pPlayer = *iter;
        if (pPlayer != pSkip && pPlayer->IsJoined() && pPlayer->IsSubscribed(pElement, szName))
        {
            sendList.push_back(pPlayer);
        }
    }

    CPlayerManager::Broadcast(Packet, sendList);

    return sendList.size();
}

// Send one packet to a list of players, grouped by bitstream version
static void DoBroadcast(const CPacket& Packet, const std::multimap<ushort, CPlayer*>& groupMap)
{
    if (!CNetBufferWatchDog::CanSendPacket(Packet.GetPacketID()))
        return;

    // Use the flags to determine how to send it
    NetServerPacketReliability Reliability;
    unsigned long              ulFlags = Packet.GetFlags();
    if (ulFlags & PACKET_RELIABLE)
    {
        if (ulFlags & PACKET_SEQUENCED)
        {
            Reliability = PACKET_RELIABILITY_RELIABLE_ORDERED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_RELIABLE;
        }
    }
    else
    {
        if (ulFlags & PACKET_SEQUENCED)
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE_SEQUENCED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE;
        }
    }
    NetServerPacketPriority packetPriority = PACKET_PRIORITY_MEDIUM;
    if (ulFlags & PACKET_HIGH_PRIORITY)
    {
        packetPriority = PACKET_PRIORITY_HIGH;
    }
    else if (ulFlags & PACKET_LOW_PRIORITY)
    {
        packetPriority = PACKET_PRIORITY_LOW;
    }

    // For each bitstream version, make and send a packet
    typedef std::multimap<ushort, CPlayer*>::const_iterator mapIter;
    mapIter                                                 m_it, s_it;
    for (m_it = groupMap.begin(); m_it != groupMap.end(); m_it = s_it)
    {
        ushort usBitStreamVersion = (*m_it).first;

        // Allocate a bitstream
        NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream(usBitStreamVersion);

        // Write the content
        if (Packet.Write(*pBitStream))
        {
            g_pGame->SendPacketBatchBegin(Packet.GetPacketID(), pBitStream);

            // For each player, send the packet
            const pair<mapIter, mapIter> keyRange = groupMap.equal_range(usBitStreamVersion);
            for (s_it = keyRange.first; s_it != keyRange.second; ++s_it)
            {
                CPlayer* pPlayer = s_it->second;
                dassert(usBitStreamVersion == pPlayer->GetBitStreamVersion());
                g_pGame->SendPacket(Packet.GetPacketID(), pPlayer->GetSocket(), pBitStream, false, packetPriority, Reliability, Packet.GetPacketOrdering());
            }

            g_pGame->SendPacketBatchEnd();
        }
        else
        {
            // Skip
            const pair<mapIter, mapIter> keyRange = groupMap.equal_range(usBitStreamVersion);
            for (s_it = keyRange.first; s_it != keyRange.second; ++s_it)
            {
            }
        }

        // Destroy the bitstream
        g_pNetServer->DeallocateNetServerBitStream(pBitStream);
    }
}

// Send one packet to a list of players
template <class T>
static void DoBroadcast(const CPacket& Packet, const T& sendList)
{
    // Group players by bitstream version
    std::multimap<ushort, CPlayer*> groupMap;
    for (typename T::const_iterator iter = sendList.begin(); iter != sendList.end(); ++iter)
    {
        CPlayer* pPlayer = *iter;
        MapInsert(groupMap, pPlayer->GetBitStreamVersion(), pPlayer);
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

bool CPlayerManager::IsValidPlayerModel(unsigned short model)
{
    if (model > 312)
        return false;            // TODO: On client side maybe check if a model was allocated with engineRequestModel and it is a ped

    switch (model)
    {
        case 74:            // Missing skin
        case 149:
        case 208:
            return false;
        default:
            return true;
    }
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

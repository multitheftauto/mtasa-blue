/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerManager.h
 *  PURPOSE:     Player ped entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPlayerManager;

#pragma once

#include "CCommon.h"
#include "packets/CPacket.h"
#include "CPlayer.h"
#include "../Config.h"
#include <net/CNetBufferWatchDog.h>

class CPlayerManager
{
    friend class CPlayer;

protected:
    // Some protected typedefs for code readability
    using BitStreamVersion = unsigned short;            // Todo: move this to it's appropriate place
    using PlayersByBitStreamVersionMap = std::unordered_map<BitStreamVersion, std::vector<CPlayer*>>;

public:
    // Send one packet to a list of players
    template <class T, class Pred_t>
    static void BroadcastIf(const CPacket& Packet, const T& sendList, Pred_t&& pred)
    {
        if constexpr (std::is_base_of_v<PlayersByBitStreamVersionMap, T>) // Can we call it as is?
            DoBroadcast(Packet, sendList, std::forward<Pred_t>(pred));

        else // Sadly not, conversion needed..
        {
            // Group players by bitstream version
            PlayersByBitStreamVersionMap groupMap;

            for (CPlayer* pPlayer : sendList)
                groupMap[pPlayer->GetBitStreamVersion()].push_back(pPlayer);

            DoBroadcast(Packet, groupMap, std::forward<Pred_t>(pred));
        }
    }

    template <class T>
    static void Broadcast(const CPacket& Packet, T&& sendList)
    {
        BroadcastIf(Packet, std::forward<T>(sendList), [](CPlayer*) { return true; });
    }

    static bool IsValidPlayerModel(unsigned short usPlayerModel)
    {
        return (usPlayerModel == 0 || usPlayerModel == 1 || usPlayerModel == 2 || usPlayerModel == 7 ||
            (usPlayerModel >= 9 && usPlayerModel != 208 && usPlayerModel != 149 && usPlayerModel != 119 && usPlayerModel != 86 && usPlayerModel != 74 &&
            usPlayerModel != 65 && usPlayerModel != 42 && usPlayerModel <= 272) ||
            (usPlayerModel >= 274 && usPlayerModel <= 288) || (usPlayerModel >= 290 && usPlayerModel <= 312));
    }
public:
    CPlayerManager() { m_ZombieCheckTimer.SetUseModuleTickCount(true); }
    ~CPlayerManager();

    void DoPulse();
    void PulseZombieCheck();

    void SetScriptDebugging(class CScriptDebugging* pScriptDebugging) noexcept { m_pScriptDebugging = pScriptDebugging; };

    CPlayer* Create(const NetServerPlayerID& PlayerSocket);

    unsigned int Count() { return static_cast<unsigned int>(m_Players.size()); }
    unsigned int CountJoined();
    bool         Exists(CPlayer* pPlayer);

    CPlayer* Get(const NetServerPlayerID& PlayerSocket);
    CPlayer* Get(const char* szNick, bool bCaseSensitive = false);

    std::list<CPlayer*>::const_iterator IterBegin() { return m_Players.begin(); };
    std::list<CPlayer*>::const_iterator IterEnd() { return m_Players.end(); };

    //
    // Member broadcast functions
    //

    // Alias to DoBroadcastIf with joined player map
    template <class Predicate_t>
    void BroadcastJoinedIf(const CPacket& Packet, Predicate_t&& Predicate) const
    {
        DoBroadcastIf(Packet, m_JoinedByBitStreamVer, std::forward<Predicate_t>(Predicate));
    }

    // Aliases to the above function with predefined predicates
    void BroadcastOnlyJoined(const CPacket& Packet, CPlayer* pSkip = nullptr) const;
    void BroadcastDimensionOnlyJoined(const CPacket& Packet, ushort usDimension, CPlayer* pSkip = nullptr) const;
    void BroadcastOnlySubscribed(const CPacket& Packet, CElement* pElement, const std::string& name, CPlayer* pSkip = nullptr) const;

    // Subscriber based elementdata things
    void ClearElementData(CElement* pElement, const std::string& name);
    void ClearElementData(CElement* pElement);

    void ResetAll();
    void OnPlayerJoin(CPlayer* pPlayer);

    const CMtaVersion& GetLowestConnectedPlayerVersion() { return m_LowestJoinedPlayerVersion; }
private:

    // Send one packet to a list of players, grouped by bitstream version
    // The predicate works like any std function with a predicate:
    // Eg.: It takes in a `CPlayer*` as its first (and only) argument
    // And only if it (the predicate) returns true, the packet is sent to the player.
    template<class Predicate_t>
    static void DoBroadcastIf(const CPacket& Packet, const PlayersByBitStreamVersionMap& playersByBitStreamVer, Predicate_t predicate)
    {
        if (!CNetBufferWatchDog::CanSendPacket(Packet.GetPacketID()))
            return;

        // Use the flags to determine how to send it
        const auto reliability = Packet.GetNetServerReliability();
        const auto priority = Packet.GetNetServerPriority();

        // For each bitstream version, make and send a packet
        for (auto&& [bsver, players] : playersByBitStreamVer)
        {
            // Allocate a bitstream
            NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream(bsver);

            if (Packet.Write(*pBitStream))
            {
                extern CGame* g_pGame;
                g_pGame->SendPacketBatchBegin(Packet.GetPacketID(), pBitStream);

                for (CPlayer* pPlayer : players)
                {
                    if (!predicate(pPlayer))
                        continue;

                    dassert(bsver == pPlayer->GetBitStreamVersion());
                    g_pGame->SendPacket(Packet.GetPacketID(), pPlayer->GetSocket(), pBitStream, FALSE, priority, reliability, Packet.GetPacketOrdering());
                }

                g_pGame->SendPacketBatchEnd();
            }

            // Destroy the bitstream
            g_pNetServer->DeallocateNetServerBitStream(pBitStream);
        }
    }

    // Wrapper around the above function so it can be called without a predicate
    static void DoBroadcast(const CPacket& Packet, const PlayersByBitStreamVersionMap& playersByBitStreamVer)
    {
        DoBroadcastIf(Packet, playersByBitStreamVer,
            [](CPlayer*) { return true; }); // Placeholder lambda
    }

private:
    void AddToList(CPlayer* pPlayer);
    void RemoveFromList(CPlayer* pPlayer);

    class CScriptDebugging* m_pScriptDebugging = nullptr;

    CFastHashSet<CPlayer*>                    m_Players; // All joined players
    CFastHashMap<NetServerPlayerID, CPlayer*> m_SocketPlayerMap;
    PlayersByBitStreamVersionMap              m_JoinedByBitStreamVer; // Players joined into groups by bit stream version for easy packet sending
    CMtaVersion                               m_LowestJoinedPlayerVersion;
    CElapsedTime                              m_ZombieCheckTimer;
};

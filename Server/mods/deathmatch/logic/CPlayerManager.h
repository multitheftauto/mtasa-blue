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

#include "CGame.h" // Todo: Somehow remove this.. Needed for SendPacketBatchBegin
#include "packets/CPacket.h"
#include "CSendList.h"
#include "CPlayer.h"
#include "../Config.h"
#include <net/CNetBufferWatchDog.h>

class CPlayerManager
{
    friend class CPlayer;

public:
    // Send one packet to a list of players
    // CSendList can be used, and is prefferred over anything else
    // as CSendList is already pre constructed
    // The list must be sequencial
    // So for example.: map, wont work, because its value_type is a pair, not CPlayer*
    // list, vector, set, and other iterable containers will work
    template <class T, class Pred_t>
    static void BroadcastIf(const CPacket& Packet, T&& sendList, Pred_t&& pred)
    {
        DoBroadcastIf(Packet, std::forward<T>(sendList), std::forward<Pred_t>(pred));
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

    size_t Count() const noexcept { return m_Players.size(); }
    size_t CountJoined() const;
    size_t HasAnyJoined() const noexcept { return m_JoinedByBitStreamVer.CountUniqueVersions() > 0; }

    bool   Exists(CPlayer* pPlayer) const noexcept { return m_Players.find(pPlayer) != m_Players.end(); }

    CPlayer* Get(const NetServerPlayerID& PlayerSocket) const noexcept { return MapFindRef(m_SocketPlayerMap, PlayerSocket); }
    CPlayer* Get(const char* szNick, bool bCaseSensitive = false) const;

    const auto& GetAllPlayers() const noexcept { return m_Players; }

    template<class UnaryFun_t>
    void IterateJoined(UnaryFun_t UnaryFun) const noexcept
    {
        for (const auto& [k, players] : m_JoinedByBitStreamVer)
            for (CPlayer* pPlayer : players)
                UnaryFun(pPlayer);
    }

    CPlayer* GetRandom() const noexcept;

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
    void BroadcastAll(const CPacket& Packet, CPlayer* pSkip = nullptr) const
    {
        DoBroadcastIf(Packet, m_Players,
            [=](CPlayer* pPlayer) {return pSkip != pPlayer; });
    }

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
    static void DoBroadcastIf(const CPacket& Packet, const CSendList& playersByBitStreamVer, Predicate_t predicate)
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
    static void DoBroadcast(const CPacket& Packet, const CSendList& playersByBitStreamVer)
    {
        DoBroadcastIf(Packet, playersByBitStreamVer,
            [](CPlayer*) { return true; }); // Placeholder lambda
    }

private:
    void AddToList(CPlayer* pPlayer);
    void RemoveFromList(CPlayer* pPlayer);

    class CScriptDebugging* m_pScriptDebugging = nullptr;

    CFastHashSet<CPlayer*>                    m_Players; // All players connected to the server
    CFastHashMap<NetServerPlayerID, CPlayer*> m_SocketPlayerMap; // All players connected to the server socket -> player map
    CSendList                                 m_JoinedByBitStreamVer; // JOINED players only (connected != joined)
    CMtaVersion                               m_LowestJoinedPlayerVersion;
    CElapsedTime                              m_ZombieCheckTimer;
};

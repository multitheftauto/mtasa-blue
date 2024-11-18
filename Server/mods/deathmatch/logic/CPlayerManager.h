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

class CPlayerManager
{
    friend class CPlayer;

public:
    CPlayerManager();
    ~CPlayerManager();

    void DoPulse();
    void PulseZombieCheck();

    void SetScriptDebugging(class CScriptDebugging* pScriptDebugging) { m_pScriptDebugging = pScriptDebugging; };

    CPlayer* Create(const NetServerPlayerID& PlayerSocket);
    void     DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_Players.size()); }
    unsigned int CountJoined();
    bool         Exists(CPlayer* pPlayer);

    CPlayer* Get(const NetServerPlayerID& PlayerSocket);
    CPlayer* Get(const char* szNick, bool bCaseSensitive = false);
    CPlayer* GetBySerial(const std::string_view serial) const noexcept;

    std::list<CPlayer*>::const_iterator IterBegin() { return m_Players.begin(); };
    std::list<CPlayer*>::const_iterator IterEnd() { return m_Players.end(); };

    size_t BroadcastOnlyJoined(const CPacket& Packet, CPlayer* pSkip = NULL);
    size_t BroadcastDimensionOnlyJoined(const CPacket& Packet, ushort usDimension, CPlayer* pSkip = NULL);
    size_t BroadcastOnlySubscribed(const CPacket& Packet, CElement* pElement, const char* szName, CPlayer* pSkip = NULL);

    static void Broadcast(const CPacket& Packet, const std::set<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::list<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::vector<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::multimap<ushort, CPlayer*>& groupMap);

    static bool IsValidPlayerModel(unsigned short model);

    void ClearElementData(CElement* pElement, const std::string& name);
    void ClearElementData(CElement* pElement);

    void               ResetAll();
    void               OnPlayerJoin(CPlayer* pPlayer);
    const CMtaVersion& GetLowestConnectedPlayerVersion() { return m_strLowestConnectedPlayerVersion; }

private:
    void AddToList(CPlayer* pPlayer);
    void RemoveFromList(CPlayer* pPlayer);

    class CScriptDebugging* m_pScriptDebugging;

    CMappedList<CPlayer*>                 m_Players;
    std::map<NetServerPlayerID, CPlayer*> m_SocketPlayerMap;
    CMtaVersion                           m_strLowestConnectedPlayerVersion;
    CElapsedTime                          m_ZombieCheckTimer;
};

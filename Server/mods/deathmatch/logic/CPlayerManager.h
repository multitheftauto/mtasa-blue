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
    CPlayerManager(void);
    ~CPlayerManager(void);

    void DoPulse(void);
    void PulseZombieCheck(void);

    void SetScriptDebugging(class CScriptDebugging* pScriptDebugging) { m_pScriptDebugging = pScriptDebugging; };

    CPlayer* Create(const NetServerPlayerID& PlayerSocket);
    void     DeleteAll(void);

    unsigned int Count(void) { return static_cast<unsigned int>(m_Players.size()); }
    unsigned int CountJoined(void);
    bool         Exists(CPlayer* pPlayer);

    CPlayer* Get(const NetServerPlayerID& PlayerSocket);
    CPlayer* Get(const char* szNick, bool bCaseSensitive = false);

    std::list<CPlayer*>::const_iterator IterBegin(void) { return m_Players.begin(); };
    std::list<CPlayer*>::const_iterator IterEnd(void) { return m_Players.end(); };

    void BroadcastOnlyJoined(const CPacket& Packet, CPlayer* pSkip = NULL);
    void BroadcastDimensionOnlyJoined(const CPacket& Packet, ushort usDimension, CPlayer* pSkip = NULL);

    static void Broadcast(const CPacket& Packet, const std::set<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::list<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::vector<CPlayer*>& sendList);
    static void Broadcast(const CPacket& Packet, const std::multimap<ushort, CPlayer*>& groupMap);

    static bool IsValidPlayerModel(unsigned short usPlayerModel);

    void           ResetAll(void);
    void           OnPlayerJoin(CPlayer* pPlayer);
    const SString& GetLowestConnectedPlayerVersion(void) { return m_strLowestConnectedPlayerVersion; }

private:
    void AddToList(CPlayer* pPlayer);
    void RemoveFromList(CPlayer* pPlayer);

    class CScriptDebugging* m_pScriptDebugging;

    CMappedList<CPlayer*>                 m_Players;
    std::map<NetServerPlayerID, CPlayer*> m_SocketPlayerMap;
    SString                               m_strLowestConnectedPlayerVersion;
    CElapsedTime                          m_ZombieCheckTimer;
};

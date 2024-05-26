/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/ASE.h
 *  PURPOSE:     All-Seeing Eye server query protocol handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifdef _WIN32
    #include <conio.h>
    #define sockclose closesocket
#else
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define sockclose close
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET -1
    #endif
using SOCKET = int;
#endif

#include "CConnectHistory.h"
#include <string.h>
#include <stdio.h>
#include <list>
#include <string>

#define MAX_ASE_GAME_TYPE_LENGTH    200
#define MAX_ASE_MAP_NAME_LENGTH     200
#define MAX_RULE_KEY_LENGTH         200
#define MAX_RULE_VALUE_LENGTH       200
#define MAX_ANNOUNCE_VALUE_LENGTH   200

class CASERule;
class CMainConfig;
class CPlayerManager;
class CLanBroadcast;

class ASE
{
public:
    ZERO_ON_NEW
    ASE(CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, std::uint16_t usPort,
        const SString& strServerIPList) noexcept;
    ~ASE() noexcept;

    void DoPulse();
    bool SetPortEnabled(bool bInternetEnabled, bool bLanEnabled);

    static ASE* GetInstance() noexcept { return _instance; }

    std::uint32_t GetMasterServerQueryCount() noexcept { return m_ulMasterServerQueryCount; }
    std::uint32_t GetTotalQueryCount() noexcept { return m_uiNumQueriesTotal; }
    std::uint32_t GetQueriesPerMinute() noexcept { return m_uiNumQueriesPerMinute; }

    CLanBroadcast* InitLan();

    const char* GetGameType() const noexcept { return m_strGameType.c_str(); }
    void        SetGameType(const char* szGameType) noexcept;
    const char* GetMapName() const noexcept { return m_strMapName.c_str(); }
    void        SetMapName(const char* szMapName) noexcept;

    CMainConfig*    GetMainConfig() const noexcept { return m_pMainConfig; };
    CPlayerManager* GetPlayerManager() const noexcept { return m_pPlayerManager; };

    const char* GetRuleValue(const char* szKey) const noexcept;
    void        SetRuleValue(const char* szKey, const char* szValue) noexcept;
    bool        RemoveRuleValue(const char* szKey) noexcept;
    void        ClearRules() noexcept;

    std::list<CASERule*>::iterator begin() noexcept { return m_Rules.begin(); }
    std::list<CASERule*>::iterator end() noexcept { return m_Rules.end(); }

    std::list<CASERule*>::const_iterator begin() const noexcept { return m_Rules.cbegin(); }
    std::list<CASERule*>::const_iterator end() const noexcept { return m_Rules.cend(); }

    std::string QueryLight();

private:
    const std::string* QueryFullCached();
    std::string        QueryFull();
    const std::string* QueryLightCached();
    const std::string* QueryXfireLightCached();
    std::string        QueryXfireLight();

    std::int64_t  m_llCurrentTime;
    std::uint32_t m_uiCurrentPlayerCount;

    CMainConfig*    m_pMainConfig;
    CPlayerManager* m_pPlayerManager;

    std::string m_strGameType;
    SString     m_strMapName;
    SString     m_strIPList;
    std::string m_strPort;

    static ASE* _instance;
    time_t      m_tStartTime;

    std::list<CASERule*> m_Rules;

    std::vector<SOCKET> m_SocketList;

    std::uint16_t m_usPortBase;
    std::uint16_t m_usPort;

    // Full query cache
    std::uint32_t m_uiFullLastPlayerCount;
    std::int64_t  m_llFullLastTime;
    std::int32_t  m_lFullMinInterval;
    std::string   m_strFullCached;

    // Light query cache
    std::uint32_t m_uiLightLastPlayerCount;
    std::int64_t  m_llLightLastTime;
    std::int32_t  m_lLightMinInterval;
    std::string   m_strLightCached;

    // XFire Light query cache
    std::uint32_t m_uiXfireLightLastPlayerCount;
    std::int64_t  m_llXfireLightLastTime;
    std::int32_t  m_lXfireLightMinInterval;
    std::string   m_strXfireLightCached;

    std::string m_strMtaAseVersion;

    // Stats
    std::uint32_t m_ulMasterServerQueryCount;
    std::uint32_t m_uiNumQueriesTotal;
    std::uint32_t m_uiNumQueriesPerMinute;
    std::uint32_t m_uiTotalAtMinuteStart;
    CElapsedTime  m_MinuteTimer;

    CConnectHistory m_QueryDosProtect;
};

class CASERule
{
public:
    CASERule(const char* szKey, const char* szValue)
        noexcept : m_strKey(szKey), m_strValue(szValue) {}

    const char* GetKey() const noexcept { return m_strKey.c_str(); }
    void        SetKey(const char* szKey) noexcept { m_strKey = szKey; }
    const char* GetValue() const noexcept { return m_strValue.c_str(); }
    void        SetValue(const char* szValue) noexcept { m_strValue = szValue; }

private:
    std::string m_strKey;
    std::string m_strValue;
};

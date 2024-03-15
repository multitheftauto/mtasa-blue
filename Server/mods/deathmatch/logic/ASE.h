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

#ifdef WIN32
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
typedef int SOCKET;
#endif

#include "CConnectHistory.h"
#include <string.h>
#include <stdio.h>
#include <list>

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
    ASE(CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, std::uint16_t usPort, const SString& strServerIPList);
    ~ASE();

    void DoPulse();
    bool SetPortEnabled(bool bInternetEnabled, bool bLanEnabled);

    static ASE* GetInstance() { return _instance; }

    std::uint32_t GetMasterServerQueryCount() { return m_ulMasterServerQueryCount; }
    uint          GetTotalQueryCount() { return m_uiNumQueriesTotal; }
    uint          GetQueriesPerMinute() { return m_uiNumQueriesPerMinute; }

    CLanBroadcast* InitLan();

    const char* GetGameType() { return m_strGameType.c_str(); }
    void        SetGameType(const char* szGameType);
    const char* GetMapName() { return m_strMapName.c_str(); }
    void        SetMapName(const char* szMapName);

    CMainConfig*    GetMainConfig() { return m_pMainConfig; };
    CPlayerManager* GetPlayerManager() { return m_pPlayerManager; };

    const char* GetRuleValue(const char* szKey);
    void        SetRuleValue(const char* szKey, const char* szValue);
    bool        RemoveRuleValue(const char* szKey);
    void        ClearRules();

    std::list<CASERule*>::iterator IterBegin() { return m_Rules.begin(); }
    std::list<CASERule*>::iterator IterEnd() { return m_Rules.end(); }

    std::string QueryLight();

private:
    const std::string* QueryFullCached();
    std::string        QueryFull();
    const std::string* QueryLightCached();
    const std::string* QueryXfireLightCached();
    std::string        QueryXfireLight();

    std::int64_t m_llCurrentTime;
    uint      m_uiCurrentPlayerCount;

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
    std::int64_t    m_llFullLastTime;
    long         m_lFullMinInterval;
    std::string  m_strFullCached;

    // Light query cache
    std::uint32_t m_uiLightLastPlayerCount;
    std::int64_t    m_llLightLastTime;
    long         m_lLightMinInterval;
    std::string  m_strLightCached;

    // XFire Light query cache
    std::uint32_t m_uiXfireLightLastPlayerCount;
    std::int64_t    m_llXfireLightLastTime;
    long         m_lXfireLightMinInterval;
    std::string  m_strXfireLightCached;

    std::string m_strMtaAseVersion;

    // Stats
    std::uint32_t m_ulMasterServerQueryCount;
    uint          m_uiNumQueriesTotal;
    uint          m_uiNumQueriesPerMinute;
    uint          m_uiTotalAtMinuteStart;
    CElapsedTime  m_MinuteTimer;

    CConnectHistory m_QueryDosProtect;
};

class CASERule
{
public:
    CASERule(const char* szKey, const char* szValue)
    {
        m_strKey = szKey;
        m_strValue = szValue;
    }
    const char* GetKey() { return m_strKey.c_str(); }
    void        SetKey(const char* szKey) { m_strKey = szKey; }
    const char* GetValue() { return m_strValue.c_str(); }
    void        SetValue(const char* szValue) { m_strValue = szValue; }

private:
    std::string m_strKey;
    std::string m_strValue;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRemoteDebugger.h
 *  PURPOSE:     All-Seeing Eye server query protocol handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CRemoteDebugger;

#pragma once

#ifdef WIN32
    #include <conio.h>
    #define sockclose closesocket
#else
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define sockclose close
typedef int SOCKET;
#endif

#include <string.h>
#include <stdio.h>

#include "CMainConfig.h"
#include "CPlayerManager.h"
#include "CPlayer.h"

#include <list>

#define MAX_CRemoteDebugger_GAME_TYPE_LENGTH    200
#define MAX_CRemoteDebugger_MAP_NAME_LENGTH     200
#define MAX_RULE_KEY_LENGTH         200
#define MAX_RULE_VALUE_LENGTH       200
#define MAX_ANNOUNCE_VALUE_LENGTH   200

class CCRemoteDebuggerRule;

class CRemoteDebugger
{
public:
    ZERO_ON_NEW
    CRemoteDebugger(CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const SString& strServerIPList);
    ~CRemoteDebugger();

    void DoPulse();
    bool SetPortEnabled(bool bInternetEnabled, bool bLanEnabled);

    static CRemoteDebugger* GetInstance() { return _instance; }

    CLanBroadcast* InitLan();

    CMainConfig*    GetMainConfig() { return m_pMainConfig; };
    CPlayerManager* GetPlayerManager() { return m_pPlayerManager; };

private:

    CMainConfig*    m_pMainConfig;
    CPlayerManager* m_pPlayerManager;

    static CRemoteDebugger* _instance;
    time_t      m_tStartTime;

    list<CCRemoteDebuggerRule*> m_Rules;

    std::vector<SOCKET> m_SocketList;
    std::vector<SOCKET> m_SocketsRemoteDebuggers;
    SString             m_strIPList;
    std::string         m_strConnectJsonVersion;
    std::string         m_strConnectJson;
    std::string         m_strTestJson;
    unsigned short m_usPort;

};

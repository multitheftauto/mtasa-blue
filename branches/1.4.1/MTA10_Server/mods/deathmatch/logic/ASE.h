/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/ASE.h
*  PURPOSE:     All-Seeing Eye server query protocol handler class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class ASE;

#ifndef __ASE_H__
#define __ASE_H__

#ifdef WIN32
    #include <conio.h>
    #include <winsock.h>
    #define sockclose closesocket
#else
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define sockclose close
#endif

#include <string.h>
#include <stdio.h>

#include "CMainConfig.h"
#include "CPlayerManager.h"
#include "CPlayer.h"

#include <list>

#define MAX_ASE_GAME_TYPE_LENGTH    200
#define MAX_ASE_MAP_NAME_LENGTH     200
#define MAX_RULE_KEY_LENGTH         200
#define MAX_RULE_VALUE_LENGTH       200
#define MAX_ANNOUNCE_VALUE_LENGTH   200

class CASERule;

class ASE
{
public:
    ZERO_ON_NEW
                            ASE                      ( CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const char* szServerIP = NULL );
                            ~ASE                     ( void );

    void                    DoPulse                  ( void );
    bool                    SetPortEnabled           ( bool bInternetEnabled, bool bLanEnabled );

    static ASE*             GetInstance              ( void )                { return _instance; }

    unsigned long           GetMasterServerQueryCount ( void )          { return m_ulMasterServerQueryCount; }
    uint                    GetTotalQueryCount      ( void )            { return m_uiNumQueriesTotal; }
    uint                    GetQueriesPerMinute     ( void )            { return m_uiNumQueriesPerMinute; }

    CLanBroadcast*          InitLan             ( void );

    const char*             GetGameType         ( void )                { return m_strGameType.c_str(); }
    void                    SetGameType         ( const char * szGameType );
    const char*             GetMapName          ( void )                { return m_strMapName.c_str(); }
    void                    SetMapName          ( const char * szMapName );

    CMainConfig*            GetMainConfig       ( void )                { return m_pMainConfig; };
    CPlayerManager*         GetPlayerManager    ( void )                { return m_pPlayerManager; };

    const char*             GetRuleValue        ( const char* szKey );
    void                    SetRuleValue        ( const char* szKey, const char* szValue );
    bool                    RemoveRuleValue     ( const char* szKey );
    void                    ClearRules          ( void );

    list < CASERule* > ::iterator IterBegin     ( void )                { return m_Rules.begin (); }
    list < CASERule* > ::iterator IterEnd       ( void )                { return m_Rules.end (); }

    std::string             QueryLight               ( void );
private:
    const std::string*      QueryFullCached          ( void );
    std::string             QueryFull                ( void );
    const std::string*      QueryLightCached         ( void );
    const std::string*      QueryXfireLightCached    ( void );
    std::string             QueryXfireLight          ( void );

    long long               m_llCurrentTime;
    uint                    m_uiCurrentPlayerCount;

    CMainConfig*            m_pMainConfig;
    CPlayerManager*         m_pPlayerManager;

    std::string             m_strGameType;
    std::string             m_strMapName;
    std::string             m_strIP;
    std::string             m_strPort;

    static ASE*             _instance;
    time_t                  m_tStartTime;

    list < CASERule* >      m_Rules;

    unsigned int            m_Socket;
    sockaddr_in             m_SockAddr;

    unsigned short          m_usPortBase;
    unsigned short          m_usPort;

    // Full query cache
    unsigned int            m_uiFullLastPlayerCount;
    long long               m_llFullLastTime;
    long                    m_lFullMinInterval;
    std::string             m_strFullCached;

    // Light query cache
    unsigned int            m_uiLightLastPlayerCount;
    long long               m_llLightLastTime;
    long                    m_lLightMinInterval;
    std::string             m_strLightCached;

    // XFire Light query cache
    unsigned int            m_uiXfireLightLastPlayerCount;
    long long               m_llXfireLightLastTime;
    long                    m_lXfireLightMinInterval;
    std::string             m_strXfireLightCached;

    std::string             m_strMtaAseVersion;

    // Stats
    unsigned long           m_ulMasterServerQueryCount;
    uint                    m_uiNumQueriesTotal;
    uint                    m_uiNumQueriesPerMinute;
    uint                    m_uiTotalAtMinuteStart;
    CElapsedTime            m_MinuteTimer;

    CConnectHistory         m_QueryDosProtect;
};

class CASERule
{
public:
    inline              CASERule        ( const char* szKey, const char* szValue )
    {
        m_strKey = szKey;
        m_strValue = szValue;
    }
    inline const char*  GetKey          ( void )            { return m_strKey.c_str (); }
    inline void         SetKey          ( const char* szKey )
    {
        m_strKey = szKey;
    }
    inline const char*  GetValue          ( void )          { return m_strValue.c_str (); }
    inline void         SetValue          ( const char* szValue )
    {
        m_strValue = szValue;
    }
private:
    std::string         m_strKey;
    std::string         m_strValue;
};

#endif

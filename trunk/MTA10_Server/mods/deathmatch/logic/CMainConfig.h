/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMainConfig.h
*  PURPOSE:     XML-based main configuration file parser class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*               Oliver Brown <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               Ed Lyons <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CMainConfig;

#ifndef __CMAINCONFIG_H
#define __CMAINCONFIG_H

#include "CCommon.h"
#include "CLogger.h"
#include "CConsole.h"
#include "CXMLConfig.h"
#include <list>

#define MAX_MAP_NAME_LENGTH 64

class CMainConfig: public CXMLConfig
{
public:
                                    CMainConfig                     ( CConsole* pConsole, class CLuaManager* pLuaMain );

    bool                            Load                            ( const char* szFileName = NULL );
    bool                            LoadExtended                    ( void );
    bool                            Save                            ( const char* szFileName = NULL );

    inline const std::string&       GetServerName                   ( void )        { return m_strServerName; };
    void                            SetServerName                   ( std::string strServerName );
    std::string                     GetServerIP                     ( void );
    unsigned short                  GetServerPort                   ( void );
    unsigned int                    GetMaxPlayers                   ( void );
    inline bool                     IsHTTPEnabled                   ( void )        { return m_bHTTPEnabled; };

    bool                            IsValidPassword                 ( const char* szPassword, unsigned int& cUnsupportedIndex );
    inline bool                     HasPassword                     ( void )        { return !m_strPassword.empty (); };
    inline const std::string&       GetPassword                     ( void )        { return m_strPassword; };
    void                            SetPassword                     ( const char* szPassword );

    inline bool                     GetASEEnabled                   ( void )        { return m_bAseEnabled; };
    unsigned short                  GetHTTPPort                     ( void );
    inline eHTTPDownloadType        GetHTTPDownloadType             ( void )        { return m_ucHTTPDownloadType; };
    inline const std::string&       GetHTTPDownloadURL              ( void )        { return m_strHTTPDownloadURL; };
    inline int                      GetHTTPMaxConnectionsPerClient  ( void )        { return m_iHTTPMaxConnectionsPerClient; };
    inline int                      GetHTTPThreadCount              ( void )        { return m_iHTTPThreadCount; };
    inline int                      GetHTTPDosThreshold             ( void )        { return m_iHTTPDosThreshold; };
    inline int                      GetEnableClientChecks           ( void )        { return m_iEnableClientChecks; };
    inline const std::string&       GetLogFile                      ( void )        { return m_strLogFile; };
    inline const std::string&       GetAuthFile                     ( void )        { return m_strAuthFile; };
    inline bool                     GetAutoUpdateAntiCheatEnabled   ( void )        { return m_bAutoUpdateAntiCheatEnabled; };
    inline bool                     GetJoinFloodProtectionEnabled   ( void )        { return m_bJoinFloodProtectionEnabled; };
    inline bool                     GetScriptDebugLogEnabled        ( void )        { return m_bScriptDebugLogEnabled && !m_strScriptDebugLogFile.empty (); };
    inline const std::string&       GetScriptDebugLogFile           ( void )        { return m_strScriptDebugLogFile; };
    inline unsigned int             GetScriptDebugLogLevel          ( void )        { return m_uiScriptDebugLogLevel; };
    inline const std::string&       GetAccessControlListFile        ( void )        { return m_strAccessControlListFile; };
    inline bool                     GetAutoUpdateIncludedResourcesEnabled   ( void )        { return m_bAutoUpdateIncludedResources; };
    inline bool                     GetDontBroadcastLan             ( void )        { return m_bDontBroadcastLan; };
    inline bool                     GetSerialVerificationEnabled    ( void )        { return m_bVerifySerials; };
    bool                            IsDisableAC                     ( const char* szTagAC )     { return MapContains ( m_DisableACMap, szTagAC ); };
    bool                            IsEnableDiagnostic              ( const char* szTag )       { return MapContains ( m_EnableDiagnosticMap, szTag ); };
    bool                            IsBelowMinimumClient            ( const char* szVersion )   { return m_strMinClientVersion.length () && m_strMinClientVersion > szVersion; }
    bool                            IsBelowRecommendedClient        ( const char* szVersion )   { return m_strRecommendedClientVersion.length () && m_strRecommendedClientVersion > szVersion; }
    const SString&                  GetMinimumClientVersion         ( void )                    { return m_strMinClientVersion; }
    const SString&                  GetRecommendedClientVersion     ( void )                    { return m_strRecommendedClientVersion; }

    inline unsigned short           GetFPSLimit                     ( void )        { return m_usFPSLimit; };
    void                            SetFPSLimit                     ( unsigned short usFPS );

    void                            SetCommandLineParser            ( CCommandLineParser* pCommandLineParser );

private:
    void                            RegisterCommand                 ( const char* szName, FCommandHandler* pFunction, bool bRestricted );

    CConsole*                       m_pConsole;
    CLuaManager*                    m_pLuaManager;
    CXMLNode*                       m_pRootNode;
    CCommandLineParser*             m_pCommandLineParser;

    std::string                     m_strServerIP;
    std::string                     m_strServerName;
    unsigned short                  m_usServerPort;
    unsigned int                    m_uiMaxPlayers;
    bool                            m_bHTTPEnabled;
    std::string                     m_strPassword;
    bool                            m_bAseEnabled;
    unsigned short                  m_usHTTPPort;
    eHTTPDownloadType               m_ucHTTPDownloadType;
    std::string                     m_strHTTPDownloadURL;
    int                             m_iHTTPMaxConnectionsPerClient;
    int                             m_iHTTPThreadCount;
    int                             m_iHTTPDosThreshold;
    int                             m_iEnableClientChecks;
    std::string                     m_strLogFile;
    std::string                     m_strAuthFile;
    bool                            m_bAutoUpdateAntiCheatEnabled;
    bool                            m_bJoinFloodProtectionEnabled;
    bool                            m_bScriptDebugLogEnabled;
    std::string                     m_strScriptDebugLogFile;
    unsigned int                    m_uiScriptDebugLogLevel;
    std::string                     m_strAccessControlListFile;
    bool                            m_bAutoUpdateIncludedResources;
    bool                            m_bVerifySerials;
    unsigned short                  m_usFPSLimit;
    bool                            m_bDontBroadcastLan;
    std::map < SString, int >       m_DisableACMap;
    std::map < SString, int >       m_EnableDiagnosticMap;
    SString                         m_strMinClientVersion;
    SString                         m_strRecommendedClientVersion;
};

#endif

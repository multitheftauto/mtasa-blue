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

    bool                            Load                            ( void );
    bool                            LoadExtended                    ( void );
    bool                            Save                            ( void );

    inline const std::string&       GetServerName                   ( void )        { return m_strServerName; };
    std::string                     GetServerIP                     ( void );
    unsigned short                  GetServerPort                   ( void );
    unsigned int                    GetMaxPlayers                   ( void );
    unsigned int                    GetHardMaxPlayers               ( void );
    void                            SetSoftMaxPlayers               ( unsigned int v ) { m_uiSoftMaxPlayers = v; }
    inline bool                     IsHTTPEnabled                   ( void )        { return m_bHTTPEnabled; };

    bool                            IsValidPassword                 ( const char* szPassword );
    inline bool                     HasPassword                     ( void )        { return !m_strPassword.empty (); };
    inline const std::string&       GetPassword                     ( void )        { return m_strPassword; };
    bool                            SetPassword                     ( const char* szPassword, bool bSave );

    bool                            IsVoiceEnabled                  ( void );
    unsigned int                    GetVoiceSampleRate              ( void )        { return m_uiVoiceSampleRate; };
    unsigned int                    GetVoiceQuality                 ( void )        { return m_ucVoiceQuality; };
    unsigned int                    GetVoiceBitrate                 ( void )        { return m_uiVoiceBitrate; };

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
    bool                            IsDisableAC                     ( const char* szTagAC )     { return MapContains ( m_DisableComboACMap, szTagAC ); };
    bool                            IsEnableDiagnostic              ( const char* szTag )       { return MapContains ( m_EnableDiagnosticMap, szTag ); };
    bool                            IsBelowMinimumClient            ( const char* szVersion )   { return GetMinimumClientVersion () > szVersion; }
    bool                            IsBelowRecommendedClient        ( const char* szVersion )   { return m_strRecommendedClientVersion.length () && m_strRecommendedClientVersion > szVersion; }
    void                            SetMinimumClientVersionOverride ( const SString& strOverride ) { m_strMinClientVersionOverride = strOverride; }
    const SString&                  GetMinimumClientVersion         ( void )                    { return m_strMinClientVersionOverride > m_strMinClientVersion ? m_strMinClientVersionOverride : m_strMinClientVersion; }
    const SString&                  GetRecommendedClientVersion     ( void )                    { return m_strRecommendedClientVersion; }
    inline bool                     IsAutoLoginEnabled              ( )                         { return m_bAutoLogin; }
    const SString&                  GetIdFile                       ( void )                    { return m_strIdFile; }
    bool                            GetNetworkEncryptionEnabled     ( void )                    { return m_bNetworkEncryptionEnabled; }
    const SString&                  GetGlobalDatabasesPath          ( void )                    { return m_strGlobalDatabasesPath; }
    const SString&                  GetSystemDatabasesPath          ( void )                    { return m_strSystemDatabasesPath; }
    const SString&                  GetBackupPath                   ( void )                    { return m_strBackupPath; }
    int                             GetBackupInterval               ( void )                    { return m_iBackupInterval; }
    int                             GetBackupAmount                 ( void )                    { return m_iBackupAmount; }

    inline unsigned short           GetFPSLimit                     ( void )        { return m_usFPSLimit; };
    bool                            SetFPSLimit                     ( unsigned short usFPS, bool bSave );
    int                             GetPendingWorkToDoSleepTime     ( void )        { return m_iPendingWorkToDoSleepTime; };

    SString                         GetSetting                      ( const SString& configSetting );
    bool                            GetSetting                      ( const SString& configSetting, SString& strValue );
    bool                            SetSetting                      ( const SString& configSetting, const SString& strValue, bool bSave );

    void                            SetCommandLineParser            ( CCommandLineParser* pCommandLineParser );
    void                            ApplyBandwidthReductionMode     ( void );

private:
    void                            RegisterCommand                 ( const char* szName, FCommandHandler* pFunction, bool bRestricted );

    CConsole*                       m_pConsole;
    CLuaManager*                    m_pLuaManager;
    CXMLNode*                       m_pRootNode;
    CCommandLineParser*             m_pCommandLineParser;

    unsigned int                    m_uiVoiceSampleRate;
    unsigned char                   m_ucVoiceQuality;
    unsigned int                    m_uiVoiceBitrate;

    bool                            m_bVoiceEnabled;
    std::string                     m_strServerIP;
    std::string                     m_strServerName;
    unsigned short                  m_usServerPort;
    unsigned int                    m_uiMaxPlayers;
    unsigned int                    m_uiSoftMaxPlayers;
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
    std::set < SString >            m_DisableComboACMap;
    std::set < SString >            m_EnableDiagnosticMap;
    SString                         m_strMinClientVersion;
    SString                         m_strRecommendedClientVersion;
    SString                         m_strMinClientVersionOverride;
    bool                            m_bAutoLogin;
    SString                         m_strIdFile;
    SString                         m_strGlobalDatabasesPath;
    SString                         m_strSystemDatabasesPath;
    SString                         m_strBackupPath;
    int                             m_iBackupInterval;
    int                             m_iBackupAmount;
    bool                            m_bNetworkEncryptionEnabled;
    SString                         m_strBandwidthReductionMode;
    int                             m_iPendingWorkToDoSleepTime;
    std::map < SString, SString >   m_TransientSettings;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCore.h
*  PURPOSE:     Header file for base core class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCore;

#ifndef __CCORE_H
#define __CCORE_H

#include "../version.h"

#include "CClientTime.h"
#include "CClientVariables.h"
#include "CCommands.h"
#include "CFileSystemHook.h"
#include "CModuleLoader.h"
#include "CSingleton.h"
#include "CGUI.h"
#include "CConnectManager.h"
#include "CDirect3DHookManager.h"
#include "CDirectInputHookManager.h"
#include "CGraphics.h"
#include "CSetCursorPosHook.h"
#include "CMessageLoopHook.h"
#include "CLogger.h"
#include "CConsoleLogger.h"
#include "CModManager.h"
#include <core/CClientBase.h>
#include <core/CCoreInterface.h>
#include "CDirect3DData.h"
#include "tracking/CTCPManager.h"
#include "CClientVariables.h"
#include "CKeyBinds.h"
#include "CScreenShot.h"
#include "CCommunity.h"
#include <xml/CXML.h>
#include <ijsify.h>
#include "CXfireQuery.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define BLUE_VERSION_STRING     "Multi Theft Auto v" MTA_DM_BUILDTAG_LONG "\n" \
                                "Copyright (C) 2003 - 2012 Multi Theft Auto" \

// Configuration file path (relative to Grand Theft Auto directory)
#define MTA_CONFIG_PATH             "mta/coreconfig.xml"
#define MTA_SERVER_CACHE_PATH       "mta/servercache.xml"
#define CONFIG_ROOT                 "mainconfig"
#define CONFIG_NODE_CVARS           "settings"                  // cvars node
#define CONFIG_NODE_KEYBINDS        "binds"                     // keybinds node
#define CONFIG_NODE_JOYPAD          "joypad"
#define CONFIG_NODE_UPDATER         "updater"
#define CONFIG_NODE_SERVER_INT      "internet_servers"          // backup of last successful master server list query
#define CONFIG_NODE_SERVER_FAV      "favourite_servers"         // favourite servers list node
#define CONFIG_NODE_SERVER_REC      "recently_played_servers"   // recently played servers list node
#define CONFIG_NODE_SERVER_OPTIONS  "serverbrowser_options"     // saved options for the server browser
#define CONFIG_NODE_SERVER_SAVED    "server_passwords"    // This contains saved passwords (as appose to save_server_passwords which is a setting)
#define CONFIG_NODE_SERVER_HISTORY  "connect_history"
#define CONFIG_INTERNET_LIST_TAG    "internet_server"
#define CONFIG_FAVOURITE_LIST_TAG   "favourite_server"
#define CONFIG_RECENT_LIST_TAG      "recently_played_server"
#define CONFIG_HISTORY_LIST_TAG     "connected_server"

class CCore : public CCoreInterface, public CSingleton < CCore >
{
public:
                            CCore                           ( void );
                            ~CCore                          ( void );

    // Subsystems (query)
    eCoreVersion            GetVersion                      ( void );
    CConsoleInterface*      GetConsole                      ( void );
    CCommandsInterface*     GetCommands                     ( void );
    inline CConnectManager* GetConnectManager               ( void )                { return m_pConnectManager; };
    CGame*                  GetGame                         ( void );
    CGUI*                   GetGUI                          ( void );
    CGraphicsInterface*     GetGraphics                     ( void );
    CModManagerInterface*   GetModManager                   ( void );
    CMultiplayer*           GetMultiplayer                  ( void );
    CNet*                   GetNetwork                      ( void );
    CXML*                   GetXML                          ( void )                { return m_pXML; };
    CXMLNode*               GetConfig                       ( void );
    CClientVariables*       GetCVars                        ( void )                { return &m_ClientVariables; };
    CKeyBindsInterface*     GetKeyBinds                     ( void );
    CLocalGUI*              GetLocalGUI                     ( void );
    CCommunityInterface*    GetCommunity                    ( void )                { return &m_Community; };

    void                    SaveConfig                      ( void );

    // Debug
    void                    DebugEcho                       ( const char* szText );
    void                    DebugPrintf                     ( const char* szFormat, ... );
    void                    SetDebugVisible                 ( bool bVisible );
    bool                    IsDebugVisible                  ( void );
    void                    DebugEchoColor                  ( const char* szText, unsigned char R, unsigned char G, unsigned char B );
    void                    DebugPrintfColor                ( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    DebugClear                      ( void );

    // Chat
    void                    ChatEcho                        ( const char* szText, bool bColorCoded = false );
    void                    ChatEchoColor                   ( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false );
    void                    ChatPrintf                      ( const char* szFormat, bool bColorCoded, ... );
    void                    ChatPrintfColor                 ( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    SetChatVisible                  ( bool bVisible );
    bool                    IsChatVisible                   ( void );
    void                    EnableChatInput                 ( char* szCommand, DWORD dwColor );
    bool                    IsChatInputEnabled              ( void );

    // Screenshots
    void                    TakeScreenShot                  ( void );

    // GUI
    bool                    IsSettingsVisible               ( void );
    bool                    IsMenuVisible                   ( void );
    bool                    IsCursorForcedVisible           ( void );
    bool                    IsCursorControlsToggled         ( void ) { return m_bCursorToggleControls; }
    void                    HideMainMenu                    ( void );
    void                    HideQuickConnect                ( void );
    void                    SetCenterCursor                 ( bool bEnabled );

    void                    ShowServerInfo                  ( unsigned int WindowType );

    // Configuration
    void                    ApplyConsoleSettings            ( void );
    void                    ApplyGameSettings               ( void );
    void                    ApplyCommunityState             ( void );
    void                    UpdateRecentlyPlayed            ( void );

    // Net
    void                    SetConnected                    ( bool bConnected );
    bool                    IsConnected                     ( void );
    bool                    Reconnect                       ( const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true );

    // Mod
    void                    SetOfflineMod                   ( bool bOffline );
    void                    ForceCursorVisible              ( bool bVisible, bool bToggleControls = true );
    void                    SetMessageProcessor             ( pfnProcessMessage pfnMessageProcessor );
    void                    ShowMessageBox                  ( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler = NULL );
    void                    RemoveMessageBox                ( bool bNextFrame = false );
    bool                    IsOfflineMod                    ( void ) { return m_bIsOfflineMod; }
    const char *            GetModInstallRoot               ( const char * szModName );


    // Subsystems
    void                    CreateGame                      ( void );
    void                    CreateMultiplayer               ( void );
    void                    CreateNetwork                   ( void );
    void                    CreateXML                       ( void );
    void                    InitGUI                         ( IUnknown* pDevice );
    void                    CreateGUI                       ( void );
    void                    DestroyGame                     ( void );
    void                    DestroyMultiplayer              ( void );
    void                    DestroyNetwork                  ( void );
    void                    DestroyXML                      ( void );
    void                    DeinitGUI                       ( void );
    void                    DestroyGUI                      ( void );

    // Hooks
    void                    ApplyHooks                      ( void );
    HWND                    GetHookedWindow                 ( void );
    void                    SwitchRenderWindow              ( HWND hWnd, HWND hWndInput );
    void                    CallSetCursorPos                ( int X, int Y ) { m_pSetCursorPosHook->CallSetCursorPos(X,Y); }
    void                    SetClientMessageProcessor       ( pfnProcessMessage pfnMessageProcessor ) { m_pfnMessageProcessor = pfnMessageProcessor; };
    pfnProcessMessage       GetClientMessageProcessor       ( void ) { return m_pfnMessageProcessor; }
    void                    ChangeResolution                ( long width, long height, long depth );
    void                    ApplyLoadingCrashPatch          ( void );

    bool                    IsFocused                       ( void )                        { return ( GetForegroundWindow ( ) == GetHookedWindow ( ) ); };
    bool                    IsWindowMinimized               ( void );

    // Pulse
    void                    DoPreFramePulse                 ( void );
    void                    DoPostFramePulse                ( void );

    // Events
    bool                    OnMouseClick                    ( CGUIMouseEventArgs Args );
    bool                    OnMouseDoubleClick              ( CGUIMouseEventArgs Args );
    void                    OnModUnload                     ( void );

    // Misc
    void                    RegisterCommands                ( void );
    bool                    IsValidNick                     ( const char* szNick );     // Move somewhere else
    void                    Quit                            ( bool bInstantly = true );
    void                    InitiateUpdate                  ( const char* szType, const char* szData, const char* szHost )      { m_pLocalGUI->InitiateUpdate ( szType, szData, szHost ); }
    bool                    IsOptionalUpdateInfoRequired    ( const char* szHost )                          { return m_pLocalGUI->IsOptionalUpdateInfoRequired ( szHost ); }
    void                    InitiateDataFilesFix            ( void )                                        { m_pLocalGUI->InitiateDataFilesFix (); }

    uint                    GetFrameRateLimit               ( void )                                        { return m_uiFrameRateLimit; }
    void                    RecalculateFrameRateLimit       ( uint uiServerFrameRateLimit = -1 );
    void                    ApplyFrameRateLimit             ( uint uiOverrideRate = -1 );
    void                    EnsureFrameRateLimitApplied     ( void );
    void                    DoReliablePulse                 ( void );

    void                    CalculateStreamingMemoryRange   ( void );
    uint                    GetMinStreamingMemory           ( void );
    uint                    GetMaxStreamingMemory           ( void );

    SString                 GetConnectCommandFromURI        ( const char* szURI );  
    void                    GetConnectParametersFromURI     ( const char* szURI, std::string &strHost, unsigned short &usPort, std::string &strNick, std::string &strPassword );
    bool                    bScreenShot;
    std::map < std::string, std::string > & GetCommandLineOptions ( void ) { return m_CommandLineOptions; }
    const char *            GetCommandLineOption            ( const char* szOption );
    const char *            GetCommandLineArgs              ( void ) { return m_szCommandLineArgs; }
    void                    RequestNewNickOnStart           ( void ) { m_bWaitToSetNick = true; };
    bool                    WillRequestNewNickOnStart       ( void ) { return m_bWaitToSetNick; };
    bool                    WasLaunchedWithConnectURI       ( void );

    //XFire
    SString                 UpdateXfire                     ( void );
    void                    SetCurrentServer                ( in_addr Addr, unsigned short usQueryPort );
    void                    SetXfireData                    ( std::string strServerName, std::string strVersion, bool bPassworded, std::string strGamemode, std::string strMap, std::string strPlayerName, std::string strPlayerCount );

    void                    OnPreHUDRender                  ( void );
    void                    OnDeviceRestore                 ( void );
    void                    OnCrashAverted                  ( uint uiId );
    void                    LogEvent                        ( uint uiDebugId, const char* szType, const char* szContext, const char* szBody );
    bool                    GetDebugIdEnabled               ( uint uiDebugId );
    EDiagnosticDebugType    GetDiagnosticDebug              ( void );
    void                    SetDiagnosticDebug              ( EDiagnosticDebugType value );

private:
    // Core devices.
    CXML*                       m_pXML;
    CLocalGUI *                 m_pLocalGUI;
    CGraphics *                 m_pGraphics;
    CCommands *                 m_pCommands;
    CDirect3DData *             m_pDirect3DData;
    CConnectManager*            m_pConnectManager;

    // Instances (put new classes here!)
    CXMLFile*                   m_pConfigFile;
    CClientVariables            m_ClientVariables;
    CCommunity                  m_Community;

    // Hook interfaces.
    CMessageLoopHook *          m_pMessageLoopHook;
    CDirectInputHookManager *   m_pDirectInputHookManager;
    CDirect3DHookManager *      m_pDirect3DHookManager;
    //CFileSystemHook *           m_pFileSystemHook;
    CSetCursorPosHook *         m_pSetCursorPosHook;
    CTCPManager *               m_pTCPManager;

    bool                        m_bLastFocused;
    int                         m_iUnminimizeFrameCounter;
    bool                        m_bDidRecreateRenderTargets;

    // Module loader objects.
    CModuleLoader               m_GameModule;
    CModuleLoader               m_MultiplayerModule;
    CModuleLoader               m_NetModule;
    CModuleLoader               m_XMLModule;
    CModuleLoader               m_GUIModule;

    // Mod manager
    CModManager*                m_pModManager; 

    // Module interfaces.
    CGame *                     m_pGame;
    CNet *                      m_pNet;
    CMultiplayer *              m_pMultiplayer;
    CGUI*                       m_pGUI;

    // Logger utility interface.
    CLogger *                   m_pLogger;
    CConsoleLogger *            m_pConsoleLogger;

    CKeyBinds*                  m_pKeyBinds;

    bool                        m_bFirstFrame;
    bool                        m_bIsOfflineMod;
    bool                        m_bCursorToggleControls;
    pfnProcessMessage           m_pfnMessageProcessor;

    CGUIMessageBox*             m_pMessageBox;

    // screen res
    DEVMODE                     m_Current;

    //Xfire Update
    CXfireServerInfo*           m_pCurrentServer;
    time_t                      m_tXfireUpdate;

    SString                     m_strModInstallRoot;

    bool                        m_bQuitOnPulse;
    bool                        m_bDestroyMessageBox;

    bool                        m_bDoneFrameRateLimit;
    uint                        m_uiServerFrameRateLimit;
    uint                        m_uiFrameRateLimit;
    double                      m_dLastTimeMs;
    double                      m_dPrevOverrun;
    bool                        m_bWaitToSetNick;
    uint                        m_uiNewNickWaitFrames;
    EDiagnosticDebugType        m_DiagnosticDebug;
    float                       m_fMinStreamingMemory;
    float                       m_fMaxStreamingMemory;

    // Command line
    static void                 ParseCommandLine                ( std::map < std::string, std::string > & options, const char*& szArgs, const char** pszNoValOptions = NULL );
    std::map < std::string, std::string > m_CommandLineOptions;     // e.g. "-o option" -> {"o" = "option"}
    const char*                 m_szCommandLineArgs;                // Everything that comes after the options
};

#endif

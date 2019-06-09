/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCore.h
 *  PURPOSE:     Header file for base core class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CCore;

#pragma once

#include "../version.h"

#include "CClientVariables.h"
#include "CCommands.h"
#include "CModuleLoader.h"
#include "CSingleton.h"
#include "CGUI.h"
#include "CConnectManager.h"
#include <DXHook/CDirect3DHookManager.h>
#include <DXHook/CDirectInputHookManager.h>
#include <Graphics/CGraphics.h>
#include "CSetCursorPosHook.h"
#include "CMessageLoopHook.h"
#include "CConsoleLogger.h"
#include "CModManager.h"
#include <core/CClientBase.h>
#include <core/CClientEntityBase.h>
#include <core/CCoreInterface.h>
#include <DXHook/CDirect3DData.h>
#include "CClientVariables.h"
#include "CKeyBinds.h"
#include "CMouseControl.h"
#include "CScreenShot.h"
#include <xml/CXML.h>
#include <ijsify.h>
#include <core/CWebCoreInterface.h>
#include "CTrayIcon.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define BLUE_VERSION_STRING     "Multi Theft Auto v" MTA_DM_BUILDTAG_LONG
#define BLUE_COPYRIGHT_STRING   "Copyright (C) 2003 - %BUILD_YEAR% Multi Theft Auto"

// Configuration file path (relative to MTA install directory)
#define MTA_CONFIG_PATH             "mta/config/coreconfig.xml"
#define MTA_SERVER_CACHE_PATH       "mta/config/servercache.xml"
#define MTA_CONSOLE_LOG_PATH        "mta/logs/console.log"
#define MTA_CONSOLE_INPUT_LOG_PATH  "mta/logs/console-input.log"
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
#define IDT_TIMER1 1234

extern class CCore*         g_pCore;
extern class CGraphics*     g_pGraphics;
extern class CLocalization* g_pLocalization;
bool                        UsingAltD3DSetup();
extern SString              g_strJingleBells;

class CCore : public CCoreInterface, public CSingleton<CCore>
{
public:
    CCore();
    ~CCore();

    // Subsystems (query)
    eCoreVersion            GetVersion();
    CConsoleInterface*      GetConsole();
    CCommandsInterface*     GetCommands();
    CConnectManager*        GetConnectManager() { return m_pConnectManager; };
    CGame*                  GetGame();
    CGUI*                   GetGUI();
    CGraphicsInterface*     GetGraphics();
    CModManagerInterface*   GetModManager();
    CMultiplayer*           GetMultiplayer();
    CNet*                   GetNetwork();
    CXML*                   GetXML() { return m_pXML; };
    CXMLNode*               GetConfig();
    CClientVariables*       GetCVars() { return &m_ClientVariables; };
    CKeyBindsInterface*     GetKeyBinds();
    CMouseControl*          GetMouseControl() { return m_pMouseControl; };
    CLocalGUI*              GetLocalGUI();
    CLocalizationInterface* GetLocalization() { return g_pLocalization; };
    CWebCoreInterface*      GetWebCore();
    CTrayIconInterface*     GetTrayIcon() { return m_pTrayIcon; };

    void SaveConfig(bool bWaitUntilFinished = false);

    // Debug
    void DebugEcho(const char* szText);
    void DebugPrintf(const char* szFormat, ...);
    void SetDebugVisible(bool bVisible);
    bool IsDebugVisible();
    void DebugEchoColor(const char* szText, unsigned char R, unsigned char G, unsigned char B);
    void DebugPrintfColor(const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ...);
    void DebugClear();

    // Chat
    void ChatEcho(const char* szText, bool bColorCoded = false);
    void ChatEchoColor(const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false);
    void ChatPrintf(const char* szFormat, bool bColorCoded, ...);
    void ChatPrintfColor(const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ...);
    void SetChatVisible(bool bVisible);
    bool IsChatVisible();
    void EnableChatInput(char* szCommand, DWORD dwColor);
    bool IsChatInputEnabled();
    bool ClearChat();
    void OnGameTimerUpdate();

    // Screenshots
    void TakeScreenShot();

    // GUI
    bool IsSettingsVisible();
    bool IsMenuVisible();
    bool IsCursorForcedVisible();
    bool IsCursorControlsToggled() { return m_bCursorToggleControls; }
    void HideMainMenu();
    void SetCenterCursor(bool bEnabled);

    void ShowServerInfo(unsigned int WindowType);

    // Configuration
    void ApplyConsoleSettings();
    void ApplyGameSettings();
    void UpdateRecentlyPlayed();

    // Net
    void SetConnected(bool bConnected);
    bool IsConnected();
    bool Reconnect(const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true);

    // Mod
    void        SetOfflineMod(bool bOffline);
    void        ForceCursorVisible(bool bVisible, bool bToggleControls = true);
    void        SetMessageProcessor(pfnProcessMessage pfnMessageProcessor);
    void        ShowMessageBox(const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK* ResponseHandler = NULL);
    void        RemoveMessageBox(bool bNextFrame = false);
    void        ShowErrorMessageBox(const SString& strTitle, SString strMessage, const SString& strTroubleLink = "");
    void        ShowNetErrorMessageBox(const SString& strTitle, SString strMessage, SString strTroubleLink = "", bool bLinkRequiresErrorCode = false);
    static void ErrorMessageBoxCallBack(void* pData, uint uiButton);
    bool        IsOfflineMod() { return m_bIsOfflineMod; }
    const char* GetModInstallRoot(const char* szModName);
    bool        CheckDiskSpace(uint uiResourcesPathMinMB = 10, uint uiDataPathMinMB = 10);

    // Subsystems
    void CreateGame();
    void CreateMultiplayer();
    void CreateNetwork();
    void CreateXML();
    void InitGUI(IDirect3DDevice9* pDevice);
    void CreateGUI();
    void DestroyGame();
    void DestroyMultiplayer();
    void DestroyNetwork();
    void DestroyXML();
    void DeinitGUI();
    void DestroyGUI();

    // Web
    bool IsWebCoreLoaded() { return m_pWebCore != nullptr; }
    void DestroyWeb();

    // Hooks
    void              ApplyHooks();
    void              ApplyHooks2();
    void              ApplyHooks3(bool bEnable);
    HWND              GetHookedWindow();
    void              SwitchRenderWindow(HWND hWnd, HWND hWndInput);
    void              CallSetCursorPos(int X, int Y);
    void              SetClientMessageProcessor(pfnProcessMessage pfnMessageProcessor) { m_pfnMessageProcessor = pfnMessageProcessor; };
    pfnProcessMessage GetClientMessageProcessor() { return m_pfnMessageProcessor; }
    void              ChangeResolution(long width, long height, long depth);

    bool IsFocused() { return (GetForegroundWindow() == GetHookedWindow()); };
    bool IsWindowMinimized();
    void UpdateIsWindowMinimized();

    // Pulse
    void DoPreFramePulse();
    void DoPostFramePulse();

    // Events
    void OnModUnload();

    // Misc
    void RegisterCommands();
    bool IsValidNick(const char* szNick);            // Move somewhere else
    void Quit(bool bInstantly = true);
    void InitiateUpdate(const char* szType, const char* szData, const char* szHost) { m_pLocalGUI->InitiateUpdate(szType, szData, szHost); }
    bool IsOptionalUpdateInfoRequired(const char* szHost) { return m_pLocalGUI->IsOptionalUpdateInfoRequired(szHost); }
    void InitiateDataFilesFix() { m_pLocalGUI->InitiateDataFilesFix(); }

    uint GetFrameRateLimit() { return m_uiFrameRateLimit; }
    void RecalculateFrameRateLimit(uint uiServerFrameRateLimit = -1, bool bLogToConsole = true);
    void ApplyFrameRateLimit(uint uiOverrideRate = -1);
    void ApplyQueuedFrameRateLimit();
    void EnsureFrameRateLimitApplied();
    void SetClientScriptFrameRateLimit(uint uiClientScriptFrameRateLimit);
    void DoReliablePulse();

    bool IsTimingCheckpoints();
    void OnTimingCheckpoint(const char* szTag);
    void OnTimingDetail(const char* szTag);

    void CalculateStreamingMemoryRange();
    uint GetMinStreamingMemory();
    uint GetMaxStreamingMemory();

    SString GetConnectCommandFromURI(const char* szURI);
    void    GetConnectParametersFromURI(const char* szURI, std::string& strHost, unsigned short& usPort, std::string& strNick, std::string& strPassword);
    bool    bScreenShot;
    std::map<std::string, std::string>& GetCommandLineOptions() { return m_CommandLineOptions; }
    const char*                         GetCommandLineOption(const char* szOption);
    const char*                         GetCommandLineArgs() { return m_szCommandLineArgs; }
    void                                RequestNewNickOnStart() { m_bWaitToSetNick = true; };
    bool                                WillRequestNewNickOnStart() { return m_bWaitToSetNick; };
    bool                                WasLaunchedWithConnectURI();
    void                                HandleCrashDumpEncryption();

    void                 OnPreFxRender();
    void                 OnPreHUDRender();
    void                 OnDeviceRestore();
    void                 OnCrashAverted(uint uiId);
    void                 OnEnterCrashZone(uint uiId);
    void                 LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId = 0);
    bool                 GetDebugIdEnabled(uint uiDebugId);
    EDiagnosticDebugType GetDiagnosticDebug();
    void                 SetDiagnosticDebug(EDiagnosticDebugType value);
    CModelCacheManager*  GetModelCacheManager();
    void                 AddModelToPersistentCache(ushort usModelId);

    static void StaticIdleHandler();
    void        IdleHandler();
    void        WindowsTimerHandler();
    void        HandleIdlePulse();
    void        SetModulesLoaded(bool bLoaded);
    bool        AreModulesLoaded();
    void        UpdateDummyProgress(int iValue = -1, const char* szType = "");
    void        SetDummyProgressUpdateAlways(bool bAlways) { m_bDummyProgressUpdateAlways = bAlways; }
    bool        GetDummyProgressUpdateAlways() { return m_bDummyProgressUpdateAlways; }

    void        OnPreCreateDevice(IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD& BehaviorFlags,
                                  D3DPRESENT_PARAMETERS* pPresentationParameters);
    HRESULT     OnPostCreateDevice(HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                   D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
    bool        GetDeviceSelectionEnabled();
    bool        GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex, bool& bOutAllowUnsafeResolutions);
    void        NotifyRenderingGrass(bool bIsRenderingGrass);
    bool        IsRenderingGrass() { return m_bIsRenderingGrass; }
    bool        GetRightSizeTxdEnabled();
    const char* GetProductRegistryPath() { return SharedUtil::GetProductRegistryPath(); }
    const char* GetProductCommonDataDir() { return SharedUtil::GetProductCommonDataDir(); }
    const char* GetProductVersion() { return SharedUtil::GetProductVersion(); }
    void        SetFakeLagCommandEnabled(bool bEnabled) { m_bFakeLagCommandEnabled = bEnabled; }
    bool        IsFakeLagCommandEnabled() { return m_bFakeLagCommandEnabled; }
    SString     GetBlueCopyrightString();
    HANDLE      SetThreadHardwareBreakPoint(HANDLE hThread, HWBRK_TYPE Type, HWBRK_SIZE Size, DWORD dwAddress);
private:
    // Core devices.
    CXML*               m_pXML = nullptr;
    CLocalGUI*          m_pLocalGUI = nullptr;
    CGraphics*          m_pGraphics = nullptr;
    CCommands*          m_pCommands = nullptr;
    CDirect3DData*      m_pDirect3DData = nullptr;
    CConnectManager*    m_pConnectManager = nullptr;
    CModelCacheManager* m_pModelCacheManager = nullptr;

    // Instances (put new classes here!)
    CXMLFile*          m_pConfigFile = nullptr;
    CClientVariables   m_ClientVariables;
    CWebCoreInterface* m_pWebCore = nullptr;
    CTrayIcon*         m_pTrayIcon = nullptr;

    // Hook interfaces.
    CMessageLoopHook*        m_pMessageLoopHook = nullptr;
    CDirectInputHookManager* m_pDirectInputHookManager = nullptr;
    CDirect3DHookManager*    m_pDirect3DHookManager = nullptr;
    // CFileSystemHook *           m_pFileSystemHook;
    CSetCursorPosHook* m_pSetCursorPosHook = nullptr;

    bool m_bLastFocused = false;
    int  m_iUnminimizeFrameCounter = 0;
    bool m_bDidRecreateRenderTargets = false;
    bool m_bIsWindowMinimized = false;

    // Module loader objects.
    CModuleLoader m_GameModule;
    CModuleLoader m_MultiplayerModule;
    CModuleLoader m_NetModule;
    CModuleLoader m_XMLModule;
    CModuleLoader m_GUIModule;
    CModuleLoader m_WebCoreModule;

    // Mod manager
    CModManager* m_pModManager = nullptr;

    // Module interfaces.
    CGame*        m_pGame = nullptr;
    CNet*         m_pNet = nullptr;
    CMultiplayer* m_pMultiplayer = nullptr;
    CGUI*         m_pGUI = nullptr;

    // Logger utility interface.
    CConsoleLogger* m_pConsoleLogger = nullptr;

    CLocalization* m_pLocalization = nullptr;

    CKeyBinds*     m_pKeyBinds = nullptr;
    CMouseControl* m_pMouseControl = nullptr;

    bool              m_bFirstFrame = false;
    bool              m_bIsOfflineMod = false;
    bool              m_bCursorToggleControls = false;
    pfnProcessMessage m_pfnMessageProcessor = nullptr;

    CGUIMessageBox* m_pMessageBox = nullptr;

    // screen res
    DEVMODE m_Current;

    SString m_strModInstallRoot;

    bool m_bQuitOnPulse = false;
    bool m_bDestroyMessageBox = false;

    bool                 m_bDoneFrameRateLimit = false;
    uint                 m_uiServerFrameRateLimit = 0;
    uint                 m_uiClientScriptFrameRateLimit = 0;
    uint                 m_uiFrameRateLimit = 0;
    CElapsedTimeHD       m_FrameRateTimer;
    uint                 m_uiQueuedFrameRate = 0;
    bool                 m_bQueuedFrameRateValid = false;
    bool                 m_bWaitToSetNick = false;
    uint                 m_uiNewNickWaitFrames = 0;
    EDiagnosticDebugType m_DiagnosticDebug = EDiagnosticDebugType::NONE;
    float                m_fMinStreamingMemory = 0.0f;
    float                m_fMaxStreamingMemory = 0.0f;
    bool                 m_bGettingIdleCallsFromMultiplayer = false;
    bool                 m_bWindowsTimerEnabled = false;
    bool                 m_bModulesLoaded = false;
    int                  m_iDummyProgressValue = 0;
    HANDLE               m_DummyProgressTimerHandle = nullptr;
    SString              m_strDummyProgressType;
    bool                 m_bDummyProgressUpdateAlways = false;
    bool                 m_bIsRenderingGrass = false;
    bool                 m_bFakeLagCommandEnabled = false;

    // Command line
    static void                        ParseCommandLine(std::map<std::string, std::string>& options, const char*& szArgs, const char** pszNoValOptions = NULL);
    std::map<std::string, std::string> m_CommandLineOptions;            // e.g. "-o option" -> {"o" = "option"}
    const char*                        m_szCommandLineArgs;             // Everything that comes after the options
};

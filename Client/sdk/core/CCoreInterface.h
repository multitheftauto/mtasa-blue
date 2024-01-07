/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CCoreInterface.h
 *  PURPOSE:     Core interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CConsoleInterface.h"
#include "CCommandsInterface.h"
#include "CRenderItemManagerInterface.h"
#include "CScreenGrabberInterface.h"
#include "CPixelsManagerInterface.h"
#include "CGraphicsInterface.h"
#include "CModManagerInterface.h"
#include "CKeyBindsInterface.h"
#include "CCVarsInterface.h"
#include "CWebCoreInterface.h"
#include "CTrayIconInterface.h"
#include "CChatInterface.h"
#include "CDiscordInterface.h"
#include "effekseer/CEffekseer.h"
#include "xml/CXML.h"
#include <gui/CGUI.h>

typedef bool (*pfnProcessMessage)(HWND, UINT, WPARAM, LPARAM);

class CMultiplayer;
class CNet;
class CGame;
class CModelCacheManager;
class CLocalizationInterface;

enum eCoreVersion
{
    MTACORE_20 = 1,
};

#ifndef WITH_TIMING_CHECKPOINTS
    #define WITH_TIMING_CHECKPOINTS 1            // Comment this line to remove timing checkpoint code
#endif

#if WITH_TIMING_CHECKPOINTS
    #define IS_TIMING_CHECKPOINTS() g_pCore->IsTimingCheckpoints()
    #define TIMING_CHECKPOINT(x)    g_pCore->OnTimingCheckpoint(x)
    #define TIMING_DETAIL(x)        g_pCore->OnTimingDetail(x)
#else
    #define IS_TIMING_CHECKPOINTS() (false)
    #define TIMING_CHECKPOINT(x) \
        { \
        }
    #define TIMING_DETAIL(x) \
        { \
        }
#endif

class CCoreInterface
{
public:
    // Note: Always leave this on the top as the client must use this to verify
    //       correct MTA version before trying to use any other interface funcs.
    virtual eCoreVersion GetVersion() = 0;

    virtual CConsoleInterface*                 GetConsole() = 0;
    virtual CCommandsInterface*                GetCommands() = 0;
    virtual CGame*                             GetGame() = 0;
    virtual CGraphicsInterface*                GetGraphics() = 0;
    virtual CGUI*                              GetGUI() = 0;
    virtual CModManagerInterface*              GetModManager() = 0;
    virtual CMultiplayer*                      GetMultiplayer() = 0;
    virtual CNet*                              GetNetwork() = 0;
    virtual CXML*                              GetXML() = 0;
    virtual CKeyBindsInterface*                GetKeyBinds() = 0;
    virtual CXMLNode*                          GetConfig() = 0;
    virtual CCVarsInterface*                   GetCVars() = 0;
    virtual CLocalizationInterface*            GetLocalization() = 0;
    virtual CWebCoreInterface*                 GetWebCore() = 0;
    virtual CTrayIconInterface*                GetTrayIcon() = 0;
    virtual std::shared_ptr<CDiscordInterface> GetDiscord() = 0;
    virtual CEffekseer*        GetEffekseer() = 0;

    // Temporary functions for r1
    virtual void DebugEcho(const char* szText) = 0;
    virtual void DebugPrintf(const char* szFormat, ...) = 0;
    virtual void SetDebugVisible(bool bVisible) = 0;
    virtual bool IsDebugVisible() = 0;
    virtual void DebugEchoColor(const char* szText, unsigned char R, unsigned char G, unsigned char B) = 0;
    virtual void DebugPrintfColor(const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ...) = 0;
    virtual void DebugClear() = 0;
    virtual void ChatEcho(const char* szText, bool bColorCoded = false) = 0;
    virtual void ChatEchoColor(const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false) = 0;
    virtual void ChatPrintf(const char* szFormat, bool bColorCoded, ...) = 0;
    virtual void ChatPrintfColor(const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ...) = 0;
    virtual void SetChatVisible(bool bVisible, bool bInputBlocked = true) = 0;
    virtual bool IsChatVisible() = 0;
    virtual void InitiateScreenShot(bool bCameraShot) = 0;
    virtual void EnableChatInput(char* szCommand, DWORD dwColor) = 0;
    virtual bool IsChatInputEnabled() = 0;
    virtual bool IsSettingsVisible() = 0;
    virtual bool IsMenuVisible() = 0;
    virtual bool IsCursorForcedVisible() = 0;
    virtual bool IsCursorControlsToggled() = 0;
    virtual void CallSetCursorPos(int X, int Y) = 0;

    virtual void SetConnected(bool bConnected) = 0;
    virtual void SetOfflineMod(bool bOffline) = 0;
    virtual void ApplyHooks3(bool bEnable) = 0;
    virtual bool IsWebCoreLoaded() = 0;

    virtual bool IsConnected() = 0;
    virtual bool Reconnect(const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true) = 0;

    virtual const char* GetModInstallRoot(const char* szModName) = 0;
    virtual bool        CheckDiskSpace(uint uiResourcesPathMinMB = 10, uint uiDataPathMinMB = 10) = 0;

    virtual void ShowServerInfo(unsigned int WindowType) = 0;

    virtual void ForceCursorVisible(bool bVisible, bool bToggleControls = true) = 0;
    virtual void SetMessageProcessor(pfnProcessMessage pfnMessageProcessor) = 0;
    virtual void ShowMessageBox(const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK* ResponseHandler = NULL) = 0;
    virtual void RemoveMessageBox(bool bNextFrame = false) = 0;
    virtual void ShowErrorMessageBox(const SString& strTitle, SString strMessage, const SString& strTroubleLink = "") = 0;
    virtual void ShowNetErrorMessageBox(const SString& strTitle, SString strMessage, SString strTroubleLink = "", bool bLinkRequiresErrorCode = false) = 0;
    virtual void HideMainMenu() = 0;
    virtual HWND GetHookedWindow() = 0;
    virtual bool IsFocused() = 0;
    virtual bool IsWindowMinimized() = 0;

    virtual void SaveConfig(bool bWaitUntilFinished = false) = 0;
    virtual void UpdateRecentlyPlayed() = 0;

    virtual void SwitchRenderWindow(HWND hWnd, HWND hWndInput) = 0;
    virtual void SetCenterCursor(bool bEnabled) = 0;
    virtual bool IsTimingCheckpoints() = 0;
    virtual void OnTimingCheckpoint(const char* szTag) = 0;
    virtual void OnTimingDetail(const char* szTag) = 0;

    virtual void Quit(bool bInstantly = true) = 0;
    virtual void InitiateUpdate(const char* szType, const char* szData, const char* szHost) = 0;
    virtual bool IsOptionalUpdateInfoRequired(const char* szHost) = 0;
    virtual void InitiateDataFilesFix() = 0;

    virtual uint GetFrameRateLimit() = 0;
    virtual void RecalculateFrameRateLimit(uint uiServerFrameRateLimit = -1, bool bLogToConsole = true) = 0;
    virtual void ApplyFrameRateLimit(uint uiOverrideRate = -1) = 0;
    virtual void EnsureFrameRateLimitApplied() = 0;
    virtual void SetClientScriptFrameRateLimit(uint uiClientScriptFrameRateLimit) = 0;

    virtual void                 OnPreFxRender() = 0;
    virtual void                 OnPreHUDRender() = 0;
    virtual uint                 GetMinStreamingMemory() = 0;
    virtual uint                 GetMaxStreamingMemory() = 0;
    virtual void                 OnCrashAverted(uint uiId) = 0;
    virtual void                 OnEnterCrashZone(uint uiId) = 0;
    virtual void                 LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId = 0) = 0;
    virtual bool                 GetDebugIdEnabled(uint uiDebugId) = 0;
    virtual EDiagnosticDebugType GetDiagnosticDebug() = 0;
    virtual void                 SetDiagnosticDebug(EDiagnosticDebugType value) = 0;
    virtual CModelCacheManager*  GetModelCacheManager() = 0;
    virtual void                 AddModelToPersistentCache(ushort usModelId) = 0;
    virtual void                 UpdateDummyProgress(int iValue = -1, const char* szType = "") = 0;
    virtual void                 SetDummyProgressUpdateAlways(bool bAlways) = 0;

    virtual void    OnPreCreateDevice(IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD& BehaviorFlags,
                                      D3DPRESENT_PARAMETERS* pPresentationParameters) = 0;
    virtual HRESULT OnPostCreateDevice(HRESULT hResult, IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                       D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) = 0;
    virtual bool    GetDeviceSelectionEnabled() = 0;
    virtual bool GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex, bool& bOutAllowUnsafeResolutions) = 0;
    virtual void NotifyRenderingGrass(bool bIsRenderingGrass) = 0;
    virtual bool GetRightSizeTxdEnabled() = 0;
    virtual const char* GetProductRegistryPath() = 0;
    virtual const char* GetProductCommonDataDir() = 0;
    virtual const char* GetProductVersion() = 0;
    virtual void        SetFakeLagCommandEnabled(bool bEnabled) = 0;
    virtual SString     GetBlueCopyrightString() = 0;
    virtual bool        ClearChat() = 0;
    virtual void        OnGameTimerUpdate() = 0;

    virtual bool IsChatInputBlocked() = 0;
    virtual bool SetChatboxCharacterLimit(int charLimit) = 0;
    virtual void ResetChatboxCharacterLimit() = 0;
    virtual int  GetChatboxCharacterLimit() = 0;
    virtual int  GetChatboxMaxCharacterLimit() = 0;

    virtual void   SetCustomStreamingMemory(size_t sizeBytes) = 0;
    virtual bool   IsUsingCustomStreamingMemorySize() = 0;
    virtual size_t GetStreamingMemory() = 0;

    virtual const SString& GetLastConnectedServerName() const = 0;
    virtual void           SetLastConnectedServerName(const SString& strServerName) = 0;
};

class CClientTime
{
public:
    static unsigned long GetTime() { return GetTickCount32(); }
};

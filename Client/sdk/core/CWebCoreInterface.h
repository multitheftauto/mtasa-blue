/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CWebBrowserInterface.h
 *  PURPOSE:     Webbrowser interface class
 *
 *****************************************************************************/
#pragma once
#include <unordered_set>

class CWebBrowserItem;
class CWebViewInterface;
class CWebView;

enum class eURLState
{
    WEBPAGE_NOT_LISTED,
    WEBPAGE_ALLOWED,
    WEBPAGE_DISALLOWED
};

enum class eWebFilterType
{
    WEBFILTER_HARDCODED,
    WEBFILTER_DYNAMIC,
    WEBFILTER_USER,
    WEBFILTER_REQUEST
};

enum class eWebFilterState
{
    WEBFILTER_ALL,
    WEBFILTER_ALLOWED,
    WEBFILTER_DISALLOWED
};

enum eWebBrowserMouseButton
{
    BROWSER_MOUSEBUTTON_LEFT = 0,
    BROWSER_MOUSEBUTTON_MIDDLE = 1,
    BROWSER_MOUSEBUTTON_RIGHT = 2
};

using WebRequestCallback = std::function<void(bool, const std::unordered_set<SString>&)>;

class CWebCoreInterface
{
public:
    virtual ~CWebCoreInterface() {}
    virtual bool Initialise(bool gpuEnabled, bool gpuCompositingEnabled) = 0;

    virtual CWebViewInterface* CreateWebView(unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem,
                                             bool bTransparent) = 0;
    virtual void               DestroyWebView(CWebViewInterface* pWebView) = 0;
    virtual void               DoPulse() = 0;

    virtual void AddEventToEventQueue(std::function<void()> func, CWebView* pWebView, const SString& name) = 0;
    virtual void RemoveWebViewEvents(CWebView* pWebView) = 0;
    virtual void DoEventQueuePulse() = 0;

    virtual void WaitForTask(std::function<void(bool)> task, CWebView* webView) = 0;

    virtual eURLState                    GetDomainState(const SString& strURL, bool bOutputDebug = false) = 0;
    virtual SString                      GetDomainFromURL(const SString& strURL) = 0;
    virtual void                         ResetFilter(bool bResetRequestsOnly = true) = 0;
    virtual void                         RequestPages(const std::vector<SString>& pages, WebRequestCallback* pCallback = nullptr) = 0;
    virtual std::unordered_set<SString>  AllowPendingPages(bool bRemember) = 0;
    virtual std::unordered_set<SString>  DenyPendingPages() = 0;
    virtual std::unordered_set<SString>& GetPendingRequests() = 0;
    virtual bool                         IsRequestsGUIVisible() = 0;

    virtual bool IsTestModeEnabled() = 0;
    virtual void SetTestModeEnabled(bool bEnabled) = 0;

    virtual CWebViewInterface* GetFocusedWebView() = 0;
    virtual void               SetFocusedWebView(CWebView* pWebView) = 0;
    virtual void               ProcessInputMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    virtual void               ClearTextures() = 0;

    virtual bool GetRemotePagesEnabled() = 0;
    virtual bool GetRemoteJavascriptEnabled() = 0;

    virtual void OnPreScreenshot() = 0;
    virtual void OnPostScreenshot() = 0;

    virtual bool SetGlobalAudioVolume(float fVolume) = 0;

    virtual void WriteCustomList(const SString& strListName, const std::vector<SString>& customList, bool bReset = true) = 0;
    virtual void GetFilterEntriesByType(std::vector<std::pair<SString, bool>>& outEntries, eWebFilterType filterType,
                                        eWebFilterState state = eWebFilterState::WEBFILTER_ALL) = 0;

    virtual bool GetGPUEnabled() const noexcept = 0;
    virtual bool GetGPUCompositingEnabled() const noexcept = 0;
};

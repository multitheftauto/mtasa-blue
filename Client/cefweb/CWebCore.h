/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebCore.h
 *  PURPOSE:     Webbrowser class
 *
 *****************************************************************************/
#pragma once

#undef GetNextSibling
#undef GetFirstChild
#include <functional>
#include <mutex>
#include <unordered_set>
#include <future>
#include <core/CWebCoreInterface.h>
#include <cef3/cef/include/cef_app.h>
#define MTA_BROWSERDATA_PATH "mta/cef/browserdata.xml"
#define BROWSER_LIST_UPDATE_INTERVAL (24*60*60)
#define BROWSER_UPDATE_URL "https://cef.multitheftauto.com/get.php"
#define MAX_EVENT_QUEUE_SIZE 10000
#define MAX_TASK_QUEUE_SIZE 1000
#define MAX_WHITELIST_SIZE 50000
#define MAX_PENDING_REQUESTS 100
#define GetNextSibling(hwnd) GetWindow(hwnd, GW_HWNDNEXT) // Re-define the conflicting macro
#define GetFirstChild(hwnd) GetTopWindow(hwnd)

class CWebBrowserItem;
class CWebsiteRequests;
class CWebView;
class CWebViewInterface;

class CWebCore : public CWebCoreInterface
{
    struct EventEntry
    {
        std::function<void()> callback;
        CefRefPtr<CWebView>   pWebView;
    #ifdef MTA_DEBUG
        SString name;
    #endif

        EventEntry(const std::function<void()>& callback_, CWebView* pWebView_);
#ifdef MTA_DEBUG
        EventEntry(const std::function<void()>& callback_, CWebView* pWebView_, const SString& name_);
#endif
    };

    struct TaskEntry
    {
        std::packaged_task<void(bool)> task;
        CefRefPtr<CWebView>            webView;

        TaskEntry(std::function<void(bool)> callback, CWebView* webView);
    };

public:
    CWebCore();
    ~CWebCore();
    bool Initialise(bool gpuEnabled) override;
    bool IsInitialised() const noexcept override { return m_bInitialised; }

    CWebViewInterface* CreateWebView(unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent);
    void               DestroyWebView(CWebViewInterface* pWebViewInterface);
    void               DoPulse();
    CWebView*          FindWebView(CefRefPtr<CefBrowser> browser);

    void AddEventToEventQueue(std::function<void()> func, CWebView* pWebView, const SString& name);
    void RemoveWebViewEvents(CWebView* pWebView);
    void DoEventQueuePulse();

    void WaitForTask(std::function<void(bool)> task, CWebView* webView) override;
    void RemoveWebViewTasks(CWebView* webView);
    void DoTaskQueuePulse();

    eURLState                    GetDomainState(const SString& strURL, bool bOutputDebug = false);
    SString                      GetDomainFromURL(const SString& strURL);
    void                         ResetFilter(bool bResetRequestsOnly = true);
    void                         InitialiseWhiteAndBlacklist(bool bAddHardcoded = true, bool bAddDynamic = true);
    void                         AddAllowedPage(const SString& strURL, eWebFilterType filterType);
    void                         AddBlockedPage(const SString& strURL, eWebFilterType filterType);
    void                         RequestPages(const std::vector<SString>& pages, WebRequestCallback* pCallback = nullptr);
    std::unordered_set<SString>  AllowPendingPages(bool bRemember);
    std::unordered_set<SString>  DenyPendingPages();
    std::unordered_set<SString>& GetPendingRequests() { return m_PendingRequests; };
    bool                         IsRequestsGUIVisible();

    bool IsTestModeEnabled() { return m_bTestmodeEnabled; };
    void SetTestModeEnabled(bool bEnabled) { m_bTestmodeEnabled = bEnabled; };
    void DebugOutputThreadsafe(const SString& message, unsigned char R, unsigned char G, unsigned char B);

    CWebViewInterface* GetFocusedWebView();
    void               SetFocusedWebView(CWebView* pWebView) { m_pFocusedWebView = pWebView; };
    void               ProcessInputMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void               ClearTextures();

    bool GetRemotePagesEnabled();
    bool GetRemoteJavascriptEnabled();

    void OnPreScreenshot();
    void OnPostScreenshot();

    void OnFPSLimitChange(std::uint16_t fps) override;

    bool SetGlobalAudioVolume(float fVolume);

    bool        UpdateListsFromMaster();
    bool        MakeSureXMLNodesExist();
    void        LoadListsFromXML(bool bWhitelist, bool bBlacklist, bool bCustomLists);
    void        WriteCustomList(const SString& strListName, const std::vector<SString>& customList, bool bReset = true);
    void        GetFilterEntriesByType(std::vector<std::pair<SString, bool>>& outEntries, eWebFilterType filterType,
                                       eWebFilterState state = eWebFilterState::WEBFILTER_ALL);
    static void StaticFetchRevisionFinished(const SHttpDownloadResult& result);
    static void StaticFetchWhitelistFinished(const SHttpDownloadResult& result);
    static void StaticFetchBlacklistFinished(const SHttpDownloadResult& result);

    bool GetGPUEnabled() const noexcept;

private:
    typedef std::pair<bool, eWebFilterType> WebFilterPair;

    CWebsiteRequests*              m_pRequestsGUI;
    std::list<CefRefPtr<CWebView>> m_WebViews;
    bool                           m_bTestmodeEnabled;
    CWebView*                      m_pFocusedWebView;

    std::list<EventEntry> m_EventQueue;
    std::mutex            m_EventQueueMutex;

    std::list<TaskEntry> m_TaskQueue;
    std::mutex           m_TaskQueueMutex;

    CFastHashMap<SString, WebFilterPair> m_Whitelist;
    std::unordered_set<SString>          m_PendingRequests;
    std::recursive_mutex                 m_FilterMutex;

    CXMLFile* m_pXmlConfig;
    int       m_iWhitelistRevision;
    int       m_iBlacklistRevision;

    // Shouldn't be changed after init
    bool m_bGPUEnabled;
    bool m_bInitialised = false;            // Track if CefInitialize() succeeded

    // ===== AUTH: IPC message validation =====
public:
    std::string m_AuthCode;            // Random 30-char code for validating browser IPC messages
    // ===== END AUTH =====
};

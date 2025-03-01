/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebCore.cpp
 *  PURPOSE:     Webbrowser class
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CWebCore.h"
#include "CWebView.h"
#include "CWebsiteRequests.h"
#include <cef3/cef/include/cef_app.h>
#include <cef3/cef/include/cef_browser.h>
#include <cef3/cef/include/cef_sandbox_win.h>
#include <cef3/cef/include/cef_parser.h>
#include "WebBrowserHelpers.h"
#include "CWebApp.h"

// #define CEF_ENABLE_SANDBOX
#ifdef CEF_ENABLE_SANDBOX
    #pragma comment(lib, "cef_sandbox.lib")
#endif

CWebCore::CWebCore()
{
    m_pRequestsGUI = nullptr;
    m_bTestmodeEnabled = false;
    m_pXmlConfig = nullptr;
    m_pFocusedWebView = nullptr;

    MakeSureXMLNodesExist();
    InitialiseWhiteAndBlacklist();

    // Update dynamic lists from QA server
    UpdateListsFromMaster();
}

CWebCore::~CWebCore()
{
    // Unregister schema factories
    CefClearSchemeHandlerFactories();

    // Shutdown CEF
    CefShutdown();

    delete m_pRequestsGUI;
    delete m_pXmlConfig;
}

bool CWebCore::Initialise(bool gpuEnabled)
{
    CefMainArgs        mainArgs;
    void*              sandboxInfo = nullptr;

    m_bGPUEnabled = gpuEnabled;

    CefRefPtr<CWebApp> app(new CWebApp);

#ifdef CEF_ENABLE_SANDBOX
    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    CefSettings settings;
#ifndef CEF_ENABLE_SANDBOX
    settings.no_sandbox = true;
#endif

    // Specifiy sub process executable path
#ifndef MTA_DEBUG
    CefString(&settings.browser_subprocess_path).FromWString(FromUTF8(CalcMTASAPath("MTA\\CEF\\CEFLauncher.exe")));
#else
    CefString(&settings.browser_subprocess_path).FromWString(FromUTF8(CalcMTASAPath("MTA\\CEF\\CEFLauncher_d.exe")));
#endif
    CefString(&settings.cache_path).FromWString(FromUTF8(CalcMTASAPath("MTA\\CEF\\cache")));
    CefString(&settings.locales_dir_path).FromWString(FromUTF8(CalcMTASAPath("MTA\\CEF\\locales")));
    CefString(&settings.log_file).FromWString(FromUTF8(CalcMTASAPath("MTA\\CEF\\cefdebug.txt")));
#ifdef MTA_DEBUG
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_INFO;
#else
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_WARNING;
#endif

    settings.multi_threaded_message_loop = true;
    settings.windowless_rendering_enabled = true;

    bool state = CefInitialize(mainArgs, settings, app, sandboxInfo);

    // Register custom scheme handler factory
    CefRegisterSchemeHandlerFactory("http", "mta", app);

    return state;
}

CWebViewInterface* CWebCore::CreateWebView(unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem,
                                           bool bTransparent)
{
    // Create our webview implementation
    CefRefPtr<CWebView> pWebView = new CWebView(bIsLocal, pWebBrowserRenderItem, bTransparent);
    m_WebViews.push_back(pWebView);

    return static_cast<CWebViewInterface*>(pWebView.get());
}

void CWebCore::DestroyWebView(CWebViewInterface* pWebViewInterface)
{
    CefRefPtr<CWebView> pWebView = dynamic_cast<CWebView*>(pWebViewInterface);
    if (pWebView)
    {
        // Ensure that no attached events or tasks are in the queue
        RemoveWebViewEvents(pWebView.get());
        RemoveWebViewTasks(pWebView.get());

        m_WebViews.remove(pWebView);
        // pWebView->Release(); // Do not release since other references get corrupted then
        pWebView->CloseBrowser();
    }
}

void CWebCore::DoPulse()
{
    // Check for queued whitelist/blacklist downloads
    g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadModeType::WEBBROWSER_LISTS)->ProcessQueuedFiles();

    // Execute queued tasks on the main thread
    DoTaskQueuePulse();

    // Execute queued events on the main thread (Lua calls must be executed on the main thread)
    DoEventQueuePulse();
}

CWebView* CWebCore::FindWebView(CefRefPtr<CefBrowser> browser)
{
    if (!browser)
        return nullptr;

    for (auto pWebView : m_WebViews)
    {
        if (!pWebView)
            continue;

        CefRefPtr<CefBrowser> pBrowser = pWebView->GetCefBrowser();

        if (!pBrowser)
            continue;

        // CefBrowser objects are not unique
        if (pBrowser->GetIdentifier() == browser->GetIdentifier())
            return pWebView.get();
    }

    return nullptr;
}

void CWebCore::AddEventToEventQueue(std::function<void()> event, CWebView* pWebView, const SString& name)
{
#ifndef MTA_DEBUG
    UNREFERENCED_PARAMETER(name);
#endif
    if (pWebView && pWebView->IsBeingDestroyed())
        return;

    std::lock_guard<std::mutex> lock(m_EventQueueMutex);

#ifndef MTA_DEBUG
    m_EventQueue.push_back(EventEntry(event, pWebView));
#else
    m_EventQueue.push_back(EventEntry(event, pWebView, name));
#endif
}

void CWebCore::RemoveWebViewEvents(CWebView* pWebView)
{
    std::lock_guard<std::mutex> lock(m_EventQueueMutex);

    for (auto iter = m_EventQueue.begin(); iter != m_EventQueue.end();)
    {
        // Increment iterator before we remove the element from the list (to guarantee iterator validity)
        auto tempIterator = iter++;
        if (tempIterator->pWebView == pWebView)
            m_EventQueue.erase(tempIterator);
    }
}

void CWebCore::DoEventQueuePulse()
{
    std::list<EventEntry> eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_EventQueueMutex);
        std::swap(eventQueue, m_EventQueue);
    }

    for (auto& event : eventQueue)
    {
        event.callback();
    }

    // Invoke paint method if necessary on the main thread
    for (auto& view : m_WebViews)
    {
        view->UpdateTexture();
    }
}

void CWebCore::WaitForTask(std::function<void(bool)> task, CWebView* webView)
{
    if (!webView || webView->IsBeingDestroyed())
        return;

    // NOTE: Tasks are processed in the main thread: NEVER call this method in the main thread
    assert(!IsMainThread());

    std::future<void> result;
    {
        std::scoped_lock lock(m_TaskQueueMutex);
        m_TaskQueue.emplace_back(TaskEntry{task, webView});
        result = m_TaskQueue.back().task.get_future();
    }

    result.get();
}

void CWebCore::RemoveWebViewTasks(CWebView* webView)
{
    std::scoped_lock lock(m_TaskQueueMutex);

    for (auto iter = m_TaskQueue.begin(); iter != m_TaskQueue.end(); ++iter)
    {
        if (iter->webView != webView)
            continue;

        iter->task(true);
        iter = m_TaskQueue.erase(iter);
    }
}

void CWebCore::DoTaskQueuePulse()
{
    std::list<TaskEntry> taskQueue;
    {
        std::scoped_lock lock(m_TaskQueueMutex);
        std::swap(m_TaskQueue, taskQueue);
    }

    for (TaskEntry& entry : taskQueue)
    {
        entry.task(false);
    }
}

eURLState CWebCore::GetDomainState(const SString& strURL, bool bOutputDebug)
{
    std::lock_guard<std::recursive_mutex> lock(m_FilterMutex);

    // Initialize wildcard whitelist (be careful with modifying) | Todo: Think about the following
    static SString wildcardWhitelist[] = {"*.googlevideo.com", "*.google.com",  "*.youtube.com",    "*.ytimg.com",
                                          "*.vimeocdn.com",    "*.gstatic.com", "*.googleapis.com", "*.ggpht.com"};

    for (int i = 0; i < sizeof(wildcardWhitelist) / sizeof(SString); ++i)
    {
        if (WildcardMatch(wildcardWhitelist[i], strURL))
            return eURLState::WEBPAGE_ALLOWED;
    }

    google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.find(strURL);
    if (iter != m_Whitelist.end())
    {
        if (iter->second.first == true)
            return eURLState::WEBPAGE_ALLOWED;
        else
        {
            if (m_bTestmodeEnabled && bOutputDebug)
                DebugOutputThreadsafe(SString("[BROWSER] Blocked page: %s", strURL.c_str()), 255, 0, 0);
            return eURLState::WEBPAGE_DISALLOWED;
        }
    }

    if (m_bTestmodeEnabled && bOutputDebug)
        DebugOutputThreadsafe(SString("[BROWSER] Blocked page: %s", strURL.c_str()), 255, 0, 0);
    return eURLState::WEBPAGE_NOT_LISTED;
}

SString CWebCore::GetDomainFromURL(const SString& strURL)
{
    CefURLParts urlParts;
    if (strURL.empty() || !CefParseURL(strURL, urlParts) || !urlParts.host.str)
        return "";

    return UTF16ToMbUTF8(urlParts.host.str);
}

void CWebCore::ResetFilter(bool bResetRequestsOnly)
{
    std::lock_guard<std::recursive_mutex> lock(m_FilterMutex);

    // Clear old data
    m_PendingRequests.clear();

    if (!bResetRequestsOnly)
    {
        m_Whitelist.clear();

        // Re-add "default" entries
        InitialiseWhiteAndBlacklist();
    }
    else
    {
        // Close requests GUI
        if (m_pRequestsGUI)
        {
            delete m_pRequestsGUI;
            m_pRequestsGUI = nullptr;
        }

        // Erase all WEBFILTER_REQUEST entries
        google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.begin();
        for (; iter != m_Whitelist.end();)
        {
            if (iter->second.second == eWebFilterType::WEBFILTER_REQUEST)
                m_Whitelist.erase(iter++);
            else
                ++iter;
        }
    }
}

void CWebCore::InitialiseWhiteAndBlacklist(bool bAddHardcoded, bool bAddDynamic)
{
    if (bAddDynamic)
    {
        // Hardcoded whitelist
        static SString whitelist[] = {"google.com",         "youtube.com", "www.youtube-nocookie.com", "vimeo.com", "player.vimeo.com",
                                      "code.jquery.com",    "mtasa.com",   "multitheftauto.com",       "mtavc.com", "www.googleapis.com",
                                      "ajax.googleapis.com"};

        // Hardcoded blacklist
        static SString blacklist[] = {"nobrain.dk"};

        // Blacklist or whitelist URLs now
        for (unsigned int i = 0; i < sizeof(whitelist) / sizeof(SString); ++i)
        {
            AddAllowedPage(whitelist[i], eWebFilterType::WEBFILTER_HARDCODED);
        }
        for (unsigned int i = 0; i < sizeof(blacklist) / sizeof(SString); ++i)
        {
            AddBlockedPage(blacklist[i], eWebFilterType::WEBFILTER_HARDCODED);
        }
    }

    // Load dynamic and custom blacklist from XML config
    if (bAddDynamic)
        LoadListsFromXML(true, true, true);
}

void CWebCore::AddAllowedPage(const SString& strURL, eWebFilterType filterType)
{
    std::lock_guard<std::recursive_mutex> lock(m_FilterMutex);

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType>(true, filterType);
}

void CWebCore::AddBlockedPage(const SString& strURL, eWebFilterType filterType)
{
    std::lock_guard<std::recursive_mutex> lock(m_FilterMutex);

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType>(false, filterType);
}

void CWebCore::RequestPages(const std::vector<SString>& pages, WebRequestCallback* pCallback)
{
    // Add to pending pages queue
    bool bNewItem = false;
    for (const auto& page : pages)
    {
        eURLState status = GetDomainState(page);
        if (status == eURLState::WEBPAGE_ALLOWED || status == eURLState::WEBPAGE_DISALLOWED)
            continue;

        m_PendingRequests.insert(page);
        bNewItem = true;
    }

    if (bNewItem)
    {
        // Open CEGUI dialog
        if (!m_pRequestsGUI)
            m_pRequestsGUI = new CWebsiteRequests;

        // Set pending requests memo content and show the window
        m_pRequestsGUI->SetPendingRequests(m_PendingRequests, pCallback);
        m_pRequestsGUI->Show();
    }
    else
    {
        // Call callback immediately if nothing has changed (all entries are most likely already on the whitelist)
        // There is still the possibility that all websites are blacklisted; this is not the usual case tho, so ignore for now (TODO)
        if (pCallback)
            (*pCallback)(true, std::unordered_set<SString>(pages.begin(), pages.end()));
    }
}

std::unordered_set<SString> CWebCore::AllowPendingPages(bool bRemember)
{
    for (const auto& request : m_PendingRequests)
    {
        AddAllowedPage(request, !bRemember ? eWebFilterType::WEBFILTER_REQUEST : eWebFilterType::WEBFILTER_USER);
    }

    // Trigger an event now
    auto pCurrentMod = g_pCore->GetModManager()->GetCurrentMod();
    if (!pCurrentMod)
        return std::unordered_set<SString>();

    pCurrentMod->WebsiteRequestResultHandler(m_PendingRequests);

    if (bRemember)
    {
        std::vector<std::pair<SString, bool>> result;            // Contains only allowed entries
        g_pCore->GetWebCore()->GetFilterEntriesByType(result, eWebFilterType::WEBFILTER_USER, eWebFilterState::WEBFILTER_ALLOWED);
        std::vector<SString> customWhitelist;
        for (std::vector<std::pair<SString, bool>>::iterator iter = result.begin(); iter != result.end(); ++iter)
            customWhitelist.push_back(iter->first);

        WriteCustomList("customwhitelist", customWhitelist, false);
    }

    auto allowedRequests(std::move(m_PendingRequests));
    m_PendingRequests.clear();            // MSVC's move constructor already clears the list which isn't specified by the C++ standard though

    return allowedRequests;
}

std::unordered_set<SString> CWebCore::DenyPendingPages()
{
    auto deniedRequests(std::move(m_PendingRequests));
    m_PendingRequests.clear();

    return deniedRequests;
}

bool CWebCore::IsRequestsGUIVisible()
{
    return m_pRequestsGUI && m_pRequestsGUI->IsVisible();
}

void CWebCore::DebugOutputThreadsafe(const SString& message, unsigned char R, unsigned char G, unsigned char B)
{
    AddEventToEventQueue([message, R, G, B]() { g_pCore->DebugEchoColor(message, R, G, B); }, nullptr, "DebugOutputThreadsafe");
}

bool CWebCore::GetRemotePagesEnabled()
{
    bool bCanLoadRemotePages;
    g_pCore->GetCVars()->Get("browser_remote_websites", bCanLoadRemotePages);
    return bCanLoadRemotePages;
}

bool CWebCore::GetRemoteJavascriptEnabled()
{
    bool bIsRemoteJavascriptEnabled;
    g_pCore->GetCVars()->Get("browser_remote_javascript", bIsRemoteJavascriptEnabled);
    return bIsRemoteJavascriptEnabled;
}

void CWebCore::OnPreScreenshot()
{
    // Clear all textures
    g_pCore->GetWebCore()->ClearTextures();
}

void CWebCore::OnPostScreenshot()
{
    // Re-draw textures
    for (auto& pWebView : m_WebViews)
    {
        pWebView->GetCefBrowser()->GetHost()->Invalidate(CefBrowserHost::PaintElementType::PET_VIEW);
    }
}

void CWebCore::ProcessInputMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pFocusedWebView ||
        !(uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR || uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP))
        return;

    CefKeyEvent keyEvent;
    keyEvent.windows_key_code = wParam;
    keyEvent.native_key_code = lParam;
    keyEvent.modifiers = GetCefKeyboardModifiers(wParam, lParam);
    keyEvent.is_system_key = uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP;

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
        keyEvent.type = cef_key_event_type_t::KEYEVENT_RAWKEYDOWN;
    else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
        keyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
    else if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR)
        keyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;

    // Alt-Gr check
    if ((keyEvent.type == KEYEVENT_CHAR) && isKeyDown(VK_RMENU))
    {
        HKL   current_layout = ::GetKeyboardLayout(0);
        SHORT scan_res = ::VkKeyScanExW(wParam, current_layout);
        if ((HIBYTE(scan_res) & (2 | 4)) == (2 | 4))
        {
            keyEvent.modifiers &= ~(EVENTFLAG_CONTROL_DOWN | EVENTFLAG_ALT_DOWN);
            keyEvent.modifiers |= EVENTFLAG_ALTGR_DOWN;
        }
    }

    m_pFocusedWebView->InjectKeyboardEvent(keyEvent);
}

void CWebCore::ClearTextures()
{
    for (auto& pWebBrowser : m_WebViews)
    {
        pWebBrowser->ClearTexture();
    }
}

bool CWebCore::SetGlobalAudioVolume(float fVolume)
{
    if (fVolume < 0.0f || fVolume > 1.0f)
        return false;

    for (auto& pWebView : m_WebViews)
    {
        pWebView->SetAudioVolume(fVolume);
    }
    return true;
}

bool CWebCore::UpdateListsFromMaster()
{
    if (!m_pXmlConfig)
        return false;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode();
    if (!pRootNode || !MakeSureXMLNodesExist())
        return false;

    // Fetch white- and blacklist revision from config
    CXMLNode* pWhiteRevNode = pRootNode->FindSubNode("whitelistrev");
    if (!pWhiteRevNode || !pWhiteRevNode->GetTagContent(m_iWhitelistRevision))
    {
        m_iWhitelistRevision = 0;
    }
    CXMLNode* pBlackRevNode = pRootNode->FindSubNode("blacklistrev");
    if (!pBlackRevNode || !pBlackRevNode->GetTagContent(m_iBlacklistRevision))
    {
        m_iBlacklistRevision = 0;
    }

    // Get last update timestamp and compare with current time
    CXMLNode* pLastUpdateNode = pRootNode->FindSubNode("lastupdate");
    if (pLastUpdateNode)
    {
        SString lastUpdateTime = pLastUpdateNode->GetTagContent();

        time_t currentTime;
        time(&currentTime);

        if (lastUpdateTime < SString("%d", (long long)currentTime - BROWSER_LIST_UPDATE_INTERVAL))
        {
            OutputDebugLine("Updating white- and blacklist...");
            SHttpRequestOptions options;
            options.uiConnectionAttempts = 3;
            g_pCore->GetNetwork()
                ->GetHTTPDownloadManager(EDownloadModeType::WEBBROWSER_LISTS)
                ->QueueFile(SString("%s?type=getrev", BROWSER_UPDATE_URL), NULL, this, &CWebCore::StaticFetchRevisionFinished, options);

            pLastUpdateNode->SetTagContent(SString("%d", (long long)currentTime));
            m_pXmlConfig->Write();
        }
    }

    return true;
}

bool CWebCore::MakeSureXMLNodesExist()
{
    // Check xml file
    if (!m_pXmlConfig)
    {
        SString browserDataPath = CalcMTASAPath(MTA_BROWSERDATA_PATH);
        bool    exists = FileExists(browserDataPath);

        m_pXmlConfig = g_pCore->GetXML()->CreateXML(browserDataPath);

        if (!m_pXmlConfig || (exists && !m_pXmlConfig->Parse()))
            return false;
    }

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode();
    if (!pRootNode)
    {
        pRootNode = m_pXmlConfig->CreateRootNode("browserdata");
        if (!pRootNode)
            return false;
    }

    if (!pRootNode->FindSubNode("lastupdate"))
    {
        CXMLNode* pNode = pRootNode->CreateSubNode("lastupdate");
        if (!pNode)
            return false;

        pNode->SetTagContent(0);
    }

    if (!pRootNode->FindSubNode("whitelistrev"))
    {
        if (!pRootNode->CreateSubNode("whitelistrev"))
            return false;
    }

    if (!pRootNode->FindSubNode("blacklistrev"))
    {
        if (!pRootNode->CreateSubNode("blacklistrev"))
            return false;
    }

    if (!pRootNode->FindSubNode("globalwhitelist"))
    {
        if (!pRootNode->CreateSubNode("globalwhitelist"))
            return false;
    }

    if (!pRootNode->FindSubNode("globalblacklist"))
    {
        if (!pRootNode->CreateSubNode("globalblacklist"))
            return false;
    }

    if (!pRootNode->FindSubNode("customblacklist"))
    {
        if (!pRootNode->CreateSubNode("customblacklist"))
            return false;
    }

    if (!pRootNode->FindSubNode("customwhitelist"))
    {
        if (!pRootNode->CreateSubNode("customwhitelist"))
            return false;
    }

    return true;
}

void CWebCore::LoadListsFromXML(bool bWhitelist, bool bBlacklist, bool bCustomLists)
{
    if (!m_pXmlConfig)
        return;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode();
    if (!pRootNode)
        return;

    if (bWhitelist)
    {
        CXMLNode* pWhiteSubNode = pRootNode->FindSubNode("globalwhitelist");
        if (pWhiteSubNode)
        {
            for (std::list<CXMLNode*>::iterator iter = pWhiteSubNode->ChildrenBegin(); iter != pWhiteSubNode->ChildrenEnd(); ++iter)
            {
                AddAllowedPage((*iter)->GetTagContent(), eWebFilterType::WEBFILTER_DYNAMIC);
            }
        }
    }

    if (bBlacklist)
    {
        CXMLNode* pBlackSubNode = pRootNode->FindSubNode("globalblacklist");
        if (pBlackSubNode)
        {
            for (std::list<CXMLNode*>::iterator iter = pBlackSubNode->ChildrenBegin(); iter != pBlackSubNode->ChildrenEnd(); ++iter)
            {
                AddBlockedPage((*iter)->GetTagContent(), eWebFilterType::WEBFILTER_DYNAMIC);
            }
        }
    }

    if (bCustomLists)
    {
        CXMLNode* pBlackSubNode = pRootNode->FindSubNode("customblacklist");
        if (pBlackSubNode)
        {
            for (std::list<CXMLNode*>::iterator iter = pBlackSubNode->ChildrenBegin(); iter != pBlackSubNode->ChildrenEnd(); ++iter)
            {
                AddBlockedPage((*iter)->GetTagContent(), eWebFilterType::WEBFILTER_USER);
            }
        }
        CXMLNode* pWhiteSubNode = pRootNode->FindSubNode("customwhitelist");
        if (pWhiteSubNode)
        {
            for (std::list<CXMLNode*>::iterator iter = pWhiteSubNode->ChildrenBegin(); iter != pWhiteSubNode->ChildrenEnd(); ++iter)
            {
                AddAllowedPage((*iter)->GetTagContent(), eWebFilterType::WEBFILTER_USER);
            }
        }
    }
}

void CWebCore::WriteCustomList(const SString& strListName, const std::vector<SString>& customList, bool bReset)
{
    if (!m_pXmlConfig || !MakeSureXMLNodesExist())
        return;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode();
    if (!pRootNode)
        return;

    CXMLNode* pCustomListNode = pRootNode->FindSubNode(strListName);
    if (!pCustomListNode)
        return;

    pCustomListNode->DeleteAllSubNodes();
    for (std::vector<SString>::const_iterator iter = customList.begin(); iter != customList.end(); ++iter)
    {
        CXMLNode* pNode = pCustomListNode->CreateSubNode("url");
        if (pNode)
            pNode->SetTagContent(*iter);
    }

    // Write custom blacklist and reload from XML
    m_pXmlConfig->Write();
    if (bReset)
        ResetFilter(false);
}

void CWebCore::GetFilterEntriesByType(std::vector<std::pair<SString, bool>>& outEntries, eWebFilterType filterType, eWebFilterState state)
{
    std::lock_guard<std::recursive_mutex> lock(m_FilterMutex);

    google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.begin();
    for (; iter != m_Whitelist.end(); ++iter)
    {
        if (iter->second.second == filterType)
        {
            if (state == eWebFilterState::WEBFILTER_ALL)
                outEntries.push_back(std::pair<SString, bool>(iter->first, iter->second.first));
            else if (state == eWebFilterState::WEBFILTER_ALLOWED && iter->second.first == true)
                outEntries.push_back(std::pair<SString, bool>(iter->first, iter->second.first));
            else
                outEntries.push_back(std::pair<SString, bool>(iter->first, iter->second.first));
        }
    }
}

void CWebCore::StaticFetchRevisionFinished(const SHttpDownloadResult& result)
{
    CWebCore* pWebCore = static_cast<CWebCore*>(result.pObj);
    if (result.bSuccess)
    {
        SString strData = result.pData;
        SString strWhiteRevision, strBlackRevision;
        strData.Split(";", &strWhiteRevision, &strBlackRevision);

        if (!strWhiteRevision.empty() && !strBlackRevision.empty())
        {
            int iWhiteListRevision = atoi(strWhiteRevision);
            if (iWhiteListRevision > pWebCore->m_iWhitelistRevision)
            {
                SHttpRequestOptions options;
                options.uiConnectionAttempts = 3;
                g_pCore->GetNetwork()
                    ->GetHTTPDownloadManager(EDownloadModeType::WEBBROWSER_LISTS)
                    ->QueueFile(SString("%s?type=fetchwhite", BROWSER_UPDATE_URL), NULL, pWebCore, &CWebCore::StaticFetchWhitelistFinished, options);

                pWebCore->m_iWhitelistRevision = iWhiteListRevision;
            }
            int iBlackListRevision = atoi(strBlackRevision);
            if (iBlackListRevision > pWebCore->m_iBlacklistRevision)
            {
                SHttpRequestOptions options;
                options.uiConnectionAttempts = 3;
                g_pCore->GetNetwork()
                    ->GetHTTPDownloadManager(EDownloadModeType::WEBBROWSER_LISTS)
                    ->QueueFile(SString("%s?type=fetchblack", BROWSER_UPDATE_URL), NULL, pWebCore, &CWebCore::StaticFetchBlacklistFinished, options);

                pWebCore->m_iBlacklistRevision = iBlackListRevision;
            }
        }
    }
}

void CWebCore::StaticFetchWhitelistFinished(const SHttpDownloadResult& result)
{
    if (!result.bSuccess)
        return;

    CWebCore* pWebCore = static_cast<CWebCore*>(result.pObj);
    if (!pWebCore->m_pXmlConfig)
        return;

    if (!pWebCore->MakeSureXMLNodesExist())
        return;

    CXMLNode*            pRootNode = pWebCore->m_pXmlConfig->GetRootNode();
    std::vector<SString> whitelist;
    SString              strData = result.pData;
    strData.Split(";", whitelist);
    CXMLNode* pListNode = pRootNode->FindSubNode("globalwhitelist");
    if (!pListNode)
        return;
    pListNode->DeleteAllSubNodes();

    for (std::vector<SString>::const_iterator iter = whitelist.begin(); iter != whitelist.end(); ++iter)
    {
        CXMLNode* pNode = pListNode->CreateSubNode("url");
        pNode->SetTagContent(*iter);
    }

    // Set whitelist revision
    CXMLNode* pNode = pRootNode->FindSubNode("whitelistrev");
    if (!pNode)
        return;
    pNode->SetTagContent(pWebCore->m_iWhitelistRevision);

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write();

    pWebCore->LoadListsFromXML(true, false, false);

#ifdef MTA_DEBUG
    OutputDebugLine("Updated whitelist!");
#endif
}

void CWebCore::StaticFetchBlacklistFinished(const SHttpDownloadResult& result)
{
    if (!result.bSuccess)
        return;

    CWebCore* pWebCore = static_cast<CWebCore*>(result.pObj);
    if (!pWebCore->m_pXmlConfig)
        return;

    if (!pWebCore->MakeSureXMLNodesExist())
        return;

    CXMLNode*            pRootNode = pWebCore->m_pXmlConfig->GetRootNode();
    std::vector<SString> blacklist;
    SString              strData = result.pData;
    strData.Split(";", blacklist);
    CXMLNode* pListNode = pRootNode->FindSubNode("globalblacklist");
    if (!pListNode)
        return;
    pListNode->DeleteAllSubNodes();

    for (std::vector<SString>::const_iterator iter = blacklist.begin(); iter != blacklist.end(); ++iter)
    {
        CXMLNode* pNode = pListNode->CreateSubNode("url");
        pNode->SetTagContent(*iter);
    }

    // Set blacklist revision
    CXMLNode* pNode = pRootNode->FindSubNode("blacklistrev");
    if (!pNode)
        return;
    pNode->SetTagContent(pWebCore->m_iBlacklistRevision);

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write();

    pWebCore->LoadListsFromXML(false, true, false);

#ifdef MTA_DEBUG
    OutputDebugLine("Updated browser blacklist!");
#endif
}

bool CWebCore::GetGPUEnabled() const noexcept
{
    return m_bGPUEnabled;
}

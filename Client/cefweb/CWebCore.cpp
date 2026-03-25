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
#include "CWebAppAuth.h"  // For GenerateAuthCode()
#include <algorithm>
#include <ranges>
#include <filesystem>
#include <cstdlib>

// #define CEF_ENABLE_SANDBOX
#ifdef CEF_ENABLE_SANDBOX
    #pragma comment(lib, "cef_sandbox.lib")
#endif

CWebCore::EventEntry::EventEntry(const std::function<void()>& callback_, CWebView* pWebView_) : callback(callback_), pWebView(pWebView_)
{
}

#ifdef MTA_DEBUG
CWebCore::EventEntry::EventEntry(const std::function<void()>& callback_, CWebView* pWebView_, const SString& name_)
    : callback(callback_), pWebView(pWebView_), name(name_)
{
}
#endif

CWebCore::TaskEntry::TaskEntry(std::function<void(bool)> callback, CWebView* webView) : task(callback), webView(webView)
{
}

CWebCore::CWebCore()
{
    m_pRequestsGUI = nullptr;
    m_bTestmodeEnabled = false;
    m_pXmlConfig = nullptr;
    m_pFocusedWebView = nullptr;
    m_bGPUEnabled = false;
    m_bInitialised = false;
    m_iWhitelistRevision = 0;
    m_iBlacklistRevision = 0;

    // Initialize auth code BEFORE CefInitialize (ensures webCore->m_AuthCode populated early)
    m_AuthCode = WebAppAuth::GenerateAuthCode();

    MakeSureXMLNodesExist();
    InitialiseWhiteAndBlacklist();

    // Update dynamic lists from QA server
    UpdateListsFromMaster();
}

CWebCore::~CWebCore()
{
    std::ranges::for_each(m_WebViews,
                          [](const auto& pWebView)
                          {
                              if (pWebView) [[likely]]
                                  pWebView->CloseBrowser();
                          });
    m_WebViews.clear();
    CefClearSchemeHandlerFactories();

    // Don't call CefShutdown() here to avoid freeze.
    // TerminateProcess (during quit) is called before CCore destruction anyways.
    delete m_pRequestsGUI;
    delete m_pXmlConfig;
}

bool CWebCore::Initialise(bool gpuEnabled)
{
    // CefInitialize() can only be called once per process lifetime
    // Do not call this function again or recreate CWebCore if initialization fails
    // Repeated calls cause "Timeout of new browser info response for frame" errors

    m_bGPUEnabled = gpuEnabled;

    // Get MTA base directory
    SString strBaseDir = SharedUtil::GetMTAProcessBaseDir();

    if (strBaseDir.empty())
    {
        g_pCore->GetConsole()->Printf("CEF initialization skipped - Unable to determine MTA base directory");
        AddReportLog(8000, "CEF initialization skipped - Unable to determine MTA base directory");
        m_bInitialised = false;
        return false;
    }

    SString strMTADir = PathJoin(strBaseDir, "MTA");

#ifndef MTA_DEBUG
    SString strLauncherPath = PathJoin(strMTADir, "CEF", "CEFLauncher.exe");
#else
    SString strLauncherPath = PathJoin(strMTADir, "CEF", "CEFLauncher_d.exe");
#endif

    // Set DLL directory for CEFLauncher subprocess to locate required libraries
    SString strCEFDir = PathJoin(strMTADir, "CEF");
#ifdef _WIN32
    SetDllDirectoryW(FromUTF8(strCEFDir));
#else
    // On Wine/Proton: Use environment variable for library search
    const char* existingPath = std::getenv("LD_LIBRARY_PATH");
    SString     newPath = strCEFDir;
    if (existingPath)
    {
        newPath = SString("%s:%s", strCEFDir.c_str(), existingPath);
    }
    // Note: setenv is not available in MSVC, but _putenv is.
    // However, since we are compiling for Windows (running on Wine), we use Windows APIs.
    // Wine maps Windows environment variables.
    // But LD_LIBRARY_PATH is a Linux variable.
    // If we are in Wine, we might want to set PATH instead or as well.
    // SetDllDirectoryW handles the Windows loader.

    // Log for debugging
    if (std::getenv("WINE") || std::getenv("WINEPREFIX"))
    {
        g_pCore->GetConsole()->Printf("DEBUG: CEF library path set via SetDllDirectoryW: %s", strCEFDir.c_str());
    }
#endif

    // Read GTA path from registry to pass to CEF subprocess
    int           iRegistryResult = 0;
    const SString strGTAPath = GetCommonRegistryValue("", "GTA:SA Path", &iRegistryResult);

    // Check if process is running with elevated privileges
    // CEF subprocesses may have communication issues when running elevated
    const bool bIsElevated = []() -> bool
    {
        // Check for Wine environment
        if (std::getenv("WINE") || std::getenv("WINEPREFIX"))
        {
            // In Wine, privilege escalation works differently
            // Assume not elevated for browser feature purposes
            return false;
        }

        HANDLE hToken = nullptr;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
            return false;

        // RAII wrapper for token handle
        const std::unique_ptr<void, decltype(&CloseHandle)> tokenGuard(hToken, &CloseHandle);

        TOKEN_ELEVATION elevation{};
        DWORD           dwSize = sizeof(elevation);
        if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
            return false;

        return elevation.TokenIsElevated != 0;
    }();

    if (bIsElevated && !std::getenv("WINE"))
    {
        g_pCore->GetConsole()->Printf("WARNING: Running as Administrator - browser features may be limited");
    }

    // Verify CEFLauncher can run in current environment
    auto CanExecuteCEFLauncher = []() -> bool
    {
#ifdef _WIN32
        // On Windows, we know it works
        if (!std::getenv("WINE") && !std::getenv("WINEPREFIX") && !std::getenv("PROTON_VERSION"))
            return true;
#endif

        // Check if Wine can execute the launcher
        // This is a basic check - if we are in Wine, we assume it works unless proven otherwise
        // But we can log if we are in a mixed environment
        return true;
    };

    if (!FileExists(strLauncherPath))
    {
        g_pCore->GetConsole()->Printf("CEF initialization skipped - CEFLauncher not found: %s", *strLauncherPath);
        AddReportLog(8001, SString("CEF initialization skipped - CEFLauncher not found: %s", *strLauncherPath));
        m_bInitialised = false;
        return false;
    }

    if (!CanExecuteCEFLauncher())
    {
        g_pCore->GetConsole()->Printf("CEF initialization skipped - Wine/Proton not available");
        AddReportLog(8026, "CEF initialization skipped - Wine/Proton not available or misconfigured");
        m_bInitialised = false;
        return false;
    }

    // Ensure cache directory can be created
    const SString strCachePath = PathJoin(strMTADir, "CEF", "cache");
    MakeSureDirExists(strCachePath);

    // Verify locales directory exists
    const SString strLocalesPath = PathJoin(strMTADir, "CEF", "locales");
    if (!DirectoryExists(strLocalesPath))
    {
        g_pCore->GetConsole()->Printf("CEF initialization skipped - locales directory not found: %s", *strLocalesPath);
        AddReportLog(8002, SString("CEF initialization skipped - locales directory not found: %s", *strLocalesPath));
        m_bInitialised = false;
        return false;
    }

    // Use std::filesystem for CWD management with RAII scope guard
    namespace fs = std::filesystem;
    std::error_code ec;

    // Save current working directory
    const fs::path savedCwd = fs::current_path(ec);
    if (ec)
    {
        AddReportLog(8025, SString("Failed to get current directory: %s", ec.message().c_str()));
        m_bInitialised = false;
        return false;
    }

    // RAII scope guard to restore CWD, even if CefInitialize throws or returns early
    struct CwdGuard
    {
        fs::path savedPath;
        explicit CwdGuard(fs::path path) : savedPath(std::move(path)) {}
        ~CwdGuard()
        {
            std::error_code restoreEc;
            fs::current_path(savedPath, restoreEc);
        }
    } cwdGuard(savedCwd);

    // Temporarily change CWD to MTA directory for CefInitialize
    // CEFLauncher.exe requires this to locate CEF dependencies
    fs::current_path(fs::path(FromUTF8(strMTADir)), ec);
    if (ec)
    {
        AddReportLog(8026, SString("Failed to change CWD to MTA dir: %s", ec.message().c_str()));
        m_bInitialised = false;
        return false;
    }

    CefMainArgs mainArgs;
    void*       sandboxInfo = nullptr;

    CefRefPtr<CWebApp> app(new CWebApp);

#ifdef CEF_ENABLE_SANDBOX
    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    CefSettings settings;
    CefString(&settings.browser_subprocess_path).FromWString(FromUTF8(strLauncherPath));
#ifndef CEF_ENABLE_SANDBOX
    settings.no_sandbox = true;
#endif

    CefString(&settings.browser_subprocess_path).FromWString(FromUTF8(strLauncherPath));
    CefString(&settings.cache_path).FromWString(FromUTF8(strCachePath));
    CefString(&settings.locales_dir_path).FromWString(FromUTF8(strLocalesPath));
    CefString(&settings.log_file).FromWString(FromUTF8(PathJoin(strMTADir, "CEF", "cefdebug.txt")));
#ifdef MTA_DEBUG
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_INFO;
#else
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_WARNING;
#endif

    settings.multi_threaded_message_loop = true;
    settings.windowless_rendering_enabled = true;

    // Wrap CefInitialize in try-catch for exception safety
    try
    {
        m_bInitialised = CefInitialize(mainArgs, settings, app, sandboxInfo);
    }
    catch (...)
    {
        g_pCore->GetConsole()->Printf("CefInitialize threw exception - CEF features will be disabled");
        AddReportLog(8003, "CefInitialize threw exception - CEF features will be disabled");
        m_bInitialised = false;
    }

    // CWD will be restored by cwdGuard destructor when this function returns

    if (m_bInitialised)
    {
        // Register custom scheme handler factory only if initialization succeeded
        CefRegisterSchemeHandlerFactory("http", "mta", app);
    }
    else
    {
        // Log initialization failure
        g_pCore->GetConsole()->Printf("CefInitialize failed - CEF features will be disabled");
        AddReportLog(8004, "CefInitialize failed - CEF features will be disabled");
    }

    return m_bInitialised;
}

CWebViewInterface* CWebCore::CreateWebView(unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem,
                                           bool bTransparent)
{
    // Skip browser creation if CEF initialization failed
    if (!m_bInitialised)
        return nullptr;

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
        // Mark as being destroyed to prevent new events/tasks
        pWebView->SetBeingDestroyed(true);

        // Ensure that no attached events or tasks are in the queue
        RemoveWebViewEvents(pWebView.get());
        RemoveWebViewTasks(pWebView.get());

        // Remove from list before closing to break reference cycles early
        m_WebViews.remove(pWebView);

        // CloseBrowser will eventually trigger OnBeforeClose which clears m_pWebView
        // This breaks the circular reference: CWebView -> CefBrowser -> CWebView
        pWebView->CloseBrowser();

        // Note: Do not call Release() - let CefRefPtr manage the lifecycle
        // The circular reference is broken via OnBeforeClose setting m_pWebView = nullptr
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

    for (const auto& pWebView : m_WebViews)
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

    std::scoped_lock lock(m_EventQueueMutex);

    // Prevent unbounded queue growth - drop oldest events if queue is too large
    if (m_EventQueue.size() >= MAX_EVENT_QUEUE_SIZE)
    {
        // Log warning even in release builds as this indicates a serious issue
        g_pCore->GetConsole()->Printf("WARNING: Browser event queue size limit reached (%d), dropping oldest events", MAX_EVENT_QUEUE_SIZE);

        // Remove oldest 10% of events to make room
        auto removeCount = static_cast<size_t>(MAX_EVENT_QUEUE_SIZE / 10);
        for (auto i = size_t{0}; i < removeCount && !m_EventQueue.empty(); ++i)
            m_EventQueue.pop_front();
    }

#ifndef MTA_DEBUG
    m_EventQueue.push_back(EventEntry(event, pWebView));
#else
    m_EventQueue.push_back(EventEntry(event, pWebView, name));
#endif
}

void CWebCore::RemoveWebViewEvents(CWebView* pWebView)
{
    std::scoped_lock lock(m_EventQueueMutex);

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
        std::scoped_lock lock(m_EventQueueMutex);
        std::swap(eventQueue, m_EventQueue);
    }

    for (auto& event : eventQueue)
    {
        // Skip event if the associated WebView is being destroyed
        if (event.pWebView && event.pWebView->IsBeingDestroyed())
            continue;

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

        // Prevent unbounded queue growth - abort new task if queue is too large
        if (m_TaskQueue.size() >= MAX_TASK_QUEUE_SIZE) [[unlikely]]
        {
#ifdef MTA_DEBUG
            static constexpr auto WARNING_MSG = "Warning: Task queue size limit reached (%d), aborting new task";
            g_pCore->GetConsole()->Printf(WARNING_MSG, MAX_TASK_QUEUE_SIZE);
#endif
            // Abort the new task immediately to prevent deadlock
            // Don't add it to the queue
            task(true);
            return;
        }

        m_TaskQueue.emplace_back(TaskEntry{task, webView});
        result = m_TaskQueue.back().task.get_future();
    }

    result.get();
}

void CWebCore::RemoveWebViewTasks(CWebView* webView)
{
    std::scoped_lock lock(m_TaskQueueMutex);

    std::erase_if(m_TaskQueue,
                  [webView](TaskEntry& entry)
                  {
                      if (entry.webView == webView)
                      {
                          entry.task(true);
                          return true;
                      }
                      return false;
                  });
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
        // Abort task if the associated WebView is being destroyed
        if (entry.webView && entry.webView->IsBeingDestroyed())
        {
            entry.task(true);
            continue;
        }

        entry.task(false);
    }
}

eURLState CWebCore::GetDomainState(const SString& strURL, bool bOutputDebug)
{
    std::scoped_lock lock(m_FilterMutex);

    // Initialize wildcard whitelist (be careful with modifying) | Todo: Think about the following
    static constexpr const char* wildcardWhitelist[] = {"*.googlevideo.com", "*.google.com",  "*.youtube.com",    "*.ytimg.com",
                                                        "*.vimeocdn.com",    "*.gstatic.com", "*.googleapis.com", "*.ggpht.com"};

    for (const auto& pattern : wildcardWhitelist)
    {
        if (WildcardMatch(pattern, strURL))
            return eURLState::WEBPAGE_ALLOWED;
    }

    if (auto iter = m_Whitelist.find(strURL); iter != m_Whitelist.end())
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
    std::scoped_lock lock(m_FilterMutex);

    // Prevent unbounded whitelist growth - remove old REQUEST entries if limit reached
    if (m_Whitelist.size() >= 50000)
    {
        // Remove WEBFILTER_REQUEST entries (temporary session entries)
        for (auto iter = m_Whitelist.begin(); iter != m_Whitelist.end();)
        {
            if (iter->second.second == eWebFilterType::WEBFILTER_REQUEST)
                m_Whitelist.erase(iter++);
            else
                ++iter;
        }
    }

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType>(true, filterType);
}

void CWebCore::AddBlockedPage(const SString& strURL, eWebFilterType filterType)
{
    std::scoped_lock lock(m_FilterMutex);

    // Prevent unbounded whitelist growth - remove old REQUEST entries if limit reached
    if (m_Whitelist.size() >= 50000)
    {
        // Remove WEBFILTER_REQUEST entries (temporary session entries)
        for (auto iter = m_Whitelist.begin(); iter != m_Whitelist.end();)
        {
            if (iter->second.second == eWebFilterType::WEBFILTER_REQUEST)
                m_Whitelist.erase(iter++);
            else
                ++iter;
        }
    }

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType>(false, filterType);
}

void CWebCore::RequestPages(const std::vector<SString>& pages, WebRequestCallback* pCallback)
{
    if (m_PendingRequests.size() >= MAX_PENDING_REQUESTS)
    {
        if (pCallback)
            (*pCallback)(false, std::unordered_set<SString>(pages.begin(), pages.end()));
        return;
    }

    // Add to pending pages queue
    bool bNewItem = false;
    for (const auto& page : pages)
    {
        eURLState status = GetDomainState(page);
        if (status == eURLState::WEBPAGE_ALLOWED || status == eURLState::WEBPAGE_DISALLOWED)
            continue;

        const auto [iter, inserted] = m_PendingRequests.insert(page);
        if (inserted)
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
    auto pCurrentMod = g_pCore->GetModManager()->GetClient();
    if (!pCurrentMod)
        return std::unordered_set<SString>();

    pCurrentMod->WebsiteRequestResultHandler(m_PendingRequests);

    if (bRemember)
    {
        std::vector<std::pair<SString, bool>> result;  // Contains only allowed entries
        GetFilterEntriesByType(result, eWebFilterType::WEBFILTER_USER, eWebFilterState::WEBFILTER_ALLOWED);
        std::vector<SString> customWhitelist;
        for (std::vector<std::pair<SString, bool>>::iterator iter = result.begin(); iter != result.end(); ++iter)
            customWhitelist.push_back(iter->first);

        WriteCustomList("customwhitelist", customWhitelist, false);
    }

    auto allowedRequests(std::move(m_PendingRequests));
    m_PendingRequests.clear();  // MSVC's move constructor already clears the list which isn't specified by the C++ standard though

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
    ClearTextures();
}

void CWebCore::OnPostScreenshot()
{
    // Re-draw textures
    for (auto& pWebView : m_WebViews)
    {
        pWebView->GetCefBrowser()->GetHost()->Invalidate(CefBrowserHost::PaintElementType::PET_VIEW);
    }
}

void CWebCore::OnFPSLimitChange(std::uint16_t fps)
{
    dassert(g_pCore->GetNetwork() != nullptr);  // Ensure network module is loaded
    for (auto& webView : m_WebViews)
    {
        if (auto browser = webView->GetCefBrowser(); browser) [[likely]]
            browser->GetHost()->SetWindowlessFrameRate(fps);
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
        SHORT scan_res = ::VkKeyScanExW(static_cast<WCHAR>(wParam), current_layout);
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

CWebViewInterface* CWebCore::GetFocusedWebView()
{
    return m_pFocusedWebView;
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
            else if (state == eWebFilterState::WEBFILTER_DISALLOWED && iter->second.first == false)
                outEntries.push_back(std::pair<SString, bool>(iter->first, iter->second.first));
        }
    }
}

void CWebCore::StaticFetchRevisionFinished(const SHttpDownloadResult& result)
{
    CWebCore* pWebCore = static_cast<CWebCore*>(result.pObj);
    if (!pWebCore) [[unlikely]]
        return;

    if (result.bSuccess)
    {
        if (result.dataSize > 1024 * 1024) [[unlikely]]
            return;

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
    if (!pWebCore) [[unlikely]]
        return;

    if (!pWebCore->m_pXmlConfig)
        return;

    if (!pWebCore->MakeSureXMLNodesExist())
        return;

    if (result.dataSize > 5 * 1024 * 1024) [[unlikely]]
    {
        return;
    }

    CXMLNode*            pRootNode = pWebCore->m_pXmlConfig->GetRootNode();
    std::vector<SString> whitelist;
    SString              strData = result.pData;
    strData.Split(";", whitelist);

    if (whitelist.size() > 50000) [[unlikely]]
    {
        whitelist.resize(50000);
    }

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
    if (!pWebCore) [[unlikely]]
        return;

    if (!pWebCore->m_pXmlConfig)
        return;

    if (!pWebCore->MakeSureXMLNodesExist())
        return;

    if (result.dataSize > 5 * 1024 * 1024) [[unlikely]]
    {
        return;
    }

    CXMLNode*            pRootNode = pWebCore->m_pXmlConfig->GetRootNode();
    std::vector<SString> blacklist;
    SString              strData = result.pData;
    strData.Split(";", blacklist);

    if (blacklist.size() > 50000) [[unlikely]]
    {
        blacklist.resize(50000);
    }

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

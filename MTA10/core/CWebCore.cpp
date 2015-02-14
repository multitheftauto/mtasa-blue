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
#include <Audiopolicy.h>
#include <cef3/include/cef_app.h>
#include <cef3/include/cef_browser.h>
#include <cef3/include/cef_sandbox_win.h>
#include <cef3/include/cef_url.h>
#include <cef3/include/wrapper/cef_stream_resource_handler.h>
#include "WebBrowserHelpers.h"

#ifdef CEF_ENABLE_SANDBOX
    #pragma comment(lib, "cef_sandbox.lib")
#endif

CWebCore::CWebCore ()
{
    m_pRequestsGUI = nullptr;
    m_bTestmodeEnabled = false;
    m_pAudioSessionManager = nullptr;
    m_pXmlConfig = nullptr;
    m_pFocusedWebView = nullptr;

    MakeSureXMLNodesExist ();
    InitialiseWhiteAndBlacklist ();

    // Update dynamic lists from QA server
    UpdateListsFromMaster ();

    // Get AudioVolume COM interface if the Audio Core API is available
    if ( GetApplicationSetting("os-version") >= "6.2" )
    {
        InitialiseCoreAudio ();
    }
}

CWebCore::~CWebCore ()
{
    // Shutdown CEF
    CefShutdown();

    if ( m_pAudioSessionManager )
        m_pAudioSessionManager->Release ();

    delete m_pRequestsGUI;
    delete m_pXmlConfig;
}

bool CWebCore::Initialise ()
{
    CefMainArgs mainArgs;
    void* sandboxInfo = nullptr;
    CefRefPtr<CCefApp> app = new CCefApp;

#if CEF_ENABLE_SANDBOX
    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    if ( CefExecuteProcess ( mainArgs, app, sandboxInfo ) >= 0 )
        return false;

    CefSettings settings;
#if !CEF_ENABLE_SANDBOX
    settings.no_sandbox = true;
#endif

    // Specifiy sub process executable path
#ifndef MTA_DEBUG
    CefString ( &settings.browser_subprocess_path ).FromASCII( CalcMTASAPath ( "MTA\\CEF\\CEFLauncher.exe" ) );
#else
    CefString ( &settings.browser_subprocess_path ).FromASCII( CalcMTASAPath ( "MTA\\CEF\\CEFLauncher_d.exe" ) );
#endif
    //CefString ( &settings.user_agent ).FromASCII ( "Multi Theft Auto: San Andreas Client; Chromium" );
    CefString ( &settings.resources_dir_path ).FromASCII ( CalcMTASAPath( "MTA\\CEF") );
    CefString ( &settings.locales_dir_path ).FromASCII( CalcMTASAPath( "MTA\\CEF\\locales" ) );
    CefString ( &settings.log_file ).FromASCII ( CalcMTASAPath ( "MTA\\CEF\\cefdebug.txt" ) );
#ifdef MTA_DEBUG
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_INFO;
#else
    settings.log_severity = cef_log_severity_t::LOGSEVERITY_WARNING;
#endif

    settings.multi_threaded_message_loop = true;
    settings.windowless_rendering_enabled = true;

    bool state = CefInitialize ( mainArgs, settings, app, sandboxInfo );
    
    // Register custom scheme handler factory
    CefRegisterSchemeHandlerFactory ( "mtalocal", "", app );

    return state;
}

CWebViewInterface* CWebCore::CreateWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent )
{
    // Create our webview implementation
    CefRefPtr<CWebView> pWebView = new CWebView ( uiWidth, uiHeight, bIsLocal, pWebBrowserRenderItem, bTransparent );
    m_WebViews.push_back ( pWebView );

    return static_cast < CWebViewInterface* > ( pWebView.get () );
}

void CWebCore::DestroyWebView ( CWebViewInterface* pWebViewInterface )
{
    CefRefPtr<CWebView> pWebView = dynamic_cast<CWebView*> ( pWebViewInterface );
    if ( pWebView )
    {
        // Ensure that no attached events are in the queue
        RemoveWebViewEvents ( pWebView );
        pWebView->NotifyPaint ();

        m_WebViews.remove ( pWebView );
        //pWebView->Release(); // Do not release since other references get corrupted then
        pWebView->CloseBrowser ();
    }
}

void CWebCore::DoPulse ()
{
    // Check for queued whitelist/blacklist downloads
    g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->ProcessQueuedFiles ();

    // Execute queued events on the main thread (Lua calls must be executed on the main thread)
    DoEventQueuePulse ();
}

CWebView* CWebCore::FindWebView ( CefRefPtr<CefBrowser> browser )
{
    for ( auto pWebView : m_WebViews )
    {
        // CefBrowser objects are not unique
        if ( pWebView->GetCefBrowser ()->GetIdentifier () == browser->GetIdentifier () )
            return pWebView.get ();
    }
    return nullptr;
}

void CWebCore::AddEventToEventQueue ( std::function<void(void)> event, CWebView* pWebView, const SString& name )
{
#ifndef MTA_DEBUG
    UNREFERENCED_PARAMETER(name);
#endif
    if ( pWebView->IsBeingDestroyed () )
        return;

    std::lock_guard<std::mutex> lock ( m_EventQueueMutex );
    
#ifndef MTA_DEBUG
    m_EventQueue.push_back ( EventEntry ( event, pWebView ) );
#else
    m_EventQueue.push_back ( EventEntry ( event, pWebView, name ) );
#endif
}

void CWebCore::RemoveWebViewEvents ( CWebView* pWebView )
{
    std::lock_guard<std::mutex> lock ( m_EventQueueMutex );

    for ( auto iter = m_EventQueue.begin (); iter != m_EventQueue.end (); )
    {
        // Increment iterator before we remove the element from the list (to guarantee iterator validity)
        auto tempIterator = iter++;
        if ( tempIterator->pWebView == pWebView )
            m_EventQueue.erase ( tempIterator );
    }
}

void CWebCore::DoEventQueuePulse ()
{
    std::lock_guard<std::mutex> lock ( m_EventQueueMutex );

    for ( auto& event : m_EventQueue )
    {
        event.callback ();
    }

    // Clear message queue
    m_EventQueue.clear ();

    // Invoke paint method if necessary on the main thread
    for ( auto& view : m_WebViews )
    {
        view->NotifyPaint ();
    }
}

eURLState CWebCore::GetURLState ( const SString& strURL )
{
    std::lock_guard<std::recursive_mutex> lock ( m_FilterMutex );
    
    // Initialize wildcard whitelist (be careful with modifying) | Todo: Think about the following
    static SString wildcardWhitelist[] = { "*.googlevideo.com", "*.google.com", "*.youtube.com", "*.ytimg.com", "*.vimeocdn.com" };

    for ( int i = 0; i < sizeof(wildcardWhitelist) / sizeof(SString); ++i )
    {
        if ( WildcardMatch ( wildcardWhitelist[i], strURL ) )
            return eURLState::WEBPAGE_ALLOWED;
    }

    google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.find ( strURL );
    if ( iter != m_Whitelist.end () )
    {
        if ( iter->second.first == true )
            return eURLState::WEBPAGE_ALLOWED;
        else
        {
            if ( m_bTestmodeEnabled ) g_pCore->DebugPrintfColor ( "[BROWSER] Blocked page: %s", 255, 0, 0, strURL.c_str() );
            return eURLState::WEBPAGE_DISALLOWED;
        }
    }

    if ( m_bTestmodeEnabled ) g_pCore->DebugPrintfColor ( "[BROWSER] Blocked page: %s", 255, 0, 0, strURL.c_str() );
    return eURLState::WEBPAGE_NOT_LISTED;
}

void CWebCore::ResetFilter ( bool bResetRequestsOnly )
{
    std::lock_guard<std::recursive_mutex> lock ( m_FilterMutex );

    // Clear old data
    m_PendingRequests.clear ();

    if ( !bResetRequestsOnly )
    {
        m_Whitelist.clear ();

        // Re-add "default" entries
        InitialiseWhiteAndBlacklist ();
    }
    else
    {
        // Erase all WEBFILTER_REQUEST entries
        google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.begin ();
        for ( ; iter != m_Whitelist.end (); )
        {
            if ( iter->second.second == eWebFilterType::WEBFILTER_REQUEST )
                m_Whitelist.erase ( iter++ );
            else
                ++iter;
        }
    }
}

void CWebCore::InitialiseWhiteAndBlacklist ( bool bAddHardcoded, bool bAddDynamic )
{
    if ( bAddDynamic )
    {
        // Hardcoded whitelist
        static SString whitelist[] = { 
            "google.com", "youtube.com", "www.youtube-nocookie.com", "vimeo.com", "player.vimeo.com", "code.jquery.com",
            "myvideo.com", "mtasa.com", "multitheftauto.com", "mtavc.com", "www.googleapis.com", "ajax.googleapis.com",
        };

        // Hardcoded blacklist
        static SString blacklist[] = { "nobrain.dk" };

        // Blacklist or whitelist URLs now
        for ( unsigned int i = 0; i < sizeof(whitelist) / sizeof(SString); ++i )
        {
            AddAllowedPage ( whitelist[i], eWebFilterType::WEBFILTER_HARDCODED );
        }
        for ( unsigned int i = 0; i < sizeof(blacklist) / sizeof(SString); ++i )
        {
            AddBlockedPage ( blacklist[i], eWebFilterType::WEBFILTER_HARDCODED );
        }
    }

    // Load dynamic and custom blacklist from XML config
    if ( bAddDynamic )
        LoadListsFromXML ( true, true, true );
}

void CWebCore::AddAllowedPage ( const SString& strURL, eWebFilterType filterType )
{
    std::lock_guard<std::recursive_mutex> lock ( m_FilterMutex );

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType> ( true, filterType );
}

void CWebCore::AddBlockedPage ( const SString& strURL, eWebFilterType filterType )
{
    std::lock_guard<std::recursive_mutex> lock ( m_FilterMutex );

    m_Whitelist[strURL] = std::pair<bool, eWebFilterType> ( false, filterType );
}

void CWebCore::RequestPages ( const std::vector<SString>& pages )
{
    // Add to pending pages queue
    bool bNewItem = false;
    for ( std::vector<SString>::const_iterator iter = pages.begin(); iter != pages.end(); ++iter )
    {
        eURLState status = GetURLState ( *iter );
        if ( status == eURLState::WEBPAGE_ALLOWED || status == eURLState::WEBPAGE_DISALLOWED )
            continue;

        m_PendingRequests.push_back ( *iter );
        bNewItem = true;
    }

    if ( bNewItem )
    {
        // Open CEGUI dialog
        if ( !m_pRequestsGUI )
            m_pRequestsGUI = new CWebsiteRequests;

        // Set pending requests memo content and show the window
        m_pRequestsGUI->SetPendingRequests ( m_PendingRequests );
        m_pRequestsGUI->Show ();
    }
}

void CWebCore::AllowPendingPages ( bool bRemember )
{
    for ( const auto& request : m_PendingRequests )
    {
        AddAllowedPage ( request, !bRemember ? eWebFilterType::WEBFILTER_REQUEST : eWebFilterType::WEBFILTER_USER );
    }

    // Trigger an event now
    CModManager::GetSingleton ().GetCurrentMod ()->WebsiteRequestResultHandler ( m_PendingRequests );
    m_PendingRequests.clear ();

    if ( bRemember )
    {
        std::vector<std::pair<SString, bool>> result; // Contains only allowed entries
        CCore::GetSingleton ().GetWebCore ()->GetFilterEntriesByType ( result, eWebFilterType::WEBFILTER_USER, eWebFilterState::WEBFILTER_ALLOWED );
        std::vector<SString> customWhitelist;
        for ( std::vector<std::pair<SString, bool>>::iterator iter = result.begin (); iter != result.end (); ++iter )
            customWhitelist.push_back ( iter->first );

        WriteCustomList ( "customwhitelist", customWhitelist, false );
    }
}

void CWebCore::DenyPendingPages ()
{
    m_PendingRequests.clear ();
}

bool CWebCore::CanLoadRemotePages ()
{
    bool bCanLoadRemotePages;
    CVARS_GET ( "browser_remote_websites", bCanLoadRemotePages );
    return bCanLoadRemotePages;
}

void CWebCore::OnPreScreenshot ()
{
    // Clear all textures
    g_pCore->GetWebCore ()->ClearTextures ();
}

void CWebCore::OnPostScreenshot ()
{
    // Re-draw textures
    for ( auto& pWebView : m_WebViews )
    {
        pWebView->GetCefBrowser ()->GetHost ()->Invalidate ( CefBrowserHost::PaintElementType::PET_VIEW );
    }
}

void CWebCore::ProcessInputMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( !m_pFocusedWebView || !( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR || uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) )
        return;

    CefKeyEvent keyEvent;
    keyEvent.windows_key_code = wParam;
    keyEvent.native_key_code = lParam;
    keyEvent.modifiers = GetCefKeyboardModifiers ( wParam, lParam );
    keyEvent.is_system_key = uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP;

    if ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_RAWKEYDOWN;
    else if ( uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
    else if ( uMsg == WM_CHAR || uMsg == WM_SYSCHAR )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;

    m_pFocusedWebView->InjectKeyboardEvent ( keyEvent );
}

void CWebCore::ClearTextures ()
{
    for ( auto& pWebBrowser : m_WebViews )
    {
        pWebBrowser->ClearTexture ();
    }
}

bool CWebCore::InitialiseCoreAudio()
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioSessionManager2 = __uuidof(IAudioSessionManager2);

    IMMDeviceEnumerator* pEnumerator;
    CoCreateInstance ( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator );
    if ( !pEnumerator )
        return false;

    IMMDevice* pMMDevice;
    pEnumerator->GetDefaultAudioEndpoint ( EDataFlow::eRender, ERole::eMultimedia, &pMMDevice );
    pEnumerator->Release ();
    if ( !pMMDevice )
        return false;

    IAudioSessionManager2* pAudioSessionManager;
    pMMDevice->Activate ( IID_IAudioSessionManager2, 0, NULL, (void**)&pAudioSessionManager );
    pMMDevice->Release ();
    if ( !pAudioSessionManager )
        return false;

    m_pAudioSessionManager = pAudioSessionManager;
    return true;
}

bool CWebCore::SetGlobalAudioVolume ( float fVolume )
{
    if ( fVolume < 0.0f || fVolume > 1.0f )
        return false;

    if ( GetApplicationSetting ( "os-version" ) < "6.2" || !m_pAudioSessionManager )
    {
        for ( auto& pWebView : m_WebViews )
        {
            pWebView->SetAudioVolume ( fVolume );
        }
    }
    else
    {
        IAudioSessionEnumerator* pSessionEnumerator;
        m_pAudioSessionManager->GetSessionEnumerator ( &pSessionEnumerator );
        if ( !pSessionEnumerator )
            return false;

        // Obtain the associated ISimpleAudioVolume interface
        int sessionCount;
        pSessionEnumerator->GetCount ( &sessionCount );
        for ( int i = 0; i < sessionCount; ++i )
        {
            IAudioSessionControl2* pSessionControl;
            pSessionEnumerator->GetSession ( i, (IAudioSessionControl**)&pSessionControl );
            PWSTR szIdentifier;
            pSessionControl->GetSessionIdentifier(&szIdentifier);

            if ( std::wstring(szIdentifier).find(L"CEFLauncher") != std::string::npos )
            {
                ISimpleAudioVolume* pSimpleAudioVolume;
                pSessionControl->QueryInterface ( &pSimpleAudioVolume );
                pSimpleAudioVolume->SetMasterVolume ( fVolume, NULL );
            }
            pSessionControl->Release ();
        }
        pSessionEnumerator->Release ();
    }
    return true;
}

bool CWebCore::UpdateListsFromMaster ()
{
    if ( !m_pXmlConfig )
        return false;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode ();
    if ( !pRootNode || !MakeSureXMLNodesExist () )
        return false;

    // Fetch white- and blacklist revision from config
    CXMLNode* pWhiteRevNode = pRootNode->FindSubNode ( "whitelistrev" );
    if ( !pWhiteRevNode || !pWhiteRevNode->GetTagContent ( m_iWhitelistRevision ) )
    {
       m_iWhitelistRevision = 0;
    }
    CXMLNode* pBlackRevNode = pRootNode->FindSubNode ( "blacklistrev" );
    if ( !pBlackRevNode || !pBlackRevNode->GetTagContent ( m_iBlacklistRevision ) )
    {
       m_iBlacklistRevision = 0;
    }

    // Get last update timestamp and compare with current time
    CXMLNode* pLastUpdateNode = pRootNode->FindSubNode ( "lastupdate" );
    if ( pLastUpdateNode )
    {
        SString lastUpdateTime = pLastUpdateNode->GetTagContent ();

        time_t currentTime;
        time ( &currentTime );

        if ( lastUpdateTime < SString ( "%d", (long long)currentTime - BROWSER_LIST_UPDATE_INTERVAL ) )
        {
        #ifdef MTA_DEBUG
            OutputDebugLine ( "Updating white- and blacklist..." );
        #endif
            g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->QueueFile ( SString("%s?type=getrev", BROWSER_UPDATE_URL),
                NULL, 0, NULL, 0, false, this, &CWebCore::StaticFetchRevisionProgress, false, 3 );

            pLastUpdateNode->SetTagContent ( SString ( "%d", (long long)currentTime ) );
            m_pXmlConfig->Write ();
        }
    }

    return true;
}

bool CWebCore::MakeSureXMLNodesExist ()
{
    // Check xml file
    if ( !m_pXmlConfig )
    {
        SString browserDataPath = CalcMTASAPath ( MTA_BROWSERDATA_PATH );
        bool exists = FileExists ( browserDataPath );

        m_pXmlConfig = g_pCore->GetXML ()->CreateXML ( browserDataPath );

        if ( !m_pXmlConfig || ( exists && !m_pXmlConfig->Parse () ) )
            return false;
    }

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode ();
    if ( !pRootNode )
    {
        pRootNode = m_pXmlConfig->CreateRootNode ( "browserdata" );
        if ( !pRootNode )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "lastupdate" ) )
    {
        CXMLNode* pNode = pRootNode->CreateSubNode ( "lastupdate" );
        if ( !pNode )
            return false;

        pNode->SetTagContent ( 0 );
    }

    if ( !pRootNode->FindSubNode ( "whitelistrev" ) )
    {
        if ( !pRootNode->CreateSubNode ( "whitelistrev" ) )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "blacklistrev" ) )
    {
        if ( !pRootNode->CreateSubNode ( "blacklistrev" ) )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "globalwhitelist" ) )
    {
        if ( !pRootNode->CreateSubNode ( "globalwhitelist" ) )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "globalblacklist" ) )
    {
        if ( !pRootNode->CreateSubNode ( "globalblacklist" ) )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "customblacklist" ) )
    {
        if ( !pRootNode->CreateSubNode ( "customblacklist" ) )
            return false;
    }

    if ( !pRootNode->FindSubNode ( "customwhitelist" ) )
    {
        if ( !pRootNode->CreateSubNode ( "customwhitelist" ) )
            return false;
    }

    return true;
}

void CWebCore::LoadListsFromXML ( bool bWhitelist, bool bBlacklist, bool bCustomLists )
{
    if ( !m_pXmlConfig )
        return;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode ();
    if ( !pRootNode )
        return;

    if ( bWhitelist )
    {
        CXMLNode* pWhiteSubNode = pRootNode->FindSubNode ( "globalwhitelist" );
        if ( pWhiteSubNode )
        {
            for ( std::list<CXMLNode*>::iterator iter = pWhiteSubNode->ChildrenBegin (); iter != pWhiteSubNode->ChildrenEnd (); ++iter )
            {
                AddAllowedPage ( (*iter)->GetTagContent (), eWebFilterType::WEBFILTER_DYNAMIC );
            }
        }
    }
    
    if ( bBlacklist )
    {
        CXMLNode* pBlackSubNode = pRootNode->FindSubNode ( "globalblacklist" );
        if ( pBlackSubNode )
        {
            for ( std::list<CXMLNode*>::iterator iter = pBlackSubNode->ChildrenBegin (); iter != pBlackSubNode->ChildrenEnd (); ++iter )
            {
                AddBlockedPage ( (*iter)->GetTagContent (), eWebFilterType::WEBFILTER_DYNAMIC );
            }
        }
    }

    if ( bCustomLists )
    {
        CXMLNode* pBlackSubNode = pRootNode->FindSubNode ( "customblacklist" );
        if ( pBlackSubNode )
        {
            for ( std::list<CXMLNode*>::iterator iter = pBlackSubNode->ChildrenBegin (); iter != pBlackSubNode->ChildrenEnd (); ++iter )
            {
                AddBlockedPage ( (*iter)->GetTagContent (), eWebFilterType::WEBFILTER_USER );
            }
        }
        CXMLNode* pWhiteSubNode = pRootNode->FindSubNode ( "customwhitelist" );
        if ( pWhiteSubNode )
        {
            for ( std::list<CXMLNode*>::iterator iter = pWhiteSubNode->ChildrenBegin (); iter != pWhiteSubNode->ChildrenEnd (); ++iter )
            {
                AddAllowedPage ( (*iter)->GetTagContent (), eWebFilterType::WEBFILTER_USER );
            }
        }
    }
}

void CWebCore::WriteCustomList ( const SString& strListName, const std::vector<SString>& customList, bool bReset )
{
    if ( !m_pXmlConfig || !MakeSureXMLNodesExist () )
        return;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode ();
    if ( !pRootNode )
        return;

    CXMLNode* pCustomListNode = pRootNode->FindSubNode ( strListName );
    if ( !pCustomListNode )
        return;

    pCustomListNode->DeleteAllSubNodes();
    for ( std::vector<SString>::const_iterator iter = customList.begin (); iter != customList.end (); ++iter )
    {
        CXMLNode* pNode = pCustomListNode->CreateSubNode ( "url" );
        if ( pNode )
            pNode->SetTagContent ( *iter );
    }

    // Write custom blacklist and reload from XML
    m_pXmlConfig->Write ();
    if ( bReset )
        ResetFilter ( false );
}

void CWebCore::GetFilterEntriesByType ( std::vector<std::pair<SString, bool>>& outEntries, eWebFilterType filterType, eWebFilterState state )
{
    std::lock_guard<std::recursive_mutex> lock ( m_FilterMutex );

    google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.begin ();
    for ( ; iter != m_Whitelist.end(); ++iter )
    {
        if ( iter->second.second == filterType )
        {
            if ( state == eWebFilterState::WEBFILTER_ALL )
                outEntries.push_back ( std::pair<SString, bool> ( iter->first, iter->second.first ) );
            else if ( state == eWebFilterState::WEBFILTER_ALLOWED && iter->second.first == true )
                outEntries.push_back ( std::pair<SString, bool> ( iter->first, iter->second.first ) );
            else
                outEntries.push_back ( std::pair<SString, bool> ( iter->first, iter->second.first ) );
        }
    }
}

bool CWebCore::StaticFetchRevisionProgress ( double dDownloadNow, double dDownloadTotal, char* pCompletedData, size_t completedLength, void *pObj, bool bComplete, int iError )
{
    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( bComplete )
    {
        SString strData = pCompletedData;
        SString strWhiteRevision, strBlackRevision;
        strData.Split ( ";", &strWhiteRevision, &strBlackRevision );

        if ( !strWhiteRevision.empty () && !strBlackRevision.empty () )
        {
            int iWhiteListRevision = atoi ( strWhiteRevision );
            if ( iWhiteListRevision > pWebCore->m_iWhitelistRevision )
            {
                g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->QueueFile ( SString("%s?type=fetchwhite", BROWSER_UPDATE_URL ),
                    NULL, 0, NULL, 0, false, pWebCore, &CWebCore::StaticFetchWhitelistProgress, false, 3 );

                pWebCore->m_iWhitelistRevision = iWhiteListRevision;
            }
            int iBlackListRevision = atoi ( strBlackRevision );
            if ( iBlackListRevision > pWebCore->m_iBlacklistRevision )
            {
                g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->QueueFile ( SString("%s?type=fetchblack", BROWSER_UPDATE_URL),
                    NULL, 0, NULL, 0, false, pWebCore, &CWebCore::StaticFetchBlacklistProgress, false, 3 );

                pWebCore->m_iBlacklistRevision = iBlackListRevision;
            }
        }
    }

    return true;
}

bool CWebCore::StaticFetchWhitelistProgress ( double dDownloadNow, double dDownloadTotal, char* pCompletedData, size_t completedLength, void *pObj, bool bComplete, int iError )
{
    if ( !bComplete )
        return false;

    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( !pWebCore->m_pXmlConfig )
        return false;

    if ( !pWebCore->MakeSureXMLNodesExist () )
        return false;

    CXMLNode* pRootNode = pWebCore->m_pXmlConfig->GetRootNode ();
    std::vector<SString> whitelist;
    SString strData = pCompletedData;
    strData.Split ( ";", whitelist );
    CXMLNode* pListNode = pRootNode->FindSubNode ( "globalwhitelist" );
    if ( !pListNode )
        return false;
    pListNode->DeleteAllSubNodes ();

    for ( std::vector<SString>::const_iterator iter = whitelist.begin (); iter != whitelist.end (); ++iter )
    {
        CXMLNode* pNode = pListNode->CreateSubNode ( "url" );
        pNode->SetTagContent ( *iter );
    }

    // Set whitelist revision
    CXMLNode* pNode = pRootNode->FindSubNode ( "whitelistrev" );
    if ( !pNode )
        return false;
    pNode->SetTagContent ( pWebCore->m_iWhitelistRevision );

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write ();

    pWebCore->LoadListsFromXML ( true, false, false );

#ifdef MTA_DEBUG
    OutputDebugLine ( "Updated whitelist!" );
#endif
    return true;
}

bool CWebCore::StaticFetchBlacklistProgress ( double dDownloadNow, double dDownloadTotal, char* pCompletedData, size_t completedLength, void *pObj, bool bComplete, int iError )
{
    if ( !bComplete )
        return false;

    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( !pWebCore->m_pXmlConfig )
        return false;

    if ( !pWebCore->MakeSureXMLNodesExist () )
        return false;

    CXMLNode* pRootNode = pWebCore->m_pXmlConfig->GetRootNode ();
    std::vector<SString> blacklist;
    SString strData = pCompletedData;
    strData.Split ( ";", blacklist );
    CXMLNode* pListNode = pRootNode->FindSubNode ( "globalblacklist" );
    if ( !pListNode )
        return false;
    pListNode->DeleteAllSubNodes ();

    for ( std::vector<SString>::const_iterator iter = blacklist.begin (); iter != blacklist.end (); ++iter )
    {
        CXMLNode* pNode = pListNode->CreateSubNode ( "url" );
        pNode->SetTagContent ( *iter );
    }

    // Set blacklist revision
    CXMLNode* pNode = pRootNode->FindSubNode ( "blacklistrev" );
    if ( !pNode )
        return false;
    pNode->SetTagContent ( pWebCore->m_iBlacklistRevision );

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write ();

    pWebCore->LoadListsFromXML ( false, true, false );

#ifdef MTA_DEBUG
    OutputDebugLine ( "Updated browser blacklist!" );
#endif
    return true;
}

//////////////////////////////////////////////////////////////
//                CefApp implementation                     //
//////////////////////////////////////////////////////////////
void CCefApp::OnRegisterCustomSchemes ( CefRefPtr < CefSchemeRegistrar > registrar )
{
    // Register custom MTA scheme (has to be called in all proceseses)
    registrar->AddCustomScheme ( "mtalocal", false, false, false );
}

CefRefPtr<CefResourceHandler> CCefApp::Create ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request )
{
    if (scheme_name != "mtalocal")
        return nullptr;

    CWebCore* pWebCore = static_cast<CWebCore*>(g_pCore->GetWebCore ());
    auto pWebView = pWebCore->FindWebView ( browser );
    if ( !pWebView || !pWebView->IsLocal () )
        return nullptr;

    CefURLParts urlParts;
    if ( !CefParseURL ( request->GetURL (), urlParts ) )
        return nullptr;

    // Get full path
    SString path = UTF16ToMbUTF8 ( urlParts.path.str ).substr ( 2 );
    if ( !pWebView->GetFullPathFromLocal ( path ) )
        return nullptr;

    // Get mime type from extension
    CefString mimeType;
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos)
        mimeType = CefGetMimeType ( path.substr ( pos + 1 ) );
    else
        mimeType = "text/plain";

    auto stream = CefStreamReader::CreateForFile ( path );
    if ( stream.get () )
        return new CefStreamResourceHandler ( mimeType, stream );

    return nullptr;
}

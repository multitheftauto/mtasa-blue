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
#include "CAjaxResourceHandler.h"
#include <cef3/include/cef_app.h>
#include <cef3/include/cef_browser.h>
#include <cef3/include/cef_sandbox_win.h>
#include <cef3/include/cef_parser.h>
#include <cef3/include/wrapper/cef_stream_resource_handler.h>
#include "WebBrowserHelpers.h"

#ifdef CEF_ENABLE_SANDBOX
    #pragma comment(lib, "cef_sandbox.lib")
#endif

CWebCore::CWebCore ()
{
    m_pRequestsGUI = nullptr;
    m_bTestmodeEnabled = false;
    m_pXmlConfig = nullptr;
    m_pFocusedWebView = nullptr;

    MakeSureXMLNodesExist ();
    InitialiseWhiteAndBlacklist ();

    // Update dynamic lists from QA server
    UpdateListsFromMaster ();
}

CWebCore::~CWebCore ()
{
    // Shutdown CEF
    CefShutdown();

    delete m_pRequestsGUI;
    delete m_pXmlConfig;
}

bool CWebCore::Initialise ()
{
    CefMainArgs mainArgs;
    void* sandboxInfo = nullptr;
    CefRefPtr<CCefApp> app { new CCefApp };

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
    CefString ( &settings.browser_subprocess_path ).FromWString( FromUTF8( CalcMTASAPath ( "MTA\\CEF\\CEFLauncher.exe" ) ) );
#else
    CefString ( &settings.browser_subprocess_path ).FromWString( FromUTF8( CalcMTASAPath ( "MTA\\CEF\\CEFLauncher_d.exe" ) ) );
#endif
    CefString ( &settings.resources_dir_path ).FromWString ( FromUTF8( CalcMTASAPath( "MTA\\CEF") ) );
    CefString ( &settings.locales_dir_path ).FromWString( FromUTF8( CalcMTASAPath( "MTA\\CEF\\locales" ) ) );
    CefString ( &settings.log_file ).FromWString ( FromUTF8( CalcMTASAPath ( "MTA\\CEF\\cefdebug.txt" ) ) );
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
    CefRegisterSchemeHandlerFactory ( "http", "mta", app );

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
    if ( pWebView && pWebView->IsBeingDestroyed () )
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

eURLState CWebCore::GetURLState ( const SString& strURL, bool bOutputDebug )
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
            if ( m_bTestmodeEnabled && bOutputDebug ) DebugOutputThreadsafe ( SString ( "[BROWSER] Blocked page: %s", strURL.c_str () ), 255, 0, 0 );
            return eURLState::WEBPAGE_DISALLOWED;
        }
    }

    if ( m_bTestmodeEnabled && bOutputDebug ) DebugOutputThreadsafe ( SString ( "[BROWSER] Blocked page: %s", strURL.c_str () ), 255, 0, 0 );
    return eURLState::WEBPAGE_NOT_LISTED;
}

SString CWebCore::GetDomainFromURL ( const SString& strURL )
{
    CefURLParts urlParts;
    if ( !CefParseURL ( strURL, urlParts ) || !urlParts.host.str )
        return "";

    return UTF16ToMbUTF8 ( urlParts.host.str );
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
        // Close requests GUI
        if ( m_pRequestsGUI )
        {
            delete m_pRequestsGUI;
            m_pRequestsGUI = nullptr;
        }

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

void CWebCore::RequestPages ( const std::vector<SString>& pages, WebRequestCallback* pCallback )
{
    // Add to pending pages queue
    bool bNewItem = false;
    for ( const auto& page : pages )
    {
        eURLState status = GetURLState ( page );
        if ( status == eURLState::WEBPAGE_ALLOWED || status == eURLState::WEBPAGE_DISALLOWED )
            continue;

        m_PendingRequests.push_back ( page );
        bNewItem = true;
    }

    if ( bNewItem )
    {
        // Open CEGUI dialog
        if ( !m_pRequestsGUI )
            m_pRequestsGUI = new CWebsiteRequests;

        // Set pending requests memo content and show the window
        m_pRequestsGUI->SetPendingRequests ( m_PendingRequests, pCallback );
        m_pRequestsGUI->Show ();
    }
    else
    {
        // Call callback immediately if nothing has changed (all entries are most likely already on the whitelist)
        // There is still the possibility that all websites are blacklisted; this is not the usual case tho, so ignore for now (TODO)
        if ( pCallback )
            (*pCallback)( true, pages );
    }
}

void CWebCore::AllowPendingPages ( bool bRemember )
{
    for ( const auto& request : m_PendingRequests )
    {
        AddAllowedPage ( request, !bRemember ? eWebFilterType::WEBFILTER_REQUEST : eWebFilterType::WEBFILTER_USER );
    }

    // Trigger an event now
    auto pCurrentMod = CModManager::GetSingleton ().GetCurrentMod ();
    if ( !pCurrentMod )
        return;

    pCurrentMod->WebsiteRequestResultHandler ( m_PendingRequests );
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

bool CWebCore::IsRequestsGUIVisible ()
{
    return m_pRequestsGUI && m_pRequestsGUI->IsVisible ();
}

void CWebCore::DebugOutputThreadsafe ( const SString& message, unsigned char R, unsigned char G, unsigned char B )
{
    AddEventToEventQueue( [message, R, G, B]() {
        g_pCore->DebugEchoColor ( message, R, G, B );
    }, nullptr, "DebugOutputThreadsafe" );
}

bool CWebCore::GetRemotePagesEnabled ()
{
    bool bCanLoadRemotePages;
    CVARS_GET ( "browser_remote_websites", bCanLoadRemotePages );
    return bCanLoadRemotePages;
}

bool CWebCore::GetRemoteJavascriptEnabled ()
{
    bool bIsRemoteJavascriptEnabled;
    CVARS_GET ( "browser_remote_javascript", bIsRemoteJavascriptEnabled );
    return bIsRemoteJavascriptEnabled;
}

bool CWebCore::GetPluginsEnabled ()
{
    bool bArePluginsEnabled;
    CVARS_GET ( "browser_plugins", bArePluginsEnabled );
    return bArePluginsEnabled;
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

bool CWebCore::SetGlobalAudioVolume ( float fVolume )
{
    if ( fVolume < 0.0f || fVolume > 1.0f )
        return false;

    for ( auto& pWebView : m_WebViews )
    {
        pWebView->SetAudioVolume ( fVolume );
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
                NULL, 0, NULL, 0, false, this, &CWebCore::StaticFetchRevisionFinished, false, 3 );

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

void CWebCore::StaticFetchRevisionFinished ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode )
{
    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( bSuccess )
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
                    NULL, 0, NULL, 0, false, pWebCore, &CWebCore::StaticFetchWhitelistFinished, false, 3 );

                pWebCore->m_iWhitelistRevision = iWhiteListRevision;
            }
            int iBlackListRevision = atoi ( strBlackRevision );
            if ( iBlackListRevision > pWebCore->m_iBlacklistRevision )
            {
                g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->QueueFile ( SString("%s?type=fetchblack", BROWSER_UPDATE_URL),
                    NULL, 0, NULL, 0, false, pWebCore, &CWebCore::StaticFetchBlacklistFinished, false, 3 );

                pWebCore->m_iBlacklistRevision = iBlackListRevision;
            }
        }
    }
}

void CWebCore::StaticFetchWhitelistFinished ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode )
{
    if ( !bSuccess )
        return;

    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( !pWebCore->m_pXmlConfig )
        return;

    if ( !pWebCore->MakeSureXMLNodesExist () )
        return;

    CXMLNode* pRootNode = pWebCore->m_pXmlConfig->GetRootNode ();
    std::vector<SString> whitelist;
    SString strData = pCompletedData;
    strData.Split ( ";", whitelist );
    CXMLNode* pListNode = pRootNode->FindSubNode ( "globalwhitelist" );
    if ( !pListNode )
        return;
    pListNode->DeleteAllSubNodes ();

    for ( std::vector<SString>::const_iterator iter = whitelist.begin (); iter != whitelist.end (); ++iter )
    {
        CXMLNode* pNode = pListNode->CreateSubNode ( "url" );
        pNode->SetTagContent ( *iter );
    }

    // Set whitelist revision
    CXMLNode* pNode = pRootNode->FindSubNode ( "whitelistrev" );
    if ( !pNode )
        return;
    pNode->SetTagContent ( pWebCore->m_iWhitelistRevision );

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write ();

    pWebCore->LoadListsFromXML ( true, false, false );

#ifdef MTA_DEBUG
    OutputDebugLine ( "Updated whitelist!" );
#endif
}

void CWebCore::StaticFetchBlacklistFinished ( char* pCompletedData, size_t completedLength, void *pObj, bool bSuccess, int iErrorCode )
{
    if ( !bSuccess )
        return;

    CWebCore* pWebCore = static_cast < CWebCore* > ( pObj );
    if ( !pWebCore->m_pXmlConfig )
        return;

    if ( !pWebCore->MakeSureXMLNodesExist () )
        return;

    CXMLNode* pRootNode = pWebCore->m_pXmlConfig->GetRootNode ();
    std::vector<SString> blacklist;
    SString strData = pCompletedData;
    strData.Split ( ";", blacklist );
    CXMLNode* pListNode = pRootNode->FindSubNode ( "globalblacklist" );
    if ( !pListNode )
        return;
    pListNode->DeleteAllSubNodes ();

    for ( std::vector<SString>::const_iterator iter = blacklist.begin (); iter != blacklist.end (); ++iter )
    {
        CXMLNode* pNode = pListNode->CreateSubNode ( "url" );
        pNode->SetTagContent ( *iter );
    }

    // Set blacklist revision
    CXMLNode* pNode = pRootNode->FindSubNode ( "blacklistrev" );
    if ( !pNode )
        return;
    pNode->SetTagContent ( pWebCore->m_iBlacklistRevision );

    // Write changes to the XML file
    pWebCore->m_pXmlConfig->Write ();

    pWebCore->LoadListsFromXML ( false, true, false );

#ifdef MTA_DEBUG
    OutputDebugLine ( "Updated browser blacklist!" );
#endif
}

//////////////////////////////////////////////////////////////
//                CefApp implementation                     //
//////////////////////////////////////////////////////////////
CefRefPtr<CefResourceHandler> CCefApp::HandleError ( const SString& strError, unsigned int uiError )
{
    auto stream = CefStreamReader::CreateForData ( (void*)strError.c_str(), strError.length());
    return new CefStreamResourceHandler ( 
            uiError, strError, "text/plain", CefResponse::HeaderMap(), stream);
}


void CCefApp::OnRegisterCustomSchemes ( CefRefPtr < CefSchemeRegistrar > registrar )
{
    // Register custom MTA scheme (has to be called in all proceseses)
    registrar->AddCustomScheme ( "mtalocal", false, false, false );
}

CefRefPtr<CefResourceHandler> CCefApp::Create ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request )
{
    // browser or frame are NULL if the request does not orginate from a browser window
    // This is for exmaple true for the application cache or CEFURLRequests
    // (http://www.html5rocks.com/en/tutorials/appcache/beginner/)
    if ( !browser || !frame )
        return nullptr;

    CWebCore* pWebCore = static_cast<CWebCore*> ( g_pCore->GetWebCore () );
    auto pWebView = pWebCore->FindWebView ( browser );
    if ( !pWebView || !pWebView->IsLocal () )
        return nullptr;

    CefURLParts urlParts;
    if ( !CefParseURL ( request->GetURL (), urlParts ) )
        return nullptr;

    if ( scheme_name == "mtalocal" ) // Backward compatibility
    {
        // Get full path
        SString path = UTF16ToMbUTF8 ( urlParts.path.str ).substr ( 2 );

        // Check if we're dealing with an external resource
        if ( path[0] == ':' )
        {
            size_t end = path.find_first_of ( '/' );
            if ( end != std::string::npos )
            {
                SString resourceName = path.substr ( 1, end-1 );
                SString resourcePath = path.substr ( end );

                // Call this function recursively and use the mta scheme instead
                request->SetURL ( "http://mta/local/" + resourceName + resourcePath );
                return Create ( browser, frame, "http", request );
            }
            return HandleError ("404 - Not found", 404);
        }
        
        // Redirect mtalocal://* to http://mta/local/*, call recursively
        request->SetURL ( "http://mta/local/" + path );
        return Create ( browser, frame, "http", request );
    }
    
    SString host = UTF16ToMbUTF8 ( urlParts.host.str );
    if ( scheme_name == "http" && host == "mta" )
    {
        // Scheme format: http://mta/resourceName/file.html or http://mta/local/file.html for the current resource

        // Get resource name and path
        SString path = UTF16ToMbUTF8 ( urlParts.path.str ).substr ( 1 ); // Remove slash at the front
        size_t slashPos = path.find ( '/' );
        if ( slashPos == std::string::npos )
            return HandleError ( "404 - Not found", 404 );

        SString resourceName = path.substr ( 0, slashPos );
        SString resourcePath = path.substr ( slashPos + 1 );

        if ( resourcePath.empty () )
            return HandleError ( "404 - Not found", 404 );

        // Get mime type from extension
        CefString mimeType;
        size_t pos = resourcePath.find_last_of ( '.' );
        if ( pos != std::string::npos )
            mimeType = CefGetMimeType ( resourcePath.substr ( pos + 1 ) );

        // Make sure we provide a mime type, even 
        // when we cannot deduct it from the file extension
        if ( mimeType.empty () )
            mimeType = "application/octet-stream";

        if ( pWebView->HasAjaxHandler ( resourcePath ) )
        {
            std::vector<SString> vecGet;
            std::vector<SString> vecPost;

            if ( urlParts.query.str != nullptr )
            {
                SString strGet = UTF16ToMbUTF8 ( urlParts.query.str );
                std::vector<SString> vecTmp;
                strGet.Split ( "&", vecTmp );

                SString key; SString value;
                for ( auto&& param : vecTmp )
                {
                    param.Split ( "=", &key, &value );
                    vecGet.push_back ( key );
                    vecGet.push_back ( value );
                }
            }

            CefPostData::ElementVector vecPostElements;
            auto postData = request->GetPostData ();
            if ( postData.get () )
            {
                request->GetPostData ()->GetElements ( vecPostElements );

                SString key; SString value;
                for ( auto&& post : vecPostElements )
                {
                    // Limit to 5MiB and allow byte data only
                    size_t bytesCount = post->GetBytesCount ();
                    if ( bytesCount > 5*1024*1024 || post->GetType () != CefPostDataElement::Type::PDE_TYPE_BYTES )
                        continue;

                    // Make string from buffer
                    std::unique_ptr<char[]> buffer { new char[bytesCount] };
                    post->GetBytes ( bytesCount, buffer.get () );
                    SStringX param ( buffer.get (), bytesCount );

                    // Parse POST data into vector
                    std::vector<SString> vecTmp;
                    param.Split ( "&", vecTmp );
                    
                    for ( auto&& param : vecTmp )
                    {
                        param.Split ( "=", &key, &value );
                        vecPost.push_back ( key );
                        vecPost.push_back ( value );
                    }
                }
            }

            auto handler = new CAjaxResourceHandler ( vecGet, vecPost, mimeType );
            pWebView->HandleAjaxRequest ( resourcePath, handler );
            return handler;
        }
        else
        {
            // Calculate MTA resource path
            if ( resourceName != "local" )
                path = ":" + resourceName + "/" + resourcePath;
            else
                path = resourcePath;

            // Calculate absolute path
            if ( !pWebView->GetFullPathFromLocal ( path ) )
                return HandleError ( "404 - Not found", 404 );

            // Verify local files
            if ( !pWebView->VerifyFile ( path ) )
                return HandleError ( "403 - Access Denied", 403 );
        
            // Finally, load the file stream
            auto stream = CefStreamReader::CreateForFile ( path );
            if ( stream.get () )
                return new CefStreamResourceHandler ( mimeType, stream );
            return HandleError ( "404 - Not found", 404 );
        }
    }

    // Return null if there is no matching scheme
    // This falls back to letting CEF handle the request
    return nullptr;
}

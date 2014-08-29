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

CWebCore::CWebCore ()
{
    //m_pWebCore = NULL;
    m_pRequestsGUI = NULL;
    m_bTestmodeEnabled = false;
    m_pAudioSessionManager = NULL;
    m_pXmlConfig = NULL;
    m_pFocusedWebView = NULL;

    Initialise ();
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
    void* sandboxInfo = NULL;

#if CEF_ENABLE_SANDBOX
    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    if ( CefExecuteProcess(mainArgs, NULL, NULL) >= 0 )
        return false;

    CefSettings settings;
#if !CEF_ENABLE_SANDBOX
    settings.no_sandbox = true;
#endif

    // Specifiy sub process executable path
    CefString ( &settings.browser_subprocess_path ).FromASCII( CalcMTASAPath ( CEF_SUBPROCESS_PATH ) );
    CefString ( &settings.user_agent ).FromASCII ( "Multi Theft Auto: San Andreas Client; Chromium" );
    CefString ( &settings.resources_dir_path ).FromASCII ( CalcMTASAPath( "mta") );
    
    // Todo: Implement multi-threading
    settings.multi_threaded_message_loop = false;

    return CefInitialize ( mainArgs, settings, NULL, sandboxInfo );
}

CWebViewInterface* CWebCore::CreateWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent )
{
    // Create our webview implementation
    CWebView* pWebView = new CWebView ( uiWidth, uiHeight, bIsLocal, pWebBrowserRenderItem, bTransparent );
    m_WebViews.push_back ( pWebView );

    return pWebView;
}

void CWebCore::DestroyWebView ( CWebViewInterface* pWebViewInterface )
{
    CWebView* pWebView = dynamic_cast<CWebView*> ( pWebViewInterface );
    if ( pWebView )
    {
        std::list<CWebView*>::iterator iter = std::find(m_WebViews.begin(), m_WebViews.end(), pWebView);
        if ( iter != m_WebViews.end () )
            m_WebViews.erase ( iter );

        delete pWebView;
    }
}

void CWebCore::DoPulse ()
{
    // Check for queued whitelist/blacklist downloads
    g_pCore->GetNetwork()->GetHTTPDownloadManager ( EDownloadModeType::WEBBROWSER_LISTS )->ProcessQueuedFiles ();

    // Update Awesomium rendering etc.
    CefDoMessageLoopWork();
}

eURLState CWebCore::GetURLState ( const SString& strURL )
{
    // Initialize wildcard whitelist (be careful with modifying) | Todo: Think about the following
    static SString wildcardWhitelist[] = { "*.googlevideo.com", "*.google.com", "*.youtube.com", "*.vimeocdn.com" };

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
            if ( m_bTestmodeEnabled ) g_pCore->DebugPrintf ( "[BROWSER] Blocked page: %s", strURL.c_str() );
            return eURLState::WEBPAGE_DISALLOWED;
        }
    }

    if ( m_bTestmodeEnabled ) g_pCore->DebugPrintf ( "[BROWSER] Blocked page: %s", strURL.c_str() );
    return eURLState::WEBPAGE_NOT_LISTED;
}

void CWebCore::ResetFilter ( bool bResetRequestsOnly )
{
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
            "google.com", "youtube.com", "www.youtube-nocookie.com", "s.ytimg.com", "vimeo.com", "player.vimeo.com",
            "myvideo.com", "reddit.com", "mtasa.com", "multitheftauto.com", "mtavc.com", "www.googleapis.com"
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
    m_Whitelist[strURL] = std::pair<bool, eWebFilterType> ( true, filterType );
}

void CWebCore::AddBlockedPage ( const SString& strURL, eWebFilterType filterType )
{
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

void CWebCore::AllowPendingPages ()
{
    for ( std::vector<SString>::iterator iter = m_PendingRequests.begin(); iter != m_PendingRequests.end(); ++iter )
    {
        AddAllowedPage ( *iter, eWebFilterType::WEBFILTER_REQUEST );
    }

    // Trigger an event now
    CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler ( m_PendingRequests );
    m_PendingRequests.clear ();
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

void CWebCore::ProcessInputMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( !m_pFocusedWebView || !( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR || uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) )
        return;

    CefKeyEvent keyEvent;
    keyEvent.windows_key_code = wParam;
    keyEvent.native_key_code = lParam;
    keyEvent.modifiers = GetCefKeyboardModifiers ( wParam, lParam );
    keyEvent.is_system_key = uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP;

    if ( uMsg == WM_KEYDOWN )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_RAWKEYDOWN;
    else if ( uMsg == WM_KEYUP )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
    else if ( uMsg == WM_CHAR )
        keyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;

    m_pFocusedWebView->InjectKeyboardEvent ( keyEvent );
}

bool isKeyDown ( WPARAM wParam )
{
    return (GetKeyState(wParam) & 0x8000) != 0;
}

int CWebCore::GetCefKeyboardModifiers ( WPARAM wParam, LPARAM lParam )
{
    int modifiers;
    if (isKeyDown(VK_SHIFT))
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (isKeyDown(VK_CONTROL))
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (isKeyDown(VK_MENU))
        modifiers |= EVENTFLAG_ALT_DOWN;

    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;

    switch (wParam) {
    case VK_RETURN:
        if ((lParam >> 16) & KF_EXTENDED)
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
        if (!((lParam >> 16) & KF_EXTENDED))
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_NUMLOCK:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_DIVIDE:
    case VK_MULTIPLY:
    case VK_SUBTRACT:
    case VK_ADD:
    case VK_DECIMAL:
    case VK_CLEAR:
        modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_SHIFT:
        if (isKeyDown(VK_LSHIFT))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RSHIFT))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_CONTROL:
        if (isKeyDown(VK_LCONTROL))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RCONTROL))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_MENU:
        if (isKeyDown(VK_LMENU))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RMENU))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_LWIN:
        modifiers |= EVENTFLAG_IS_LEFT;
        break;
    case VK_RWIN:
        modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    }
    return modifiers;
}

void CWebCore::ClearTextures ()
{
    for ( std::list<CWebView*>::iterator iter = m_WebViews.begin (); iter != m_WebViews.end (); ++iter )
    {
        (*iter)->ClearTexture ();
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
        for ( std::list<CWebView*>::iterator iter = m_WebViews.begin (); iter != m_WebViews.end (); ++iter )
        {
            (*iter)->SetAudioVolume ( fVolume );
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
    m_pXmlConfig = g_pCore->GetXML()->CreateXML ( CalcMTASAPath ( MTA_BROWSERDATA_PATH ) );
    if ( !m_pXmlConfig )
        return false;

    // Try parsing the xml file (create a new one if something went wrong)
    if ( !m_pXmlConfig->Parse () )
    {
        // Start with an entire new one
        m_pXmlConfig->Clear ();
        if ( !m_pXmlConfig->CreateRootNode ( "browserdata" ) )
            return false;
    }

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
        m_pXmlConfig = g_pCore->GetXML ()->CreateXML ( CalcMTASAPath ( MTA_BROWSERDATA_PATH ) );

        if ( !m_pXmlConfig )
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

    return true;
}

void CWebCore::LoadListsFromXML ( bool bWhitelist, bool bBlacklist, bool bCustomBlacklist )
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

    if ( bCustomBlacklist )
    {
        CXMLNode* pBlackSubNode = pRootNode->FindSubNode ( "customblacklist" );
        if ( pBlackSubNode )
        {
            for ( std::list<CXMLNode*>::iterator iter = pBlackSubNode->ChildrenBegin (); iter != pBlackSubNode->ChildrenEnd (); ++iter )
            {
                AddBlockedPage ( (*iter)->GetTagContent (), eWebFilterType::WEBFILTER_USER );
            }
        }
    }
}

void CWebCore::WriteCustomBlacklist ( const std::vector<SString>& customBlacklist )
{
    if ( !m_pXmlConfig || !MakeSureXMLNodesExist () )
        return;

    CXMLNode* pRootNode = m_pXmlConfig->GetRootNode ();
    if ( !pRootNode )
        return;

    CXMLNode* pCustomBlacklistNode = pRootNode->FindSubNode ( "customblacklist" );
    if ( !pCustomBlacklistNode )
        return;

    pCustomBlacklistNode->DeleteAllSubNodes();
    for ( std::vector<SString>::const_iterator iter = customBlacklist.begin (); iter != customBlacklist.end (); ++iter )
    {
        CXMLNode* pNode = pCustomBlacklistNode->CreateSubNode ( "url" );
        if ( pNode )
            pNode->SetTagContent ( *iter );
    }

    // Write custom blacklist and reload from XML
    m_pXmlConfig->Write ();
    ResetFilter ( true );
}

void CWebCore::GetFilterEntriesByType ( std::vector<std::pair<SString, bool>>& outEntries, eWebFilterType filterType )
{
    google::dense_hash_map<SString, WebFilterPair>::iterator iter = m_Whitelist.begin ();
    for ( ; iter != m_Whitelist.end(); ++iter )
    {
        if ( iter->second.second == filterType )
            outEntries.push_back ( std::pair<SString, bool> ( iter->first, iter->second.first ) );
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
    OutputDebugLine ( "Updated blacklist!" );
#endif
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnFilterNavigation     //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#aceecadf1ddd8e3fe42cd56bc74d6ec6c //
//                                                                //
////////////////////////////////////////////////////////////////////
/*bool CWebCore::OnFilterNavigation ( int origin_process_id, int origin_routing_id, const Awesomium::WebString& method, const Awesomium::WebURL& url, bool is_main_frame )
{
    std::map<int, CWebView*>::iterator iter = m_WebViewMap.find ( origin_process_id );
    assert ( iter != m_WebViewMap.end () );

    CWebView* pWebView = iter->second;
    if ( !pWebView )
        return true; // Block

    if ( url.scheme().Compare ( ToWebString("http") ) == 0 || url.scheme().Compare ( ToWebString("https") ) == 0 ) // Todo: Check how Awesomium reacts to other protocols
    {
        // Block if we're dealing with a remote page in local mode
        if ( pWebView->IsLocal () )
            return true; // Block

        if ( GetURLState ( ToSString(url.host()) ) != eURLState::WEBPAGE_ALLOWED )
            // Block the action
            return true;
    }

    // Don't do anything
    return false;
}*/

////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnRequest              //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#ac275121fdb030ff432c79d0337f0c19c //
//                                                                //
////////////////////////////////////////////////////////////////////
/*Awesomium::ResourceResponse* CWebCore::OnRequest ( Awesomium::ResourceRequest* pRequest )
{
    std::map<int, CWebView*>::iterator iter = m_WebViewMap.find ( pRequest->origin_process_id () );
    int i = pRequest->origin_process_id ();
    if (iter == m_WebViewMap.end ())
    {
        pRequest->Cancel ();
        return NULL;
    }

    CWebView* pWebView = iter->second;
    if ( !pWebView )
    {
        pRequest->Cancel (); // Block
        return NULL;
    }

    Awesomium::WebURL url = pRequest->url ();

    if ( url.scheme().Compare ( ToWebString("http") ) == 0 || url.scheme().Compare ( ToWebString("https") ) == 0 )
    {
        if (pWebView->IsLocal())
        {
            pRequest->Cancel (); // Block
            return NULL;
        }

        if ( GetURLState ( ToSString(url.host()) ) != eURLState::WEBPAGE_ALLOWED )
            // Block the action
            pRequest->Cancel ();
    }

    // We don't want to modify anything
    return NULL;
}*/


/*Awesomium::WebString CWebCore::ToWebString ( const SString& strString )
{
    return Awesomium::WSLit ( strString.c_str () );
}

SString CWebCore::ToSString ( const Awesomium::WebString& webString )
{
    return SharedUtil::ToUTF8 ( std::wstring( (wchar_t*)webString.data () ) );
}
*/

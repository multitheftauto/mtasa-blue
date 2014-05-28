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

CWebCore::CWebCore ()
{
    m_pWebCore = NULL;
    m_pRequestsGUI = NULL;
    m_bTestmodeEnabled = false;

    Initialise ();
    InitialiseWhiteAndBlacklist ();
}

CWebCore::~CWebCore ()
{
    // Shutdown Awesomium
    m_pWebCore->Shutdown ();

    if ( m_pRequestsGUI )
        delete m_pRequestsGUI;
}

bool CWebCore::Initialise ()
{
    Awesomium::WebConfig webConfig;
    webConfig.user_agent = Awesomium::WSLit ( "Multi Theft Auto: San Andreas Client; Chromium" ); // Needs testing (Chromium adjustments might not work anymore)
    // webConfig.user_script = Awesomium::WSLit(""); // Todo: Implement a watermark within editboxes

    m_pWebCore = Awesomium::WebCore::Initialize ( webConfig );

    // Set all the handlers
    m_pWebCore->set_resource_interceptor ( this );

    return m_pWebCore != NULL;
}

CWebViewInterface* CWebCore::CreateWebView ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface, bool bIsLocal )
{
    // Create our webview implementation
    CWebView* pWebView = new CWebView ( uiWidth, uiHeight, pD3DSurface, bIsLocal );
    m_WebViewMap[pWebView->GetAwesomiumView ()->process_id ()] = pWebView;
    return pWebView;
}

void CWebCore::DestroyWebView ( CWebViewInterface* pWebViewInterface )
{
    CWebView* pWebView = dynamic_cast<CWebView*> ( pWebViewInterface );
    if ( pWebView )
    {
        m_WebViewMap.erase ( pWebView->GetAwesomiumView ()->process_id () );
        delete pWebView;
    }
}

void CWebCore::Update ()
{
    // Update webbrowser rendering etc.
    m_pWebCore->Update ();
}

eURLState CWebCore::GetURLState ( const SString& strURL )
{
    // Initialize wildcard whitelist (be careful with modifying) | Todo: Think about the following
    static SString wildcardWhitelist[] = { "*.googlevideo.com", "*.vimeocdn.com" };

    for (int i = 0; i < sizeof(wildcardWhitelist) / sizeof(SString); ++i)
    {
        if (WildcardMatch(wildcardWhitelist[i], strURL))
            return eURLState::WEBPAGE_ALLOWED;
    }

    google::dense_hash_map<SString, bool>::iterator iter = m_Whitelist.find ( strURL );
    if ( iter != m_Whitelist.end () )
    {
        if ( iter->second == true )
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

void CWebCore::ClearWhitelist ()
{
    // Clear old data
    m_Whitelist.clear ();
    m_PendingRequests.clear ();

    // Re-add "default" entries
    InitialiseWhiteAndBlacklist ();
}

void CWebCore::InitialiseWhiteAndBlacklist ()
{
    // Hardcoded whitelist
    static SString whitelist[] = { 
        "google.com", "www.youtube.com", "youtube.com", "s.youtube.com", "s.ytimg.com", "vimeo.com", "player.vimeo.com",
        "myvideo.com", "reddit.com", "mtasa.com", "multitheftauto.com", "mtavc.com"
    };

    // Hardcoded blacklist
    static SString blacklist[] = { "nobrain.dk" };

    // Blacklist or whitelist URLs now
    for ( unsigned int i = 0; i < sizeof(whitelist) / sizeof(SString); ++i )
    {
        m_Whitelist[whitelist[i]] = true;
    }
    for ( unsigned int i = 0; i < sizeof(blacklist) / sizeof(SString); ++i )
    {
        m_Whitelist[blacklist[i]] = false;
    }

    // Todo: Implement dynamic whitelist + blacklist hosted on a MTA QA server

}

void CWebCore::AddAllowedPage ( const SString& strURL )
{
    m_Whitelist[strURL] = true;
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
    else
    {
        // Tell the client that the requested pages are already allowed
        CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler ( true );
    }
}

void CWebCore::AllowPendingPages ()
{
    for ( std::vector<SString>::iterator iter = m_PendingRequests.begin(); iter != m_PendingRequests.end(); ++iter )
    {
        m_Whitelist[*iter] = true;
    }
    m_PendingRequests.clear ();

    // Trigger an event now
    CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler ( true );
}

void CWebCore::DenyPendingPages ()
{
    m_PendingRequests.clear ();
    
    // Trigger an event now
    CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler ( false );
}

bool CWebCore::CanLoadRemotePages ()
{
    bool bCanLoadRemotePages;
    CVARS_GET ( "browser_remote_websites", bCanLoadRemotePages );
    return bCanLoadRemotePages;
}


////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnFilterNavigation     //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#aceecadf1ddd8e3fe42cd56bc74d6ec6c //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebCore::OnFilterNavigation ( int origin_process_id, int origin_routing_id, const Awesomium::WebString& method, const Awesomium::WebURL& url, bool is_main_frame )
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
}

////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnRequest              //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#ac275121fdb030ff432c79d0337f0c19c //
//                                                                //
////////////////////////////////////////////////////////////////////
Awesomium::ResourceResponse* CWebCore::OnRequest ( Awesomium::ResourceRequest* pRequest )
{
    std::map<int, CWebView*>::iterator iter = m_WebViewMap.find ( pRequest->origin_process_id () );
    int i = pRequest->origin_process_id ();
    if (iter == m_WebViewMap.end())
    {
        pRequest->Cancel();
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
}


Awesomium::WebString CWebCore::ToWebString ( const SString& strString )
{
    return Awesomium::WSLit ( strString.c_str () );
}

SString CWebCore::ToSString ( const Awesomium::WebString& webString )
{
    return SharedUtil::ToUTF8 ( std::wstring( (wchar_t*)webString.data () ) );
}

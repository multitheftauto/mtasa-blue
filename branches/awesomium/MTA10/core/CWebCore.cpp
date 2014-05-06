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
    m_pWebCore = Awesomium::WebCore::Initialize ( Awesomium::WebConfig () );
    return m_pWebCore != NULL;
}

CWebViewInterface* CWebCore::CreateWebView ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface )
{
    // Create our webview implementation
    CWebView* pWebView = new CWebView ( uiWidth, uiHeight, pD3DSurface );
    return pWebView;
}

void CWebCore::Update ()
{
    // Update webbrowser rendering etc.
    m_pWebCore->Update ();
}

bool CWebCore::IsURLAllowed ( const SString& strURL )
{
    // Todo: Implement wildcards
    google::dense_hash_map<SString, bool>::iterator iter = m_Whitelist.find ( strURL );
    if ( iter != m_Whitelist.end() && iter->second == true )
        return true;

    return false;
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
    static SString whitelist[] = { "google.com", "youtube.com", "reddit.com", "mtasa.com", "multitheftauto.com" };

    // Hardcoded blacklist
    static SString blacklist[] = { "nobrain.dk" };

    // Blacklist or whitelist URLs now
    for (unsigned int i = 0; i < sizeof(whitelist) / sizeof(SString); ++i)
    {
        m_Whitelist[whitelist[i]] = true;
    }
    for (unsigned int i = 0; i < sizeof(blacklist) / sizeof(SString); ++i)
    {
        m_Whitelist[whitelist[i]] = false;
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
        if ( IsURLAllowed ( *iter ) )
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


////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnFilterNavigation     //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#aceecadf1ddd8e3fe42cd56bc74d6ec6c //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebCore::OnFilterNavigation ( int origin_process_id, int origin_routing_id, const Awesomium::WebString& method, const Awesomium::WebURL& url, bool is_main_frame )
{
    if ( !IsURLAllowed ( ToSString ( url.host () ) ) )
        // Block the action
        return true;

    // Don't do anything
    return false;
}

////////////////////////////////////////////////////////////////////
//                                                                //
//    Implementation: ResourceInterceptor::OnFilterNavigation     //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html#ac275121fdb030ff432c79d0337f0c19c //
//                                                                //
////////////////////////////////////////////////////////////////////
Awesomium::ResourceResponse* CWebCore::OnRequest ( Awesomium::ResourceRequest* pRequest )
{
    if ( !IsURLAllowed ( ToSString ( pRequest->url ().host () ) ) )
        // Block the action
        pRequest->Cancel ();

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

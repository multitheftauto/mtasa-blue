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
    for ( std::vector<SString>::iterator iter = m_Whitelist.begin(); iter != m_Whitelist.end(); ++iter )
    {
        if ( *iter == strURL )
            return true;
    }
    return false;
}

void CWebCore::ClearWhitelist ()
{
    m_Whitelist.clear ();
    m_PendingRequests.clear ();
}

void CWebCore::AddAllowedPage ( const SString& strURL )
{
    m_Whitelist.push_back ( strURL );
}

void CWebCore::RequestPages ( const std::vector<SString>& pages )
{
    // Add to pending pages queue
    bool bNewItem = false;
    for ( std::vector<SString>::const_iterator iter = pages.begin(); iter != pages.end(); ++iter )
    {
        if ( IsURLAllowed(*iter) )
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
        m_Whitelist.push_back ( *iter );
    }
    m_PendingRequests.clear ();

    // Trigger an event now
    //CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler(true);
}

void CWebCore::DenyPendingPages ()
{
    m_PendingRequests.clear ();
    
    // Trigger an event now
    //CModManager::GetSingleton().GetCurrentMod()->WebsiteRequestResultHandler(false);
}

////////////////////////////////////////////////////////////////////
//                                                                //
//          CWebCoreResourceInterceptor section                 //
//  This methods get called when the player requests resources    //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_resource_interceptor.html //
//                                                                //
////////////////////////////////////////////////////////////////////
/*bool CWebCoreResourceInterceptor::OnFilterNavigation(int origin_process_id, int origin_routing_id, const Awesomium::WebString& method, const Awesomium::WebURL& url, bool is_main_frame)
{
    if (!g_pCore->GetWebBrowser()->IsURLAllowed(url.host()))
        // Block the action
        return true;

    // Don't do anything
    return false;
}

Awesomium::ResourceResponse* CWebCoreResourceInterceptor::OnRequest(Awesomium::ResourceRequest* pRequest)
{
    if (!g_pCore->GetWebBrowser()->IsURLAllowed(pRequest->url().host()))
        // Block the action
        pRequest->Cancel();

    // We don't want to modify anything
    return NULL;
}*/


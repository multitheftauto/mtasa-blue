/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebCore.h
*  PURPOSE:     Webbrowser class
*
*****************************************************************************/

#ifndef __CWEBCORE_H
#define __CWEBCORE_H

#include <core/CWebCoreInterface.h>
#include <Awesomium/WebCore.h>

class CWebBrowserItem;
class CWebsiteRequests;

class CWebCore : public CWebCoreInterface, public Awesomium::ResourceInterceptor
{
public:
    CWebCore();
    ~CWebCore();
    bool                Initialise();

    CWebViewInterface*  CreateWebView       ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface );
    void                Update              ();
    
    bool                IsURLAllowed        ( const SString& strURL );
    void                ClearWhitelist      ();
    void                InitialiseWhiteAndBlacklist ();
    void                AddAllowedPage      ( const SString& strURL );
    void                RequestPages        ( const std::vector<SString>& pages );
    void                AllowPendingPages   ();
    void                DenyPendingPages    ();

    // Awesomium::ResourceInterceptor implementations
    virtual bool                          OnFilterNavigation ( int origin_process_id, int origin_routing_id, const Awesomium::WebString& method, const Awesomium::WebURL& url, bool is_main_frame );
    virtual Awesomium::ResourceResponse*  OnRequest ( Awesomium::ResourceRequest* pRequest);

    // Utilities
    static Awesomium::WebString           ToWebString ( const SString& strString );
    static SString                        ToSString   ( const Awesomium::WebString& webString );

private:
    Awesomium::WebCore* m_pWebCore;
    CWebsiteRequests*   m_pRequestsGUI;

    CFastHashMap<SString, bool> m_Whitelist;
    std::vector<SString>        m_PendingRequests;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebDevTools.cpp
*  PURPOSE:     Web dev tools class
*
*****************************************************************************/
#include "StdInc.h"
#include "CWebDevTools.h"

bool CWebDevTools::Show ( CWebView* pWebView )
{
    auto pBrowser = pWebView->GetCefBrowser ();
    if (!pBrowser)
        return false;

    // Default settings are suitable
    CefBrowserSettings settings;

    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup ( NULL, "CEF Dev Tools (MTA:SA)" );
    
    // Create independent CefClient (to bypass access restrictions)
    CefRefPtr<CefClient> client { new CDevToolsClient };

    // ...and show the devtools
    pBrowser->GetHost ()->ShowDevTools ( windowInfo, client, settings, CefPoint () );
    return true;
}

bool CWebDevTools::Close ( CWebView* pWebView )
{
    auto pBrowser = pWebView->GetCefBrowser();
    if (!pBrowser)
        return false;

    pBrowser->GetHost ()->CloseDevTools ();
    return true;
}


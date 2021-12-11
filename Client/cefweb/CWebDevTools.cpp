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

bool CWebDevTools::Show(CWebView* pWebView)
{
    auto browser = pWebView->GetCefBrowser();

    if (!browser)
        return false;

    CefBrowser* pBrowser = browser.get();

    // Default settings are suitable
    CefBrowserSettings settings;

    CefWindowInfo windowInfo;
    windowInfo.SetAsPopup(nullptr, "CEF Dev Tools (MTA:SA)");

    // Create independent CefClient (to bypass access restrictions)
    CefRefPtr<CefClient> client{new CDevToolsClient};

    // ...and show the devtools
    pBrowser->GetHost()->ShowDevTools(windowInfo, client, settings, CefPoint());
    return true;
}

bool CWebDevTools::Close(CWebView* pWebView)
{
    auto browser = pWebView->GetCefBrowser();

    if (!browser)
        return false;

    auto host = browser.get()->GetHost();

    if (host)
        host->CloseDevTools();

    return true;
}

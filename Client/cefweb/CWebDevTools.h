/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebDevTools.h
*  PURPOSE:     Web dev tools class
*
*****************************************************************************/
#pragma once

class CWebDevTools
{
public:
    static bool Show ( CWebView* pWebView );
    static bool Close ( CWebView* pWebView );
};

class CDevToolsClient : public CefClient
{
public:
    CDevToolsClient() = default;

    IMPLEMENT_REFCOUNTING(CDevToolsClient);
};

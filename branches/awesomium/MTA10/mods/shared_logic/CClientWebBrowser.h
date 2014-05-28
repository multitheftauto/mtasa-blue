/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWebBrowser.h
*  PURPOSE:     ClientEntity webbrowser tab class
*
*****************************************************************************/

#ifndef __CCLIENTWEBBROWSER_H
#define __CCLIENTWEBBROWSER_H

#include <core/CWebViewInterface.h>

class CClientWebBrowser : public CClientTexture
{
    DECLARE_CLASS(CClientWebBrowser, CClientTexture)
public:
    CClientWebBrowser           ( CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, CWebViewInterface* pWebView );
    ~CClientWebBrowser          ();

    inline CWebBrowserItem*     GetWebBrowserItem  ()    { return (CWebBrowserItem*)m_pRenderItem; }
    inline CWebViewInterface*   GetWebView         ()    { return m_pWebView; }

    bool                        IsLoading           ();
    bool                        LoadURL             ( const SString& strURL, bool bFilterEnabled = true );
    void                        GetTitle            ( SString& outPageTitle );
    void                        GetURL              ( SString& outURL );
    void                        SetRenderingPaused  ( bool bPaused );

    bool                        ExecuteJavascript   ( const SString& strJavascriptCode );

    void                        InjectMouseMove     ( int iPosX, int iPosY );
    void                        InjectMouseDown     ( int mouseButton );
    void                        InjectMouseUp       ( int mouseButton );
    void                        InjectMouseWheel    ( int iScrollVert, int iScrollHorz );
    void                        InjectKeyboardEvent ( const SString& strKey, bool bKeyDown = true, bool bCharacter = false );

    bool                        IsLocal             ();
    void                        SetTempURL          ( const SString& strTempURL );

private:
    CWebViewInterface* m_pWebView;
};

#endif

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
#include <core/CWebBrowserEventsInterface.h>

class CClientWebBrowser : public CClientTexture, public CWebBrowserEventsInterface
{
    DECLARE_CLASS(CClientWebBrowser, CClientTexture)
public:
    CClientWebBrowser           ( CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, CWebViewInterface* pWebView );
    ~CClientWebBrowser          ();

    inline CWebBrowserItem*     GetWebBrowserItem   ()    { return (CWebBrowserItem*)m_pRenderItem; }
    inline CWebViewInterface*   GetWebView          ()    { return m_pWebView; }

    bool                        IsLoading           ();
    bool                        LoadURL             ( const SString& strURL, bool bFilterEnabled = true );
    void                        GetTitle            ( SString& outPageTitle );
    void                        GetURL              ( SString& outURL );
    void                        SetRenderingPaused  ( bool bPaused );
    void                        SetTransparent      ( bool bTransparent );
    void                        Focus               ();

    bool                        ExecuteJavascript   ( const SString& strJavascriptCode );

    void                        InjectMouseMove     ( int iPosX, int iPosY );
    void                        InjectMouseDown     ( int mouseButton );
    void                        InjectMouseUp       ( int mouseButton );
    void                        InjectMouseWheel    ( int iScrollVert, int iScrollHorz );
    void                        InjectKeyboardEvent ( const SString& strKey, bool bKeyDown = true, bool bCharacter = false );

    bool                        IsLocal             ();
    void                        SetTempURL          ( const SString& strTempURL );

    bool                        SetAudioVolume      ( float fVolume );

    
    // CWebBrowserEventsInterface implementation
    void                        Events_OnDocumentReady ( const SString& strURL );
    void                        Events_OnLoadingFailed ( const SString& strURL, int errorCode, const SString& errorDescription );
    void                        Events_OnNavigate      ( const SString& strURL, bool bMainFrame );
    void                        Events_OnPopup         ( const SString& strTargetURL, const SString& strOpenerURL, bool bPopup );
    void                        Events_OnChangeCursor  ( unsigned char ucCursor );

private:
    CWebViewInterface* m_pWebView;
};

#endif

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
    CClientWebBrowser           ( CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, bool bLocal, bool bTransparent );
    ~CClientWebBrowser          ();
    eClientEntityType           GetType             ( ) const { return CCLIENTBROWSER; }
    virtual void                Unlink () override;

    inline CWebBrowserItem*     GetWebBrowserItem   ()    { return (CWebBrowserItem*)m_pRenderItem; }
    inline CWebViewInterface*   GetWebView          ()    { return m_pWebView; }

    bool                        IsLoading           ();
    bool                        LoadURL             ( const SString& strURL, bool bFilterEnabled = true );
    void                        GetTitle            ( SString& outPageTitle );
    void                        GetURL              ( SString& outURL );
    void                        SetRenderingPaused  ( bool bPaused );
    void                        Focus               ();

    bool                        ExecuteJavascript   ( const SString& strJavascriptCode );

    void                        InjectMouseMove     ( int iPosX, int iPosY );
    void                        InjectMouseDown     ( eWebBrowserMouseButton mouseButton );
    void                        InjectMouseUp       ( eWebBrowserMouseButton mouseButton );
    void                        InjectMouseWheel    ( int iScrollVert, int iScrollHorz );

    bool                        IsLocal             ();
    void                        SetTempURL          ( const SString& strTempURL );

    inline CResource*           GetResource         () { return m_pResource; }
    inline void                 SetResource         ( CResource* pResource ) { m_pResource = pResource; }

    bool                        SetAudioVolume      ( float fVolume );

    
    // CWebBrowserEventsInterface implementation
    void                        Events_OnCreated       () override;
    void                        Events_OnDocumentReady ( const SString& strURL ) override;
    void                        Events_OnLoadingFailed ( const SString& strURL, int errorCode, const SString& errorDescription ) override;
    void                        Events_OnNavigate      ( const SString& strURL, bool bMainFrame ) override;
    void                        Events_OnPopup         ( const SString& strTargetURL, const SString& strOpenerURL ) override;
    void                        Events_OnChangeCursor  ( unsigned char ucCursor ) override;
    void                        Events_OnTriggerEvent  ( const SString& strEventName, const std::vector<std::string>& arguments, bool bIsServer = false ) override;
    void                        Events_OnTooltip       ( const SString& strTooltip ) override;
    void                        Events_OnInputFocusChanged ( bool bGainedFocus ) override;
    bool                        Events_OnResourcePathCheck ( SString& strURL ) override;

private:
    CWebViewInterface* m_pWebView;
    CResource*         m_pResource;
};

#endif

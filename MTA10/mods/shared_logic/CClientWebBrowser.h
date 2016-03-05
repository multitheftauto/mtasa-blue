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
#include <core/CAjaxResourceHandlerInterface.h>
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
    bool                        LoadURL             ( const SString& strURL, bool bFilterEnabled = true, const SString& strPostData = SString(), bool bURLEncoded = true );
    const SString&              GetTitle            ();
    SString                     GetURL              ();
    void                        SetRenderingPaused  ( bool bPaused );
    void                        Focus               ();

    bool                        ExecuteJavascript   ( const SString& strJavascriptCode );

    bool                        SetProperty         ( const SString& strKey, const SString& strValue );
    bool                        GetProperty         ( const SString& strKey, SString& outValue );

    void                        InjectMouseMove     ( int iPosX, int iPosY );
    void                        InjectMouseDown     ( eWebBrowserMouseButton mouseButton );
    void                        InjectMouseUp       ( eWebBrowserMouseButton mouseButton );
    void                        InjectMouseWheel    ( int iScrollVert, int iScrollHorz );

    bool                        IsLocal             ();

    inline CResource*           GetResource         () { return m_pResource; }
    inline void                 SetResource         ( CResource* pResource ) { m_pResource = pResource; }

    float                       GetAudioVolume      ();
    bool                        SetAudioVolume      ( float fVolume );

    void                        GetSourceCode       ( const std::function<void( const std::string& code )>& callback );

    using ajax_callback_t = const std::function<const SString ( std::vector<SString>& vecGet, std::vector<SString>& vecPost )>;
    
    bool                        AddAjaxHandler      ( const SString& strURL, ajax_callback_t& handler );
    bool                        RemoveAjaxHandler   ( const SString& strURL );

    bool                        ToggleDevTools      ( bool visible );

    bool                        CanGoBack           ();
    bool                        CanGoForward        ();
    bool                        GoBack              ();
    bool                        GoForward           ();
    void                        Refresh             ( bool bIgnoreCache );

    // CWebBrowserEventsInterface implementation
    void                        Events_OnCreated       () override;
    void                        Events_OnLoadingStart  ( const SString& strURL, bool bMainFrame ) override;
    void                        Events_OnDocumentReady ( const SString& strURL ) override;
    void                        Events_OnLoadingFailed ( const SString& strURL, int errorCode, const SString& errorDescription ) override;
    void                        Events_OnNavigate      ( const SString& strURL, bool bIsBlocked, bool isMainFrame ) override;
    void                        Events_OnPopup         ( const SString& strTargetURL, const SString& strOpenerURL ) override;
    void                        Events_OnChangeCursor  ( unsigned char ucCursor ) override;
    void                        Events_OnTriggerEvent  ( const SString& strEventName, const std::vector<std::string>& arguments ) override;
    void                        Events_OnTooltip       ( const SString& strTooltip ) override;
    void                        Events_OnInputFocusChanged ( bool bGainedFocus ) override;
    bool                        Events_OnResourcePathCheck ( SString& strURL ) override;
    bool                        Events_OnResourceFileCheck ( const SString& strURL ) override;
    void                        Events_OnResourceBlocked   ( const SString& strURL, const SString& strDomain, unsigned char reason ) override;
    void                        Events_OnAjaxRequest ( CAjaxResourceHandlerInterface* pHandler, const SString& strURL ) override;

private:
    CWebViewInterface* m_pWebView;
    CResource*         m_pResource;
    std::map<SString, ajax_callback_t> m_mapAjaxCallback;
};

class CClientGUIWebBrowser : public CClientGUIElement
{
public:
    CClientGUIWebBrowser ( bool isLocal, bool isTransparent, uint width, uint height, CClientManager* pManager, CLuaMain* pLuaMain, CGUIElement* pCGUIElement, ElementID ID = INVALID_ELEMENT_ID );
    
    inline CClientWebBrowser* GetBrowser () { return m_pBrowser; }

private:
    CClientWebBrowser* m_pBrowser;
};

#endif

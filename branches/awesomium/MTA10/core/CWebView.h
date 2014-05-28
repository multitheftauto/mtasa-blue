/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.h
*  PURPOSE:     Web view class
*
*****************************************************************************/

#ifndef __CWEBVIEW_H
#define __CWEBVIEW_H

#include <core/CWebViewInterface.h>
#include <Awesomium/WebView.h>
#include <Awesomium/WebViewListener.h>
#include <Awesomium/STLHelpers.h>
#include <d3d9.h>
#include <SString.h>
#include "WebBrowserHelpers.h"

class CWebView : public CWebViewInterface, public Awesomium::WebViewListener::Load
{
    friend class CRenderItemManager;

public:
    CWebView                    ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface, bool bIsLocal );
    ~CWebView                   ();
    Awesomium::WebView*         GetAwesomiumView () { return m_pWebView; };

    // Exported methods
    bool LoadURL                ( const SString& strURL, bool bFilterEnabled = true );
    bool IsLoading              ();
    void GetURL                 ( SString& outURL );
    void GetTitle               ( SString& outTitle );
    void SetRenderingPaused     ( bool bPaused );

    void ExecuteJavascript      ( const SString& strJavascriptCode );

    void InjectMouseMove        ( int iPosX, int iPosY );
    void InjectMouseDown        ( int mouseButton );
    void InjectMouseUp          ( int mouseButton );
    void InjectMouseWheel       ( int iScrollVert, int iScrollHorz );
    void InjectKeyboardEvent    ( const SString& strKey, bool bKeyDown = true, bool bCharacter = false );

    bool IsLocal                ()                                                  { return m_bIsLocal; };
    void SetTempURL             ( const SString& strTempURL )                       { m_strTempURL = strTempURL; };


    // Implementation: Awesomium::WebViewListener::Load
    virtual void OnBeginLoadingFrame    ( Awesomium::WebView *caller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, bool is_error_page ) {};
    virtual void OnFailLoadingFrame     ( Awesomium::WebView *caller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, int error_code, const Awesomium::WebString &error_desc ) {};
    virtual void OnFinishLoadingFrame   ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url );
    virtual void OnDocumentReady(Awesomium::WebView *caller, const Awesomium::WebURL &url) {};

    // Static javascript method implementations
    static void Javascript_triggerEvent(Awesomium::WebView* pWebView, const Awesomium::JSArray& args);
    
private:
    Awesomium::WebView* m_pWebView;
    IDirect3DSurface9*  m_pD3DSurface;

    bool                m_bIsLocal;
    SString             m_strTempURL;

    CJSMethodHandler    m_JSMethodHandler;
};

#endif

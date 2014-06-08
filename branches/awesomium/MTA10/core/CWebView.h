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
#include <core/CWebBrowserEventsInterface.h>
#include <Awesomium/WebView.h>
#include <Awesomium/WebViewListener.h>
#include <Awesomium/STLHelpers.h>
#include <d3d9.h>
#include <SString.h>
#include "WebBrowserHelpers.h"
#include <mmdeviceapi.h>
#include <audiopolicy.h>

class CWebView : public CWebViewInterface, public Awesomium::WebViewListener::Load
{
public:
    CWebView                    ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal );
    ~CWebView                   ();
    Awesomium::WebView*         GetAwesomiumView () { return m_pWebView; };
    void                        SetWebBrowserEvents ( CWebBrowserEventsInterface* pInterface ) { m_pEventsInterface = pInterface; };

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

    bool SetAudioVolume         ( float fVolume );


    // Implementation: Awesomium::WebViewListener::Load
    virtual void OnBeginLoadingFrame    ( Awesomium::WebView* pCaller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL& url, bool is_error_page ) {};
    virtual void OnFailLoadingFrame     ( Awesomium::WebView* pCaller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL& url, int error_code, const Awesomium::WebString& error_desc );
    virtual void OnFinishLoadingFrame   ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url );
    virtual void OnDocumentReady        ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url );

    // Static javascript method implementations
    static void Javascript_triggerEvent(Awesomium::WebView* pWebView, const Awesomium::JSArray& args);
    
private:
    Awesomium::WebView* m_pWebView;

    bool                m_bIsLocal;
    SString             m_strTempURL;

    CJSMethodHandler    m_JSMethodHandler;
    CWebBrowserEventsInterface* m_pEventsInterface;
};

#endif

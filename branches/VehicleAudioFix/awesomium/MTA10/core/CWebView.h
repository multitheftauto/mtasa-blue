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

class CWebView : public CWebViewInterface, public Awesomium::WebViewListener::Load, public Awesomium::WebViewListener::View
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
    virtual void OnBeginLoadingFrame    ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, bool bErrorPage );
    virtual void OnFailLoadingFrame     ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, int error_code, const Awesomium::WebString& error_desc );
    virtual void OnFinishLoadingFrame   ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url );
    virtual void OnDocumentReady        ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url );

    // Implementation: Awesomium::WebViewListener::View
    virtual void OnChangeTitle          ( Awesomium::WebView* pCaller, const Awesomium::WebString& title ) {};
    virtual void OnChangeAddressBar     ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url ) {};
    virtual void OnChangeTooltip        ( Awesomium::WebView* pCaller, const Awesomium::WebString& tooltip ) {};
    virtual void OnChangeTargetURL      ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url ) {};
    virtual void OnChangeCursor         ( Awesomium::WebView* pCaller, Awesomium::Cursor cursor ) {};
    virtual void OnChangeFocus          ( Awesomium::WebView* pCaller, Awesomium::FocusedElementType focused_type ) {};
    virtual void OnAddConsoleMessage    ( Awesomium::WebView* pCaller, const Awesomium::WebString& message, int line_number, const Awesomium::WebString& source ) {};
    virtual void OnShowCreatedWebView   ( Awesomium::WebView* pCaller, Awesomium::WebView* new_view, const Awesomium::WebURL& opener_url, const Awesomium::WebURL& target_url, const Awesomium::Rect& initial_pos, bool is_popup );

    // Static javascript method implementations
    static void Javascript_triggerEvent(Awesomium::WebView* pWebView, const Awesomium::JSArray& args);

protected:
    void ConvertURL ( const Awesomium::WebURL& url, SString& convertedURL );
    
private:
    Awesomium::WebView* m_pWebView;

    bool                m_bIsLocal;
    SString             m_strTempURL;

    CJSMethodHandler    m_JSMethodHandler;
    CWebBrowserEventsInterface* m_pEventsInterface;
};

#endif

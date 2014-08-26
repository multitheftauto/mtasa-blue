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

#undef GetNextSibling
#include <core/CWebViewInterface.h>
#include <core/CWebBrowserEventsInterface.h>
#include <cef3/include/cef_app.h>
#include <cef3/include/cef_browser.h>
#include <cef3/include/cef_client.h>
#include <cef3/include/cef_render_handler.h>
#include <d3d9.h>
#include <SString.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#define GetNextSibling(hwnd) GetWindow(hwnd, GW_HWNDNEXT) // Re-define the conflicting macro

class CWebView : public CWebViewInterface, public CefClient, public CefRenderHandler //, public Awesomium::WebViewListener::Load, public Awesomium::WebViewListener::View
{
public:
    CWebView                    ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem );
    ~CWebView                   ();
    //Awesomium::WebView*         GetAwesomiumView () { return m_pWebView; };
    void                        SetWebBrowserEvents ( CWebBrowserEventsInterface* pInterface ) { m_pEventsInterface = pInterface; };

    // Exported methods
    bool LoadURL                ( const SString& strURL, bool bFilterEnabled = true );
    bool IsLoading              ();
    void GetURL                 ( SString& outURL );
    void GetTitle               ( SString& outTitle );
    void SetRenderingPaused     ( bool bPaused );
    void SetTransparent         ( bool bTransparent );
    void Focus                  ();

    void ExecuteJavascript      ( const SString& strJavascriptCode );

    void InjectMouseMove        ( int iPosX, int iPosY );
    void InjectMouseDown        ( int mouseButton );
    void InjectMouseUp          ( int mouseButton );
    void InjectMouseWheel       ( int iScrollVert, int iScrollHorz );
    void InjectKeyboardEvent    ( const SString& strKey, bool bKeyDown = true, bool bCharacter = false );

    bool IsLocal                ()                                                  { return m_bIsLocal; };
    void SetTempURL             ( const SString& strTempURL )                       { m_strTempURL = strTempURL; };

    bool SetAudioVolume         ( float fVolume );


    // CefClient methods
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; };
    
    // CefRenderHandler methods
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;

    // Implementation: Awesomium::WebViewListener::Load
    /*virtual void OnBeginLoadingFrame    ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, bool bErrorPage );
    virtual void OnFailLoadingFrame     ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, int error_code, const Awesomium::WebString& error_desc );
    virtual void OnFinishLoadingFrame   ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url );
    virtual void OnDocumentReady        ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url );*/

    // Implementation: Awesomium::WebViewListener::View
    /*virtual void OnChangeTitle          ( Awesomium::WebView* pCaller, const Awesomium::WebString& title ) {};
    virtual void OnChangeAddressBar     ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url ) {};
    virtual void OnChangeTooltip        ( Awesomium::WebView* pCaller, const Awesomium::WebString& tooltip ) {};
    virtual void OnChangeTargetURL      ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url ) {};
    virtual void OnChangeCursor         ( Awesomium::WebView* pCaller, Awesomium::Cursor cursor );
    virtual void OnChangeFocus          ( Awesomium::WebView* pCaller, Awesomium::FocusedElementType focused_type ) {};
    virtual void OnAddConsoleMessage    ( Awesomium::WebView* pCaller, const Awesomium::WebString& message, int line_number, const Awesomium::WebString& source ) {};
    virtual void OnShowCreatedWebView   ( Awesomium::WebView* pCaller, Awesomium::WebView* new_view, const Awesomium::WebURL& opener_url, const Awesomium::WebURL& target_url, const Awesomium::Rect& initial_pos, bool is_popup );

    // Static javascript method implementations
    static void Javascript_triggerEvent(Awesomium::WebView* pWebView, const Awesomium::JSArray& args);*/

protected:
    //void ConvertURL ( const Awesomium::WebURL& url, SString& convertedURL );
    
private:
    CefRefPtr<CefBrowser> m_pWebView;
    CWebBrowserItem*    m_pWebBrowserRenderItem;

    bool                m_bIsLocal;
    SString             m_strTempURL;
    POINT               m_vecMousePosition;

    //CJSMethodHandler    m_JSMethodHandler;
    CWebBrowserEventsInterface* m_pEventsInterface;

public:
    // Implement smartpointer methods (all Cef-classes require that since they are derived from CefBase)
    IMPLEMENT_REFCOUNTING(CWebView);
};

#endif

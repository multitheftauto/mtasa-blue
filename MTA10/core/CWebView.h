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
#undef GetFirstChild
#include <core/CWebViewInterface.h>
#include <core/CWebBrowserEventsInterface.h>
#include <cef3/include/cef_app.h>
#include <cef3/include/cef_browser.h>
#include <cef3/include/cef_client.h>
#include <cef3/include/cef_render_handler.h>
#include <cef3/include/cef_life_span_handler.h>
#include <SString.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <condition_variable>
#define GetNextSibling(hwnd) GetWindow(hwnd, GW_HWNDNEXT) // Re-define the conflicting macro
#define GetFirstChild(hwnd) GetTopWindow(hwnd)

#define MTA_CEF_USERAGENT "Multi Theft Auto: San Andreas Client " MTA_DM_BUILDTAG_LONG

class CWebView : public CWebViewInterface, private CefClient, private CefRenderHandler, private CefLoadHandler, private CefRequestHandler, private CefLifeSpanHandler, private CefJSDialogHandler, private CefDialogHandler, private CefDisplayHandler
{
public:
    CWebView                    ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent = false );
    virtual ~CWebView           ();
    void                        Initialise ();
    void                        SetWebBrowserEvents ( CWebBrowserEventsInterface* pInterface ) { m_pEventsInterface = pInterface; };
    void                        CloseBrowser ();
    inline CefRefPtr<CefBrowser>GetCefBrowser() { return m_pWebView; };

    inline bool                 IsBeingDestroyed () { return m_bBeingDestroyed; }
    void                        SetBeingDestroyed ( bool state ) { m_bBeingDestroyed = state; }

    // Exported methods
    bool LoadURL                ( const SString& strURL, bool bFilterEnabled = true, const SString& strPostData = SString(), bool bURLEncoded = true );
    bool IsLoading              ();
    SString GetURL              ();
    const SString& GetTitle     ();
    void SetRenderingPaused     ( bool bPaused );
    void Focus                  ( bool state = true );
    IDirect3DTexture9* GetTexture () { return static_cast<IDirect3DTexture9*>(m_pWebBrowserRenderItem->m_pD3DTexture); }
    void ClearTexture           ();
    inline void NotifyPaint     () { m_PaintCV.notify_one (); }

    inline bool HasInputFocus   () { return m_bHasInputFocus; }

    void ExecuteJavascript      ( const SString& strJavascriptCode );

    bool SetProperty            ( const SString& strKey, const SString& strValue );
    bool GetProperty            ( const SString& strKey, SString& outProperty );
    
    void InjectMouseMove        ( int iPosX, int iPosY );
    void InjectMouseDown        ( eWebBrowserMouseButton mouseButton );
    void InjectMouseUp          ( eWebBrowserMouseButton mouseButton );
    void InjectMouseWheel       ( int iScrollVert, int iScrollHorz );
    void InjectKeyboardEvent    ( const CefKeyEvent& keyEvent );

    bool IsLocal                ()                                                  { return m_bIsLocal; };

    inline float GetAudioVolume ()                                                  { return m_fVolume; };
    bool SetAudioVolume         ( float fVolume );

    void GetSourceCode          ( const std::function<void( const std::string& code )>& callback );

    bool GetFullPathFromLocal   ( SString& strPath );
    bool VerifyFile             ( const SString& strPath );
    
    virtual bool RegisterAjaxHandler ( const SString& strURL ) override;
    virtual bool UnregisterAjaxHandler ( const SString& strURL ) override;
    virtual bool HasAjaxHandler ( const SString& strURL );
    virtual void HandleAjaxRequest ( const SString& strURL, class CAjaxResourceHandler * pHandler );

    virtual bool ToggleDevTools ( bool visible ) override;

    bool CanGoBack              ();
    bool CanGoForward           ();
    bool GoBack                 ();
    bool GoForward              ();
    void Refresh                ( bool ignoreCache );

    // CefClient methods
    virtual CefRefPtr<CefRenderHandler>     GetRenderHandler() override { return this; };
    virtual CefRefPtr<CefLoadHandler>       GetLoadHandler() override { return this; };
    virtual CefRefPtr<CefRequestHandler>    GetRequestHandler() override { return this; };
    virtual CefRefPtr<CefLifeSpanHandler>   GetLifeSpanHandler() override { return this; };
    virtual CefRefPtr<CefJSDialogHandler>   GetJSDialogHandler() override { return this; };
    virtual CefRefPtr<CefDialogHandler>     GetDialogHandler() override { return this; };
    virtual CefRefPtr<CefDisplayHandler>    GetDisplayHandler() override { return this; };
    virtual bool OnProcessMessageReceived ( CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message ) override;

    // CefRenderHandler methods
    virtual bool GetViewRect    ( CefRefPtr<CefBrowser> browser, CefRect& rect ) override;
    virtual void OnPopupSize    ( CefRefPtr<CefBrowser> browser, const CefRect& rect ) override;
    virtual void OnPaint        ( CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height ) override;
    virtual void OnCursorChange ( CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& cursorInfo ) override;

    // CefLoadHandler methods
    virtual void OnLoadStart    ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame ) override;
    virtual void OnLoadEnd      ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode ) override;
    virtual void OnLoadError    ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedURL ) override;
    
    // CefRequestHandler methods
    virtual bool OnBeforeBrowse ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect ) override;
    virtual CefRequestHandler::ReturnValue OnBeforeResourceLoad ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback ) override;

    // CefLifeSpawnHandler methods
    virtual void OnBeforeClose  ( CefRefPtr<CefBrowser> browser ) override;
    virtual bool OnBeforePopup  ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access ) override;
    virtual void OnAfterCreated ( CefRefPtr<CefBrowser> browser) override;

    // CefJSDialogHandler methods
    virtual bool OnJSDialog     ( CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, CefJSDialogHandler::JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr< CefJSDialogCallback > callback, bool& suppress_message ) override;

    // CefDialogHandler methods
    virtual bool OnFileDialog  ( CefRefPtr<CefBrowser> browser, CefDialogHandler::FileDialogMode mode, const CefString& title, const CefString& default_file_name, const std::vector< CefString >& accept_types, int selected_accept_filter, CefRefPtr< CefFileDialogCallback > callback ) override;

    // CefDisplayHandler methods
    virtual void OnTitleChange ( CefRefPtr<CefBrowser> browser, const CefString& title ) override;
    virtual bool OnTooltip     ( CefRefPtr<CefBrowser> browser, CefString& text ) override;
    virtual bool OnConsoleMessage ( CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line ) override;
    
private:
    CefRefPtr<CefBrowser> m_pWebView;
    CWebBrowserItem*    m_pWebBrowserRenderItem;

    bool                m_bBeingDestroyed;
    bool                m_bIsLocal;
    bool                m_bIsTransparent;
    POINT               m_vecMousePosition;
    bool                m_mouseButtonStates[3];
    SString             m_CurrentTitle;
    float               m_fVolume;
    std::mutex          m_PaintMutex;
    std::condition_variable m_PaintCV;
    int                 m_RenderPopupOffsetX, m_RenderPopupOffsetY;
    std::map<SString, SString> m_Properties;
    bool                m_bHasInputFocus;
    std::set<std::string> m_AjaxHandlers;

    CWebBrowserEventsInterface* m_pEventsInterface;

public:
    // Implement smartpointer methods (all Cef-classes require that since they are derived from CefBase)
    IMPLEMENT_REFCOUNTING(CWebView);
};

#endif

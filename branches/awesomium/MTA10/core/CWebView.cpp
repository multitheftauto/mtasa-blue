/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.cpp
*  PURPOSE:     Web view class
*
*****************************************************************************/
#include "StdInc.h"
#include "CWebView.h"
#include <cef3/include/cef_url.h>

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent )
{
    m_bIsLocal = bIsLocal;
    m_pWebBrowserRenderItem = pWebBrowserRenderItem;

    // Initialise the web session (which holds the actual settings) in in-memory mode
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = g_pCore->GetFrameRateLimit ();
    bool bEnabledPlugins;
    CVARS_GET ( "browser_plugins", bEnabledPlugins );
    browserSettings.plugins = bEnabledPlugins ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
    browserSettings.java = cef_state_t::STATE_DISABLED;
    if ( !bIsLocal )
    {
        bool bEnabledJavascript;
        CVARS_GET ( "browser_remote_javascript", bEnabledJavascript );
        browserSettings.javascript = bEnabledJavascript ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
    }

    CefWindowInfo windowInfo;
    windowInfo.SetAsWindowless ( NULL, bTransparent );
    
    m_pWebView = CefBrowserHost::CreateBrowserSync ( windowInfo, this, "", browserSettings, NULL );
    m_bBeingDestroyed = false;

    // Set handlers
    //m_pWebView->set_js_method_handler ( &m_JSMethodHandler );
}

CWebView::~CWebView()
{
    if ( IsMainThread () )
    {
        if ( g_pCore->GetWebCore ()->GetFocusedWebView () == this )
            g_pCore->GetWebCore ()->SetFocusedWebView ( NULL );
    }

    // Ensure that CefRefPtr::~CefRefPtr doesn't try to release it twice (it has already been released in CWebView::OnBeforeClose)
    m_pWebView = NULL;
}

void CWebView::CloseBrowser ()
{
    // CefBrowserHost::CloseBrowser calls the destructor after the browser has been destroyed
    m_bBeingDestroyed = true;
    m_pWebView->GetHost ()->CloseBrowser ( true );
}

bool CWebView::LoadURL ( const SString& strURL, bool bFilterEnabled )
{
    CefURLParts urlParts;
    if ( !CefParseURL ( strURL, urlParts ) )
        return false; // Invalid URL

    // Are we allowed to browse this website?
    if ( bFilterEnabled && g_pCore->GetWebCore()->GetURLState ( UTF16ToMbUTF8 ( urlParts.host.str ) ) != eURLState::WEBPAGE_ALLOWED )
        return false;

    // Load it!
    m_pWebView->GetMainFrame ()->LoadURL ( strURL );
    return true;
}

bool CWebView::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

void CWebView::GetURL ( SString& outURL )
{
    if ( !m_bIsLocal )
        outURL = static_cast < SString > ( m_pWebView->GetMainFrame ()->GetURL () );
    else
        outURL = m_strTempURL;
}

void CWebView::GetTitle ( SString& outTitle )
{
    outTitle = static_cast < SString > ( m_pWebView->GetMainFrame ()->GetName () );
}

void CWebView::SetRenderingPaused ( bool bPaused )
{
    m_pWebView->GetHost ()->WasHidden ( bPaused );
}

void CWebView::Focus ()
{
    m_pWebView->GetHost ()->SetFocus ( true );
    g_pCore->GetWebCore()->SetFocusedWebView ( this );
}

void CWebView::ClearTexture ()
{
    IDirect3DSurface9* pD3DSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if ( !pD3DSurface )
        return;

    D3DLOCKED_RECT LockedRect;
    D3DSURFACE_DESC SurfaceDesc;

    pD3DSurface->GetDesc ( &SurfaceDesc );
    pD3DSurface->LockRect ( &LockedRect, NULL, 0 );

    memset ( LockedRect.pBits, 0xFF, SurfaceDesc.Width * SurfaceDesc.Height * 4 );
    pD3DSurface->UnlockRect();
}

void CWebView::ExecuteJavascript ( const SString& strJavascriptCode )
{
    m_pWebView->GetMainFrame ()->ExecuteJavaScript ( strJavascriptCode, "", 0 );
}

void CWebView::InjectMouseMove ( int iPosX, int iPosY )
{
    CefMouseEvent mouseEvent;
    mouseEvent.x = iPosX;
    mouseEvent.y = iPosY;
    m_pWebView->GetHost ()->SendMouseMoveEvent ( mouseEvent, false );
    m_vecMousePosition.x = iPosX;
    m_vecMousePosition.y = iPosY;
}

void CWebView::InjectMouseDown ( eWebBrowserMouseButton mouseButton )
{
    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    m_pWebView->GetHost ()->SendMouseClickEvent ( mouseEvent, static_cast < CefBrowserHost::MouseButtonType > ( mouseButton ), false, 1 );
}

void CWebView::InjectMouseUp ( eWebBrowserMouseButton mouseButton )
{
    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    m_pWebView->GetHost ()->SendMouseClickEvent ( mouseEvent, static_cast < CefBrowserHost::MouseButtonType > ( mouseButton ), true, 1 );
}

void CWebView::InjectMouseWheel ( int iScrollVert, int iScrollHorz )
{
    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    m_pWebView->GetHost ()->SendMouseWheelEvent ( mouseEvent, iScrollHorz, iScrollVert );
}

void CWebView::InjectKeyboardEvent ( const CefKeyEvent& keyEvent )
{
    m_pWebView->GetHost ()->SendKeyEvent ( keyEvent );
}

bool CWebView::SetAudioVolume ( float fVolume )
{
    if ( fVolume < 0.0f || fVolume > 1.0f )
        return false;

    // Since the necessary interfaces of the core audio API were introduced in Win7, we've to fallback to HTML5 audio
    SString strJSCode ( "var tags = document.getElementsByTagName('audio'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;} " \
        "tags = document.getElementsByTagName('video'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;}",
        fVolume, fVolume );

    m_pWebView->GetMainFrame ()->ExecuteJavaScript ( strJSCode, "", 0 );
    return true;
}


////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::GetViewRect                  //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#GetViewRect(CefRefPtr%3CCefBrowser%3E,CefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::GetViewRect ( CefRefPtr<CefBrowser> browser, CefRect& rect )
{
    if ( m_bBeingDestroyed )
        return false;

    IDirect3DSurface9* pD3DSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if ( !pD3DSurface )
        return false;

    D3DSURFACE_DESC SurfaceDesc;
    pD3DSurface->GetDesc ( &SurfaceDesc );

    rect.x = 0;
    rect.y = 0;
    rect.width = SurfaceDesc.Width;
    rect.height = SurfaceDesc.Height;
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnPaint                      //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPaint(CefRefPtr%3CCefBrowser%3E,PaintElementType,constRectList&,constvoid*,int,int) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPaint ( CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height )
{
    if ( m_bBeingDestroyed )
        return;

    IDirect3DSurface9* pD3DSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if ( !pD3DSurface )
        return;

    D3DLOCKED_RECT LockedRect;
    D3DSURFACE_DESC SurfaceDesc;

    pD3DSurface->GetDesc ( &SurfaceDesc );
    pD3DSurface->LockRect ( &LockedRect, NULL, 0 );
    
    memcpy ( LockedRect.pBits, buffer, width * height * 4 );
    pD3DSurface->UnlockRect ();
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnCursorChange               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnCursorChange(CefRefPtr%3CCefBrowser%3E,CefCursorHandle) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnCursorChange ( CefRefPtr<CefBrowser> browser, CefCursorHandle cursor )
{
    m_pEventsInterface->Events_OnChangeCursor ( reinterpret_cast < unsigned char > ( cursor ) );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadStart                    //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadStart(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadStart ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame )
{
    SString strURL;
    ConvertURL ( frame->GetURL (), strURL );

    if ( strURL == "blank" )
        return;

    // Trigger navigate event
    m_pEventsInterface->Events_OnNavigate ( strURL, frame->IsMain () );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadEnd                      //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadEnd(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,int) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadEnd ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode )
{
    // TODO: Check if we've to move this to OnLoadStart
    // Clear the old list
    //m_JSMethodHandler.Clear ();

    // Bind javascript global object
    if ( IsLocal () )
    {
        /*Awesomium::JSValue jsMTA = pCaller->CreateGlobalJavascriptObject ( Awesomium::WSLit("mta")) ;
        if ( jsMTA.IsObject () )
        {
            Awesomium::JSObject& mtaObject = jsMTA.ToObject();
            m_JSMethodHandler.Bind ( mtaObject, Awesomium::WSLit("triggerEvent"), Javascript_triggerEvent );
        }*/
    }

    if ( frame->IsMain () )
    {
	    SString strURL;
	    ConvertURL ( frame->GetURL (), strURL );
	    m_pEventsInterface->Events_OnDocumentReady ( strURL );
    }
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadError                    //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadError(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,ErrorCode,constCefString&,constCefString&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadError ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedURL )
{
    SString strURL;
    ConvertURL ( failedURL, strURL );
    m_pEventsInterface->Events_OnLoadingFailed ( strURL, errorCode, SString ( errorText ) );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::View:OnShowCreatedWebView //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_view.html#af42b69bd383b5b69130feb990ad235e2 //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::OnShowCreatedWebView ( Awesomium::WebView* pCaller, Awesomium::WebView* pNewView, const Awesomium::WebURL& opener_url, const Awesomium::WebURL& target_url, const Awesomium::Rect& initial_pos, bool is_popup )
{
    // Trigger the popup/new tab event
    SString strTagetURL, strOpenerURL;
    ConvertURL ( target_url, strTagetURL );
    ConvertURL ( opener_url, strOpenerURL );
    m_pEventsInterface->Events_OnPopup ( strTagetURL, strOpenerURL, is_popup );

    // Destroy the new view immediately since we want the scripter to handle the popup event via Lua
    pNewView->Destroy ();
}*/

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRequestHandler::OnBeforeBrowe               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeBrowse(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E,bool) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforeBrowse ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect )
{
    /*
        From documentation:
        The |request| object cannot be modified in this callback.
        CefLoadHandler::OnLoadingStateChange will be called twice in all cases. If the navigation is allowed CefLoadHandler::OnLoadStart and CefLoadHandler::OnLoadEnd will be called.
        If the navigation is canceled CefLoadHandler::OnLoadError will be called with an |errorCode| value of ERR_ABORTED. 
    */

    CefURLParts urlParts;
    if ( !CefParseURL ( request->GetURL(), urlParts ) )
        return true; // Cancel if invalid URL (this line will normally not be executed)
    
    WString scheme = urlParts.scheme.str;
    if ( scheme == L"http" || scheme == L"https" )
    {
        if ( IsLocal () )
            return true; // Block remote here requests generally

        if ( g_pCore->GetWebCore ()->GetURLState ( UTF16ToMbUTF8 ( urlParts.host.str ) ) != eURLState::WEBPAGE_ALLOWED )
            return true;
    }
    
    // Do not block local websites
    return false;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRequestHandler::OnBeforeResourceLoad        //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeResourceLoad(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforeResourceLoad ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request )
{
    // Mostly the same as CWebView::OnBeforeBrowse
    CefURLParts urlParts;
    if ( !CefParseURL ( request->GetURL (), urlParts ) )
        return true; // Cancel if invalid URL (this line will normally not be executed)

    WString scheme = urlParts.scheme.str;
    if ( scheme == L"http" || scheme == L"https" )
    {
        if ( IsLocal () )
            return true; // Block remote requests in local mode generally

        if ( g_pCore->GetWebCore()->GetURLState ( UTF16ToMbUTF8 ( urlParts.host.str ) ) != eURLState::WEBPAGE_ALLOWED )
            return true;
    }

    // Do not block local websites
    return false;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforeClose              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforeClose(CefRefPtr%3CCefBrowser%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeforeClose ( CefRefPtr<CefBrowser> browser )
{
    m_pWebView = NULL;
    //Release ();
}

////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: triggerEvent                 //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::Javascript_triggerEvent ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    if ( args.size() == 0 )
        return;

    // Convert JSArray to string array
    std::vector<SString> stringArray;

    for ( unsigned int i = 1; i < args.size(); ++i ) // Ignore first arg as it is the event name
    {
        stringArray.push_back ( CWebCore::ToSString ( args[i].ToString () ) );
    }

    // Pass string array to real triggerEvent
    SString strEventName = CWebCore::ToSString(args[0].ToString());
    CModManager::GetSingleton ().GetCurrentMod ()->WebsiteTriggerEventHandler ( strEventName, stringArray ); // Does anybody know a better way to trigger an event in the deathmatch module?
}*/

////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: alert                        //
//        Exports the javascript alert to Lua                     //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::Javascript_alert ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    SString message = "";
    if ( args.size() > 0 )
        message = CWebCore::ToSString ( args[0].ToString () );

    CModManager::GetSingleton().GetCurrentMod()->WebsiteAlertHandler ( message );
}
*/


void CWebView::ConvertURL ( const CefString& url, SString& convertedURL )
{
    CefURLParts urlParts;
    CefParseURL ( url, urlParts );
    WString scheme = urlParts.scheme.str;
    
    if ( scheme == L"http" || scheme == L"https" )
    {
        convertedURL = UTF16ToMbUTF8 ( urlParts.spec.str );
    }
    else
    {
        convertedURL = UTF16ToMbUTF8 ( urlParts.path.str ); // TODO: Probably only the filename
    }
}

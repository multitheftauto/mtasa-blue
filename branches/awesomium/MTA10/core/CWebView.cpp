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
#include <cef3/include/cef_task.h>
#include <cef3/include/cef_runnable.h>

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent )
{
    m_bIsLocal = bIsLocal;
    m_pWebBrowserRenderItem = pWebBrowserRenderItem;

    // Initialise the web session (which holds the actual settings) in in-memory mode
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = g_pCore->GetFrameRateLimit ();
    browserSettings.javascript_access_clipboard = cef_state_t::STATE_DISABLED;
    browserSettings.java = cef_state_t::STATE_DISABLED;
    browserSettings.caret_browsing = cef_state_t::STATE_ENABLED;
    browserSettings.universal_access_from_file_urls = cef_state_t::STATE_DISABLED; // Also filtered by resource interceptor, but set this nevertheless
    browserSettings.file_access_from_file_urls = cef_state_t::STATE_DISABLED;
    browserSettings.webgl = cef_state_t::STATE_ENABLED;
    browserSettings.javascript_open_windows = cef_state_t::STATE_DISABLED;

    bool bEnabledPlugins;
    CVARS_GET ( "browser_plugins", bEnabledPlugins );
    browserSettings.plugins = bEnabledPlugins ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
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
    outTitle = m_CurrentTitle;
}

void CWebView::SetRenderingPaused ( bool bPaused )
{
    m_pWebView->GetHost ()->WasHidden ( bPaused );
}

void CWebView::Focus ()
{
    m_pWebView->GetHost ()->SetFocus ( true );
    m_pWebView->GetHost ()->SendFocusEvent ( true );
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

void CWebView::TriggerLuaEvent ( const SString& strEventName, const std::vector<std::string> arguments, bool bIsServer )
{
    m_pEventsInterface->Events_OnTriggerEvent ( strEventName, arguments, bIsServer );
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
// Implementation: CefClient::OnProcessMessageReceived            //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html#OnProcessMessageReceived(CefRefPtr%3CCefBrowser%3E,CefProcessId,CefRefPtr%3CCefProcessMessage%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnProcessMessageReceived ( CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message )
{
    CefRefPtr<CefListValue> argList = message->GetArgumentList ();
    if ( message->GetName () == "TriggerLuaEvent" || message->GetName () == "TriggerServerLuaEvent" )
    {
        // Get event name
        CefString eventName = argList->GetString ( 0 );

        // Get number of arguments from IPC process message
        int numArgs = argList->GetInt ( 1 );

        // Get args
        std::vector<std::string> args;
        for ( int i = 2; i < numArgs + 2; ++i )
        {
            args.push_back ( argList->GetString ( i ) );
        }

        // Trigger Lua event now
        TriggerLuaEvent ( SString ( eventName ), args, message->GetName () == "TriggerServerLuaEvent" );

        // The message was handled
        return true;
    }

    // The message wasn't handled
    return false;
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

    // Dirty rect implementation, don't use this as loops are significantly slower than memcpy
    /*auto surfaceData = (int*)LockedRect.pBits;
    auto sourceData = (const int*)buffer;
    auto pitch = LockedRect.Pitch;

    for (auto& rect : dirtyRects) 
    {
        for (int y = rect.y; y < rect.y+rect.height; ++y)
        {
            for (int x = rect.x; x < rect.x+rect.width; ++x)
            {
                int index = y * pitch / 4 + x;
                surfaceData[index] = sourceData[index];
            }
        }
    }*/

    // Copy entire texture
    memcpy ( LockedRect.pBits, buffer, width * height * 4 );
    
    pD3DSurface->UnlockRect ();
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnCursorChange               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnCursorChange(CefRefPtr%3CCefBrowser%3E,CefCursorHandle) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnCursorChange ( CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& cursorInfo )
{
    // Find the cursor index by the cursor handle
    unsigned char cursorIndex = static_cast < unsigned char > ( type );
    m_pEventsInterface->Events_OnChangeCursor ( cursorIndex );
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
    
    if ( g_pCore->GetWebCore ()->GetFocusedWebView () == this )
        g_pCore->GetWebCore ()->SetFocusedWebView ( NULL );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforePopup              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforePopup(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,constCefString&,constCefString&,constCefPopupFeatures&,CefWindowInfo&,CefRefPtr%3CCefClient%3E&,CefBrowserSettings&,bool*) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforePopup ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access )
{
    // ATTENTION: This method is called on the IO thread
    
    // Trigger the popup/new tab event
    SString strTagetURL, strOpenerURL;
    ConvertURL ( target_url, strTagetURL );
    ConvertURL ( frame->GetURL (), strOpenerURL );
    
    // Post event call task on the UI thread (will be called during the next CefDoMessageLoopWork call)
    CefPostTask ( TID_UI, NewCefRunnableMethod ( this, &CWebView::TriggerPopupEvent, strTagetURL, strOpenerURL ) );

    // Block popups generally
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefJSDialogHandler::OnJSDialog                 //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefJSDialogHandler.html#OnJSDialog(CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,JSDialogType,constCefString&,constCefString&,CefRefPtr%3CCefJSDialogCallback%3E,bool&) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnJSDialog ( CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, CefJSDialogHandler::JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr< CefJSDialogCallback > callback, bool& suppress_message )
{
    // TODO: Provide a way to influence Javascript dialogs via Lua
    // e.g. addEventHandler("onClientBrowserDialog", browser, function(message, defaultText) continueBrowserDialog("My input") end)

    // Suppress the dialog
    suppress_message = true;
    return false;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDialogHandler::OnFileDialog                 //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefDialogHandler.html#OnFileDialog(CefRefPtr%3CCefBrowser%3E,FileDialogMode,constCefString&,constCefString&,conststd::vector%3CCefString%3E&,CefRefPtr%3CCefFileDialogCallback%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnFileDialog ( CefRefPtr<CefBrowser> browser, CefDialogHandler::FileDialogMode mode, const CefString& title, const CefString& default_file_name, const std::vector< CefString >& accept_types, CefRefPtr< CefFileDialogCallback > callback )
{
    // Don't show the dialog
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnTitleChange               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefDisplayHandler.html#OnTitleChange(CefRefPtr%3CCefBrowser%3E,constCefString&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnTitleChange ( CefRefPtr<CefBrowser> browser, const CefString& title )
{
    m_CurrentTitle = UTF16ToMbUTF8 ( title );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnTooltip                   //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefDisplayHandler.html#OnTooltip(CefRefPtr%3CCefBrowser%3E,CefString&) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnTooltip ( CefRefPtr<CefBrowser> browser, CefString& title )
{
    m_pEventsInterface->Events_OnTooltip ( UTF16ToMbUTF8 ( title ) );
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnConsoleMessage            //
// http://magpcss.org/ceforum/apidocs/projects/%28default%29/CefDisplayHandler.html#OnConsoleMessage%28CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,int%29 //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnConsoleMessage ( CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line )
{
    // Redirect console message to debug window (if development mode is enabled)
    if ( g_pCore->GetWebCore ()->IsTestModeEnabled () )
    {
        g_pCore->DebugPrintfColor ( "[BROWSER] Console: %s (%s)", 255, 0, 0, UTF16ToMbUTF8 ( message ).c_str (), UTF16ToMbUTF8 ( source ).c_str () );
    }

    return true;
}


void CWebView::ConvertURL ( const CefString& url, SString& convertedURL )
{
    CefURLParts urlParts;
    if ( !CefParseURL ( url, urlParts ) )
    {
        convertedURL = "";
        return;
    }
    WString scheme = urlParts.scheme.str;
    
    if ( scheme == L"http" || scheme == L"https" )
    {
        convertedURL = UTF16ToMbUTF8 ( urlParts.spec.str );
    }
    else
    {
        // Get the file name (charsequence after last /)
        WString tempStr = urlParts.path.str;
        size_t pos = tempStr.find_last_of ( L"/" );

        if ( pos != std::wstring::npos && pos < tempStr.size () )
            convertedURL = UTF16ToMbUTF8 ( tempStr.SubStr ( pos + 1 ) );
        else
            convertedURL = "";
    }
}

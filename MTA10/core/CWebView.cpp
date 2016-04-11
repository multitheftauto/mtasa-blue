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
#include "CAjaxResourceHandler.h"
#include <cef3/include/cef_parser.h>
#include <cef3/include/cef_task.h>
#include <cef3/include/cef_runnable.h>
#include "CWebDevTools.h"

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent )
{
    m_bIsLocal = bIsLocal;
    m_bIsTransparent = bTransparent;
    m_pWebBrowserRenderItem = pWebBrowserRenderItem;
    m_pEventsInterface = nullptr;
    m_bBeingDestroyed = false;
    m_fVolume = 1.0f;
    memset ( m_mouseButtonStates, 0, sizeof(m_mouseButtonStates) );

    // Initialise properties
    m_Properties["mobile"] = "0";
}

CWebView::~CWebView ()
{
    if ( IsMainThread () )
    {
        if ( g_pCore->GetWebCore ()->GetFocusedWebView () == this )
            g_pCore->GetWebCore ()->SetFocusedWebView ( nullptr );
    }

    // Ensure that CefRefPtr::~CefRefPtr doesn't try to release it twice (it has already been released in CWebView::OnBeforeClose)
    m_pWebView = nullptr;

#ifdef MTA_DEBUG
    OutputDebugLine ( "CWebView::~CWebView" );
#endif
}

void CWebView::Initialise ()
{
    // Initialise the web session (which holds the actual settings) in in-memory mode
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = g_pCore->GetFrameRateLimit ();
    browserSettings.javascript_access_clipboard = cef_state_t::STATE_DISABLED;
    browserSettings.caret_browsing = cef_state_t::STATE_ENABLED;
    browserSettings.universal_access_from_file_urls = cef_state_t::STATE_DISABLED; // Also filtered by resource interceptor, but set this nevertheless
    browserSettings.file_access_from_file_urls = cef_state_t::STATE_DISABLED;
    browserSettings.webgl = cef_state_t::STATE_ENABLED;
    browserSettings.javascript_open_windows = cef_state_t::STATE_DISABLED;

    bool bEnabledPlugins = g_pCore->GetWebCore ()->GetPluginsEnabled ();
    browserSettings.plugins = bEnabledPlugins ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
    if ( !m_bIsLocal )
    {
        bool bEnabledJavascript = g_pCore->GetWebCore ()->GetRemoteJavascriptEnabled ();
        browserSettings.javascript = bEnabledJavascript ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
    }

    CefWindowInfo windowInfo;
    windowInfo.SetAsWindowless ( g_pCore->GetHookedWindow (), m_bIsTransparent );
    
    CefBrowserHost::CreateBrowser ( windowInfo, this, "", browserSettings, nullptr );
}

void CWebView::CloseBrowser ()
{
    // CefBrowserHost::CloseBrowser calls the destructor after the browser has been destroyed
    m_bBeingDestroyed = true;

    if ( m_pWebView )
        m_pWebView->GetHost ()->CloseBrowser ( true );
}

bool CWebView::LoadURL ( const SString& strURL, bool bFilterEnabled, const SString& strPostData, bool bURLEncoded )
{
    if ( !m_pWebView )
        return false;

    CefURLParts urlParts;
    if ( !CefParseURL ( strURL, urlParts ) )
        return false; // Invalid URL

    // Are we allowed to browse this website?
    if ( bFilterEnabled && g_pCore->GetWebCore ()->GetURLState ( UTF16ToMbUTF8 ( urlParts.host.str ), true ) != eURLState::WEBPAGE_ALLOWED )
        return false;

    // Load it!
    auto pFrame = m_pWebView->GetMainFrame ();
    if ( strPostData.empty () )
    {
        pFrame->LoadURL ( strURL );
    }
    else
    {
        // Load URL first, see https://bitbucket.org/chromiumembedded/cef/issue/579
        pFrame->LoadURL ( "about:blank" );

        // Perform HTTP POST
        auto request = CefRequest::Create ();
        auto postData = CefPostData::Create ();
        auto postDataElement = CefPostDataElement::Create ();
        postDataElement->SetToBytes ( strPostData.size (), strPostData.c_str () );
        postData->AddElement ( postDataElement );

        if ( bURLEncoded )
        {
            CefRequest::HeaderMap headerMap;
            headerMap.insert ( std::make_pair ( "Content-Type", "application/x-www-form-urlencoded" ) );
            headerMap.insert ( std::make_pair ( "Content-Length", std::to_string ( strPostData.size () ) ) );
            //headerMap.insert ( std::make_pair ( "Connection", "close" ) );
            request->SetHeaderMap ( headerMap );
        }

        request->SetURL ( strURL );
        request->SetMethod ( "POST" );
        request->SetPostData ( postData );
        pFrame->LoadRequest ( request );
    }
   
    return true;
}

bool CWebView::IsLoading ()
{
    if ( !m_pWebView )
        return false;

    return m_pWebView->IsLoading ();
}

SString CWebView::GetURL ()
{
    if ( !m_pWebView )
        return "";

    return UTF16ToMbUTF8 ( m_pWebView->GetMainFrame ()->GetURL () );
}

const SString& CWebView::GetTitle ()
{
    return m_CurrentTitle;
}

void CWebView::SetRenderingPaused ( bool bPaused )
{
    if ( m_pWebView )
        m_pWebView->GetHost ()->WasHidden ( bPaused );
}

void CWebView::Focus ( bool state )
{
    if ( m_pWebView )
    {
        m_pWebView->GetHost ()->SetFocus ( state );
        m_pWebView->GetHost ()->SendFocusEvent ( state );
    }

    if ( state )
        g_pCore->GetWebCore ()->SetFocusedWebView ( this );
    else if ( g_pCore->GetWebCore ()->GetFocusedWebView () == this )
        g_pCore->GetWebCore ()->SetFocusedWebView ( nullptr );
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
    if ( m_pWebView )
        m_pWebView->GetMainFrame ()->ExecuteJavaScript ( strJavascriptCode, "", 0 );
}

bool CWebView::SetProperty ( const SString& strKey, const SString& strValue )
{
    if ( strKey == "mobile" && ( strValue == "0" || strValue == "1" ) )
    {
        
    }
    else
        return false;

    m_Properties[strKey] = strValue;
    return true;
}

bool CWebView::GetProperty ( const SString& strKey, SString& outValue )
{
    auto iter = m_Properties.find ( strKey );
    if ( iter == m_Properties.end () )
        return false;

    outValue = iter->second;
    return true;
}

void CWebView::InjectMouseMove ( int iPosX, int iPosY )
{
    if ( !m_pWebView )
        return;
    
    CefMouseEvent mouseEvent;
    mouseEvent.x = iPosX;
    mouseEvent.y = iPosY;

    // Set modifiers from mouse states (yeah, using enum values as indices isn't best practise, but it's the easiest solution here)
    if ( m_mouseButtonStates[BROWSER_MOUSEBUTTON_LEFT] )
        mouseEvent.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    if ( m_mouseButtonStates[BROWSER_MOUSEBUTTON_MIDDLE] )
        mouseEvent.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    if ( m_mouseButtonStates[BROWSER_MOUSEBUTTON_RIGHT] )
        mouseEvent.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

    m_pWebView->GetHost ()->SendMouseMoveEvent ( mouseEvent, false );

    m_vecMousePosition.x = iPosX;
    m_vecMousePosition.y = iPosY;
}

void CWebView::InjectMouseDown ( eWebBrowserMouseButton mouseButton )
{
    if ( !m_pWebView )
        return;
    
    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    // Save mouse button states
    m_mouseButtonStates[static_cast<int>(mouseButton)] = true;

    m_pWebView->GetHost ()->SendMouseClickEvent ( mouseEvent, static_cast < CefBrowserHost::MouseButtonType > ( mouseButton ), false, 1 );
}

void CWebView::InjectMouseUp ( eWebBrowserMouseButton mouseButton )
{
    if ( !m_pWebView )
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    // Save mouse button states
    m_mouseButtonStates[static_cast<int>(mouseButton)] = false;

    m_pWebView->GetHost ()->SendMouseClickEvent ( mouseEvent, static_cast < CefBrowserHost::MouseButtonType > ( mouseButton ), true, 1 );
}

void CWebView::InjectMouseWheel ( int iScrollVert, int iScrollHorz )
{
    if ( !m_pWebView )
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    m_pWebView->GetHost ()->SendMouseWheelEvent ( mouseEvent, iScrollHorz, iScrollVert );
}

void CWebView::InjectKeyboardEvent ( const CefKeyEvent& keyEvent )
{
    if ( m_pWebView )
        m_pWebView->GetHost ()->SendKeyEvent ( keyEvent );
}

bool CWebView::SetAudioVolume ( float fVolume )
{
    // NOTE: Keep this function thread-safe
    if ( !m_pWebView || fVolume < 0.0f || fVolume > 1.0f )
        return false;

    // Since the necessary interfaces of the core audio API were introduced in Win7, we've to fallback to HTML5 audio
    SString strJSCode ( "var tags = document.getElementsByTagName('audio'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;} " \
        "tags = document.getElementsByTagName('video'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;}",
        fVolume, fVolume );

    m_pWebView->GetMainFrame ()->ExecuteJavaScript ( strJSCode, "", 0 );
    m_fVolume = fVolume;
    return true;
}

void CWebView::GetSourceCode ( const std::function<void( const std::string& code )>& callback )
{
    class MyStringVisitor : public CefStringVisitor
    {
    private:
        CWebView* webView;
        std::function<void( const std::string& )> callback;

    public:
        MyStringVisitor ( CWebView* webView_, const std::function<void( const std::string& )>& callback_ ) : webView(webView_), callback(callback_) {}

        virtual void Visit ( const CefString& code ) override
        {
            // Limit to 2MiB for now to prevent freezes (TODO: Optimize that and increase later)
            if ( code.size () <= 2097152 )
            {
                // Call callback on main thread
                g_pCore->GetWebCore ()->AddEventToEventQueue ( std::bind ( callback, code ), webView, "GetSourceCode_Visit" );
            }
        }

        IMPLEMENT_REFCOUNTING(MyStringVisitor);
    };

    CefRefPtr<CefStringVisitor> visitor { new MyStringVisitor ( this, callback ) };
    m_pWebView->GetMainFrame ()->GetSource ( visitor );
}

bool CWebView::GetFullPathFromLocal ( SString& strPath )
{
     return m_pEventsInterface->Events_OnResourcePathCheck ( strPath );
}

bool CWebView::RegisterAjaxHandler ( const SString& strURL )
{
    auto result = m_AjaxHandlers.insert ( strURL );
    return result.second;
}

bool CWebView::UnregisterAjaxHandler ( const SString& strURL )
{
    return m_AjaxHandlers.erase ( strURL ) == 1;
}


bool CWebView::HasAjaxHandler ( const SString& strURL )
{
    auto iterCB = m_AjaxHandlers.find ( strURL );
    return iterCB != m_AjaxHandlers.end ();
}

void CWebView::HandleAjaxRequest ( const SString& strURL, CAjaxResourceHandler * pHandler )
{
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnAjaxRequest, m_pEventsInterface, pHandler, strURL );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "AjaxResourceRequest" );
}

bool CWebView::ToggleDevTools ( bool visible )
{
    if ( visible )
        return CWebDevTools::Show ( this );

    return CWebDevTools::Close ( this );
}

bool CWebView::VerifyFile ( const SString& strPath )
{
    return m_pEventsInterface->Events_OnResourceFileCheck ( strPath );
}

bool CWebView::CanGoBack ()
{
    if ( !m_pWebView )
        return false;

    return m_pWebView->CanGoBack ();
}

bool CWebView::CanGoForward ()
{
    if ( !m_pWebView )
        return false;

    return m_pWebView->CanGoForward ();
}

bool CWebView::GoBack ()
{
    if ( !m_pWebView )
        return false;

    if ( !m_pWebView->CanGoBack () )
        return false;

    m_pWebView->GoBack ();
    return true;
}

bool CWebView::GoForward ()
{
    if ( !m_pWebView )
        return false;

    if ( !m_pWebView->CanGoForward () )
        return false;

    m_pWebView->GoForward ();
    return true;
}

void CWebView::Refresh ( bool bIgnoreCache )
{
    if ( !m_pWebView )
        return;

    if ( bIgnoreCache )
    {
        m_pWebView->ReloadIgnoreCache ();
    }
    else
    {
        m_pWebView->Reload ();
    }
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
    if ( message->GetName () == "TriggerLuaEvent" )
    {
        if ( !m_bIsLocal )
            return true;

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

        // Queue event to run on the main thread
        auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnTriggerEvent, m_pEventsInterface, SString ( eventName ), args );
        g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnProcessMessageReceived1" );

        // The message was handled
        return true;
    }
    if ( message->GetName () == "InputFocus" )
    {
        // Retrieve arguments from process message
        m_bHasInputFocus = argList->GetBool ( 0 );

        // Queue event to run on the main thread
        auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnInputFocusChanged, m_pEventsInterface, m_bHasInputFocus );
        g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnProcessMessageReceived2" );
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
// Implementation: CefRenderHandler::OnPopupSize                  //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPopupSize(CefRefPtr<CefBrowser>,constCefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPopupSize ( CefRefPtr<CefBrowser> browser, const CefRect& rect )
{
    m_RenderPopupOffsetX = rect.x;
    m_RenderPopupOffsetY = rect.y;
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

    /*
        DirectX9 is not thread safe, so locking the texture from another thread causes D3DDevice::Present to block forever (-> client freeze)
        The only way we can get rid of this problem is (imo) locking CEF until the texture has been written (unlocking/notifying is done in CWebCore::DoEventQueuePulse).
        If someone knows a better solution than this weird construction, feel free to contact me (Jusonex).
    */

    auto f = [&]() {
        if ( m_bBeingDestroyed )
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
        int theoreticalPitch = width * 4;
        if ( LockedRect.Pitch == theoreticalPitch )
            memcpy ( LockedRect.pBits, buffer, theoreticalPitch * height );
        else
        {
            // We've to skip a few rows if this is a popup draw
            // The pitch will never equal the theoretical pitch (see above) by definition
            int skipRows = 0;
            int skipPixels = 0;
            if ( paintType == PaintElementType::PET_POPUP )
            {
                skipRows = m_RenderPopupOffsetY;
                skipPixels = m_RenderPopupOffsetX;
            }

            uint32 destAddress = (uint32)LockedRect.pBits + skipPixels * 4;
            for ( int i = 0; i < height; ++i )
            {
                memcpy ( (void*)(destAddress + LockedRect.Pitch * (i + skipRows)), (void*)((uint32)buffer + theoreticalPitch * i), theoreticalPitch );
            }
        }

        pD3DSurface->UnlockRect ();
    };
    g_pCore->GetWebCore ()->AddEventToEventQueue ( f, this, "OnPaint" );

    std::unique_lock<std::mutex> lock ( m_PaintMutex );
    m_PaintCV.wait ( lock );
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

    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnChangeCursor, m_pEventsInterface, cursorIndex );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnCursorChange" );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadStart                    //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadStart(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadStart ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame )
{
    SString strURL = UTF16ToMbUTF8 ( frame->GetURL () );
    if ( strURL == "blank" )
        return;

    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnLoadingStart, m_pEventsInterface, strURL, frame->IsMain () );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnLoadStart" );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadEnd                      //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadEnd(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,int) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadEnd ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode )
{
    // Set browser volume once again
    SetAudioVolume ( m_fVolume );

    if ( frame->IsMain () )
    {
	    SString strURL = UTF16ToMbUTF8 ( frame->GetURL () );

        // Queue event to run on the main thread
        auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnDocumentReady, m_pEventsInterface, strURL );
        g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnLoadEnd" );
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
    SString strURL = UTF16ToMbUTF8 ( frame->GetURL () );
    
    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnLoadingFailed, m_pEventsInterface, strURL, errorCode, SString ( errorText ) );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnLoadError" );
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
    
    bool bResult;
    WString scheme = urlParts.scheme.str;
    if ( scheme == L"http" || scheme == L"https" )
    {
        SString host = UTF16ToMbUTF8 ( urlParts.host.str );
        if ( host != "mta" )
        {
            if ( IsLocal () || g_pCore->GetWebCore ()->GetURLState ( host, true ) != eURLState::WEBPAGE_ALLOWED )
                bResult = true; // Block remote here
            else
                bResult = false; // Allow
        }
        else
            bResult = false;
    }
    else if ( scheme == L"mtalocal" )
        bResult = false; // Allow mtalocal:// URLs
    else
        bResult = true; // Block other schemes

    // Check if we're in the browser's main frame or only a frame element of the current page
    bool bIsMainFrame = frame->IsMain ();

    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnNavigate, m_pEventsInterface, SString ( request->GetURL () ), bResult, bIsMainFrame );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnNavigate" );

    // Return execution to CEF
    return bResult;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRequestHandler::OnBeforeResourceLoad        //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeResourceLoad(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
CefRequestHandler::ReturnValue CWebView::OnBeforeResourceLoad ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback )
{
    // Mostly the same as CWebView::OnBeforeBrowse
    CefURLParts urlParts;
    if ( !CefParseURL ( request->GetURL (), urlParts ) )
        return RV_CANCEL; // Cancel if invalid URL (this line will normally not be executed)

    // Add some information to the HTTP header
    {
        CefRequest::HeaderMap headerMap;
        request->GetHeaderMap ( headerMap );
        auto iter = headerMap.find ( "User-Agent" );

        if ( iter != headerMap.end () )
        {
            // Add MTA:SA "watermark"
            iter->second = iter->second.ToString () + "; " MTA_CEF_USERAGENT;

            // Add 'Android' to get the mobile version
            SString strPropertyValue;
            if ( GetProperty ( "mobile", strPropertyValue ) && strPropertyValue == "1" )
                iter->second = iter->second.ToString () + "; Mobile Android";

            request->SetHeaderMap ( headerMap );
        }
    }

    WString scheme = urlParts.scheme.str;
    if ( scheme == L"http" || scheme == L"https" )
    {
        SString domain = UTF16ToMbUTF8 ( urlParts.host.str );
        if ( domain != "mta" )
        {
            if ( IsLocal () )
                return RV_CANCEL; // Block remote requests in local mode generally

            eURLState urlState = g_pCore->GetWebCore ()->GetURLState ( domain, true );
            if ( urlState != eURLState::WEBPAGE_ALLOWED )
            {
                // Trigger onClientBrowserResourceBlocked event
                auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnResourceBlocked, m_pEventsInterface,
                    SString ( request->GetURL () ), domain, urlState == eURLState::WEBPAGE_NOT_LISTED ? 0 : 1 );
                g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnResourceBlocked" );

                return RV_CANCEL; // Block if explicitly forbidden
            }

            // Allow
            return RV_CONTINUE;
        }
        else
            return RV_CONTINUE;
    }
    else if ( scheme == L"mtalocal" )
    {
        // Allow :)
        return RV_CONTINUE;
    }

     // Trigger onClientBrowserResourceBlocked event
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnResourceBlocked, m_pEventsInterface, SString ( request->GetURL () ), "", 2 ); // reason 1 := blocked protocol scheme
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnResourceBlocked" );

    // Block everything else
    return RV_CANCEL;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforeClose              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforeClose(CefRefPtr%3CCefBrowser%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeforeClose ( CefRefPtr<CefBrowser> browser )
{
    // Remove events owned by this webview and invoke left callbacks
    g_pCore->GetWebCore ()->RemoveWebViewEvents ( this );
    NotifyPaint ();

    m_pWebView = nullptr;
    
    // Remove focused web view reference
    if ( g_pCore->GetWebCore ()->GetFocusedWebView () == this )
        g_pCore->GetWebCore ()->SetFocusedWebView ( nullptr );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforePopup              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforePopup(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,constCefString&,constCefString&,constCefPopupFeatures&,CefWindowInfo&,CefRefPtr%3CCefClient%3E&,CefBrowserSettings&,bool*) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforePopup ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access )
{
    // ATTENTION: This method is called on the IO thread
    
    // Trigger the popup/new tab event
    SString strTagetURL = UTF16ToMbUTF8 ( target_url );
    SString strOpenerURL = UTF16ToMbUTF8 ( frame->GetURL () );
    
    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnPopup, m_pEventsInterface, strTagetURL, strOpenerURL );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnBeforePopup" );

    // Block popups generally
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnAfterCreated             //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnAfterCreated(CefRefPtr<CefBrowser>) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnAfterCreated ( CefRefPtr<CefBrowser> browser )
{
    // Set web view reference
    m_pWebView = browser;

    // Call created event callback
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnCreated, m_pEventsInterface );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnAfterCreated" );
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
bool CWebView::OnFileDialog ( CefRefPtr<CefBrowser> browser, CefDialogHandler::FileDialogMode mode, const CefString& title, const CefString& default_file_name, const std::vector< CefString >& accept_types, int selected_accept_filter, CefRefPtr< CefFileDialogCallback > callback )
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
    // Queue event to run on the main thread
    auto func = std::bind ( &CWebBrowserEventsInterface::Events_OnTooltip, m_pEventsInterface, UTF16ToMbUTF8 ( title ) );
    g_pCore->GetWebCore ()->AddEventToEventQueue ( func, this, "OnTooltip" );
    
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
        g_pCore->GetWebCore ()->AddEventToEventQueue ( [message, source]() { 
            g_pCore->DebugPrintfColor ( "[BROWSER] Console: %s (%s)", 255, 0, 0, UTF16ToMbUTF8 ( message ).c_str (), UTF16ToMbUTF8 ( source ).c_str () ); 
        }, this, "OnConsoleMessage" );
        
    }

    return true;
}

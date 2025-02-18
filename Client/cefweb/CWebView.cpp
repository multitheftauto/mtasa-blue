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
#include <cef3/cef/include/cef_parser.h>
#include <cef3/cef/include/cef_task.h>
#include "CWebDevTools.h"

namespace
{
    const int CEF_PIXEL_STRIDE = 4;
}

CWebView::CWebView(bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent)
{
    m_bIsLocal = bIsLocal;
    m_bIsTransparent = bTransparent;
    m_pWebBrowserRenderItem = pWebBrowserRenderItem;
    m_pEventsInterface = nullptr;
    m_bBeingDestroyed = false;
    m_fVolume = 1.0f;
    memset(m_mouseButtonStates, 0, sizeof(m_mouseButtonStates));

    // Initialise properties
    m_Properties["mobile"] = "0";
}

CWebView::~CWebView()
{
    if (IsMainThread())
    {
        if (g_pCore->GetWebCore()->GetFocusedWebView() == this)
            g_pCore->GetWebCore()->SetFocusedWebView(nullptr);
    }

    // Make sure we don't dead lock the CEF render thread
    ResumeCefThread();

    // Ensure that CefRefPtr::~CefRefPtr doesn't try to release it twice (it has already been released in CWebView::OnBeforeClose)
    m_pWebView = nullptr;

    OutputDebugLine("CWebView::~CWebView");
}

void CWebView::Initialise()
{
    // Initialise the web session (which holds the actual settings) in in-memory mode
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = g_pCore->GetFrameRateLimit();
    browserSettings.javascript_access_clipboard = cef_state_t::STATE_DISABLED;
    browserSettings.javascript_dom_paste = cef_state_t::STATE_DISABLED;
    browserSettings.webgl = cef_state_t::STATE_ENABLED;

    if (!m_bIsLocal)
    {
        bool bEnabledJavascript = g_pCore->GetWebCore()->GetRemoteJavascriptEnabled();
        browserSettings.javascript = bEnabledJavascript ? cef_state_t::STATE_ENABLED : cef_state_t::STATE_DISABLED;
    }

    // Set background color to opaque white if transparency is disabled
    if (!m_bIsTransparent)
        browserSettings.background_color = 0xffffffff;

    CefWindowInfo windowInfo;
    windowInfo.SetAsWindowless(g_pCore->GetHookedWindow());

    CefBrowserHost::CreateBrowser(windowInfo, this, "", browserSettings, nullptr, nullptr);
}

void CWebView::CloseBrowser()
{
    // CefBrowserHost::CloseBrowser calls the destructor after the browser has been destroyed
    m_bBeingDestroyed = true;

    // Make sure we don't dead lock the CEF render thread
    ResumeCefThread();

    if (m_pWebView)
        m_pWebView->GetHost()->CloseBrowser(true);
}

bool CWebView::LoadURL(const SString& strURL, bool bFilterEnabled, const SString& strPostData, bool bURLEncoded)
{
    if (!m_pWebView)
        return false;

    CefURLParts urlParts;
    if (strURL.empty() || !CefParseURL(strURL, urlParts))
        return false;            // Invalid URL

    // Are we allowed to browse this website?
    if (bFilterEnabled && g_pCore->GetWebCore()->GetDomainState(UTF16ToMbUTF8(urlParts.host.str), true) != eURLState::WEBPAGE_ALLOWED)
        return false;

    // Load it!
    auto pFrame = m_pWebView->GetMainFrame();
    if (strPostData.empty())
    {
        pFrame->LoadURL(strURL);
    }
    else
    {
        // Load URL first, see https://bitbucket.org/chromiumembedded/cef/issue/579
        pFrame->LoadURL("about:blank");

        // Perform HTTP POST
        auto request = CefRequest::Create();
        auto postData = CefPostData::Create();
        auto postDataElement = CefPostDataElement::Create();
        postDataElement->SetToBytes(strPostData.size(), strPostData.c_str());
        postData->AddElement(postDataElement);

        if (bURLEncoded)
        {
            CefRequest::HeaderMap headerMap;
            headerMap.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
            headerMap.insert(std::make_pair("Content-Length", std::to_string(strPostData.size())));
            // headerMap.insert ( std::make_pair ( "Connection", "close" ) );
            request->SetHeaderMap(headerMap);
        }

        request->SetURL(strURL);
        request->SetMethod("POST");
        request->SetPostData(postData);
        pFrame->LoadRequest(request);
    }

    return true;
}

bool CWebView::IsLoading()
{
    if (!m_pWebView)
        return false;

    return m_pWebView->IsLoading();
}

SString CWebView::GetURL()
{
    if (!m_pWebView)
        return "";

    return UTF16ToMbUTF8(m_pWebView->GetMainFrame()->GetURL());
}

const SString& CWebView::GetTitle()
{
    return m_CurrentTitle;
}

void CWebView::SetRenderingPaused(bool bPaused)
{
    if (m_pWebView)
    {
        m_pWebView->GetHost()->WasHidden(bPaused);
        m_bIsRenderingPaused = bPaused;
    }
}

const bool CWebView::GetRenderingPaused() const
{
    return m_pWebView ? m_bIsRenderingPaused : false;
}

void CWebView::Focus(bool state)
{
    if (m_pWebView)
        m_pWebView->GetHost()->SetFocus(state);

    if (state)
        g_pCore->GetWebCore()->SetFocusedWebView(this);
    else if (g_pCore->GetWebCore()->GetFocusedWebView() == this)
        g_pCore->GetWebCore()->SetFocusedWebView(nullptr);
}

void CWebView::ClearTexture()
{
    IDirect3DSurface9* pD3DSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if (!pD3DSurface)
        return;

    D3DSURFACE_DESC SurfaceDesc;
    if (FAILED(pD3DSurface->GetDesc(&SurfaceDesc)))
        return;

    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(pD3DSurface->LockRect(&LockedRect, NULL, D3DLOCK_DISCARD)))
    {
        memset(LockedRect.pBits, 0xFF, SurfaceDesc.Height * LockedRect.Pitch);
        pD3DSurface->UnlockRect();
    }
}

void CWebView::UpdateTexture()
{
    std::lock_guard<std::mutex> lock(m_RenderData.dataMutex);

    auto pSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if (m_bBeingDestroyed || !pSurface)
        m_RenderData.changed = m_RenderData.popupShown = false;

    // Discard current buffer if size doesn't match
    // This happens when resizing the browser as OnPaint is called asynchronously
    if (m_RenderData.changed && (m_pWebBrowserRenderItem->m_uiSizeX != m_RenderData.width || m_pWebBrowserRenderItem->m_uiSizeY != m_RenderData.height))
        m_RenderData.changed = false;

    if (m_RenderData.changed || m_RenderData.popupShown)
    {
        // Lock surface
        D3DLOCKED_RECT LockedRect;
        if (SUCCEEDED(pSurface->LockRect(&LockedRect, nullptr, 0)))
        {
            // Dirty rect implementation, don't use this as loops are significantly slower than memcpy
            const auto destData = static_cast<byte*>(LockedRect.pBits);
            const auto sourceData = static_cast<const byte*>(m_RenderData.buffer);
            const auto destPitch = LockedRect.Pitch;
            const auto sourcePitch = m_RenderData.width * CEF_PIXEL_STRIDE;

            // Update view area
            if (m_RenderData.changed)
            {
                // Update changed state
                m_RenderData.changed = false;

                if (m_RenderData.dirtyRects.size() > 0 && m_RenderData.dirtyRects[0].width == m_RenderData.width &&
                    m_RenderData.dirtyRects[0].height == m_RenderData.height)
                {
                    // Note that D3D texture size can be hardware dependent(especially with dynamic texture)
                    // When destination and source pitches differ we must copy pixels row by row
                    if (destPitch == sourcePitch)
                        memcpy(destData, sourceData, destPitch * m_RenderData.height);
                    else
                    {
                        for (int y = 0; y < m_RenderData.height; ++y)
                        {
                            const int sourceIndex = y * sourcePitch;
                            const int destIndex = y * destPitch;

                            memcpy(&destData[destIndex], &sourceData[sourceIndex], std::min(sourcePitch, destPitch));
                        }
                    }
                }
                else
                {
                    // Update dirty rects
                    for (const auto& rect : m_RenderData.dirtyRects)
                    {
                        for (int y = rect.y; y < rect.y + rect.height; ++y)
                        {
                            // Note that D3D texture size can be hardware dependent(especially with dynamic texture)
                            // We cannot be sure that source and destination pitches are the same
                            const int sourceIndex = y * sourcePitch + rect.x * CEF_PIXEL_STRIDE;
                            const int destIndex = y * destPitch + rect.x * CEF_PIXEL_STRIDE;

                            memcpy(&destData[destIndex], &sourceData[sourceIndex], rect.width * CEF_PIXEL_STRIDE);
                        }
                    }
                }
            }

            // Update popup area (override certain areas of the view texture)
            const bool popupSizeMismatches = m_RenderData.popupRect.x + m_RenderData.popupRect.width >= (int)m_pWebBrowserRenderItem->m_uiSizeX ||
                                             m_RenderData.popupRect.y + m_RenderData.popupRect.height >= (int)m_pWebBrowserRenderItem->m_uiSizeY;

            if (m_RenderData.popupShown && !popupSizeMismatches)
            {
                const auto popupPitch = m_RenderData.popupRect.width * CEF_PIXEL_STRIDE;
                for (int y = 0; y < m_RenderData.popupRect.height; ++y)
                {
                    const int sourceIndex = y * popupPitch;
                    const int destIndex = (y + m_RenderData.popupRect.y) * destPitch + m_RenderData.popupRect.x * CEF_PIXEL_STRIDE;

                    memcpy(&destData[destIndex], &m_RenderData.popupBuffer[sourceIndex], popupPitch);
                }
            }

            // Unlock surface
            pSurface->UnlockRect();
        }
    }

    m_RenderData.cefThreadState = ECefThreadState::Running;
    m_RenderData.cefThreadCv.notify_all();
}

void CWebView::ExecuteJavascript(const SString& strJavascriptCode)
{
    if (m_pWebView)
        m_pWebView->GetMainFrame()->ExecuteJavaScript(strJavascriptCode, "", 0);
}

bool CWebView::SetProperty(const SString& strKey, const SString& strValue)
{
    if (strKey == "mobile" && (strValue == "0" || strValue == "1"))
    {
    }
    else
        return false;

    m_Properties[strKey] = strValue;
    return true;
}

bool CWebView::GetProperty(const SString& strKey, SString& outValue)
{
    auto iter = m_Properties.find(strKey);
    if (iter == m_Properties.end())
        return false;

    outValue = iter->second;
    return true;
}

void CWebView::InjectMouseMove(int iPosX, int iPosY)
{
    if (!m_pWebView)
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = iPosX;
    mouseEvent.y = iPosY;

    // Set modifiers from mouse states (yeah, using enum values as indices isn't best practise, but it's the easiest solution here)
    if (m_mouseButtonStates[BROWSER_MOUSEBUTTON_LEFT])
        mouseEvent.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (m_mouseButtonStates[BROWSER_MOUSEBUTTON_MIDDLE])
        mouseEvent.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    if (m_mouseButtonStates[BROWSER_MOUSEBUTTON_RIGHT])
        mouseEvent.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

    m_pWebView->GetHost()->SendMouseMoveEvent(mouseEvent, false);

    m_vecMousePosition.x = iPosX;
    m_vecMousePosition.y = iPosY;
}

void CWebView::InjectMouseDown(eWebBrowserMouseButton mouseButton, int count)
{
    if (!m_pWebView)
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    // Save mouse button states
    m_mouseButtonStates[static_cast<int>(mouseButton)] = true;

    m_pWebView->GetHost()->SendMouseClickEvent(mouseEvent, static_cast<CefBrowserHost::MouseButtonType>(mouseButton), false, count);
}

void CWebView::InjectMouseUp(eWebBrowserMouseButton mouseButton)
{
    if (!m_pWebView)
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    // Save mouse button states
    m_mouseButtonStates[static_cast<int>(mouseButton)] = false;

    m_pWebView->GetHost()->SendMouseClickEvent(mouseEvent, static_cast<CefBrowserHost::MouseButtonType>(mouseButton), true, 1);
}

void CWebView::InjectMouseWheel(int iScrollVert, int iScrollHorz)
{
    if (!m_pWebView)
        return;

    CefMouseEvent mouseEvent;
    mouseEvent.x = m_vecMousePosition.x;
    mouseEvent.y = m_vecMousePosition.y;

    m_pWebView->GetHost()->SendMouseWheelEvent(mouseEvent, iScrollHorz, iScrollVert);
}

void CWebView::InjectKeyboardEvent(const CefKeyEvent& keyEvent)
{
    if (m_pWebView)
        m_pWebView->GetHost()->SendKeyEvent(keyEvent);
}

bool CWebView::SetAudioVolume(float fVolume)
{
    // NOTE: Keep this function thread-safe
    if (!m_pWebView || fVolume < 0.0f || fVolume > 1.0f)
        return false;

    // Since the necessary interfaces of the core audio API were introduced in Win7, we've to fallback to HTML5 audio
    SString strJSCode(
        "function mta_adjustAudioVol(elem, vol) { elem.volume = vol; elem.onvolumechange = function() { if (Math.abs(elem.volume - vol) >= 0.001) elem.volume "
        "= vol; } }"
        "var tags = document.getElementsByTagName('audio'); for (var i = 0; i<tags.length; ++i) { mta_adjustAudioVol(tags[i], %f); }"
        "tags = document.getElementsByTagName('video'); for (var i = 0; i<tags.length; ++i) { mta_adjustAudioVol(tags[i], %f); }",
        fVolume, fVolume);

    std::vector<CefString> frameNames;
    m_pWebView->GetFrameNames(frameNames);

    for (auto& name : frameNames)
    {
        auto frame = m_pWebView->GetFrameByName(name);
        frame->ExecuteJavaScript(strJSCode, "", 0);
    }
    m_fVolume = fVolume;
    return true;
}

void CWebView::GetSourceCode(const std::function<void(const std::string& code)>& callback)
{
    if (!m_pWebView)
        return;

    class MyStringVisitor : public CefStringVisitor
    {
    private:
        CWebView*                               webView;
        std::function<void(const std::string&)> callback;

    public:
        MyStringVisitor(CWebView* webView_, const std::function<void(const std::string&)>& callback_) : webView(webView_), callback(callback_) {}

        virtual void Visit(const CefString& code) override
        {
            // Limit to 2MiB for now to prevent freezes (TODO: Optimize that and increase later)
            if (code.size() <= 2097152)
            {
                // Call callback on main thread
                g_pCore->GetWebCore()->AddEventToEventQueue(std::bind(callback, code), webView, "GetSourceCode_Visit");
            }
        }

        IMPLEMENT_REFCOUNTING(MyStringVisitor);
    };

    CefRefPtr<CefStringVisitor> visitor{new MyStringVisitor(this, callback)};
    m_pWebView->GetMainFrame()->GetSource(visitor);
}

void CWebView::Resize(const CVector2D& size)
{
    // Resize underlying texture
    m_pWebBrowserRenderItem->Resize(size);

    // Send resize event to CEF
    if (m_pWebView)
        m_pWebView->GetHost()->WasResized();

    ResumeCefThread();
}

CVector2D CWebView::GetSize()
{
    return CVector2D(static_cast<float>(m_pWebBrowserRenderItem->m_uiSizeX), static_cast<float>(m_pWebBrowserRenderItem->m_uiSizeY));
}

bool CWebView::GetFullPathFromLocal(SString& strPath)
{
    bool result = false;

    g_pCore->GetWebCore()->WaitForTask(
        [&](bool aborted) {
            if (aborted)
                return;

            result = m_pEventsInterface->Events_OnResourcePathCheck(strPath);
        },
        this);

    return result;
}

bool CWebView::RegisterAjaxHandler(const SString& strURL)
{
    auto result = m_AjaxHandlers.insert(strURL);
    return result.second;
}

bool CWebView::UnregisterAjaxHandler(const SString& strURL)
{
    return m_AjaxHandlers.erase(strURL) == 1;
}

bool CWebView::HasAjaxHandler(const SString& strURL)
{
    auto iterCB = m_AjaxHandlers.find(strURL);
    return iterCB != m_AjaxHandlers.end();
}

void CWebView::HandleAjaxRequest(const SString& strURL, CAjaxResourceHandler* pHandler)
{
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnAjaxRequest, m_pEventsInterface, pHandler, strURL);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "AjaxResourceRequest");
}

bool CWebView::ToggleDevTools(bool visible)
{
    if (visible)
        return CWebDevTools::Show(this);

    return CWebDevTools::Close(this);
}

bool CWebView::VerifyFile(const SString& strPath, CBuffer& outFileData)
{
    bool result = false;

    g_pCore->GetWebCore()->WaitForTask(
        [&](bool aborted) {
            if (aborted)
                return;

            result = m_pEventsInterface->Events_OnResourceFileCheck(strPath, outFileData);
        },
        this);

    return result;
}

bool CWebView::CanGoBack()
{
    if (!m_pWebView)
        return false;

    return m_pWebView->CanGoBack();
}

bool CWebView::CanGoForward()
{
    if (!m_pWebView)
        return false;

    return m_pWebView->CanGoForward();
}

bool CWebView::GoBack()
{
    if (!m_pWebView)
        return false;

    if (!m_pWebView->CanGoBack())
        return false;

    m_pWebView->GoBack();
    return true;
}

bool CWebView::GoForward()
{
    if (!m_pWebView)
        return false;

    if (!m_pWebView->CanGoForward())
        return false;

    m_pWebView->GoForward();
    return true;
}

void CWebView::Refresh(bool bIgnoreCache)
{
    if (!m_pWebView)
        return;

    if (bIgnoreCache)
    {
        m_pWebView->ReloadIgnoreCache();
    }
    else
    {
        m_pWebView->Reload();
    }
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefClient::OnProcessMessageReceived            //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html#OnProcessMessageReceived(CefRefPtr%3CCefBrowser%3E,CefProcessId,CefRefPtr%3CCefProcessMessage%3E)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message)
{
    CefRefPtr<CefListValue> argList = message->GetArgumentList();
    if (message->GetName() == "TriggerLuaEvent")
    {
        if (!m_bIsLocal)
            return true;

        // Get event name
        CefString eventName = argList->GetString(0);

        // Get number of arguments from IPC process message
        int numArgs = argList->GetInt(1);

        // Get args
        std::vector<std::string> args;
        for (int i = 2; i < numArgs + 2; ++i)
        {
            args.push_back(argList->GetString(i));
        }

        // Queue event to run on the main thread
        auto func = std::bind(&CWebBrowserEventsInterface::Events_OnTriggerEvent, m_pEventsInterface, SString(eventName), args);
        g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnProcessMessageReceived1");

        // The message was handled
        return true;
    }
    if (message->GetName() == "InputFocus")
    {
        // Retrieve arguments from process message
        m_bHasInputFocus = argList->GetBool(0);

        // Queue event to run on the main thread
        auto func = std::bind(&CWebBrowserEventsInterface::Events_OnInputFocusChanged, m_pEventsInterface, m_bHasInputFocus);
        g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnProcessMessageReceived2");
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
void CWebView::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;

    if (m_bBeingDestroyed)
    {
        rect.width = 1;
        rect.height = 1;
        return;
    }

    rect.width = static_cast<int>(m_pWebBrowserRenderItem->m_uiSizeX);
    rect.height = static_cast<int>(m_pWebBrowserRenderItem->m_uiSizeY);
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnPopupShow                  //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPopupShow(CefRefPtr<CefBrowser>,bool) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    std::lock_guard<std::mutex> lock{m_RenderData.dataMutex};
    m_RenderData.popupShown = show;

    // Free popup buffer memory if hidden
    if (!show)
        m_RenderData.popupBuffer.reset();
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnPopupSize                  //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPopupSize(CefRefPtr<CefBrowser>,constCefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    std::lock_guard<std::mutex> lock{m_RenderData.dataMutex};

    // Update rect
    m_RenderData.popupRect = rect;

    // Resize buffer
    m_RenderData.popupBuffer.reset(new byte[rect.width * rect.height * CEF_PIXEL_STRIDE]);
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnPaint                      //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPaint(CefRefPtr%3CCefBrowser%3E,PaintElementType,constRectList&,constvoid*,int,int)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer, int width, int height)
{
    if (m_bBeingDestroyed)
        return;

    std::unique_lock<std::mutex> lock(m_RenderData.dataMutex);

    // Copy popup buffer
    if (paintType == PET_POPUP)
    {
        if (m_RenderData.popupBuffer)
        {
            memcpy(m_RenderData.popupBuffer.get(), buffer, width * height * CEF_PIXEL_STRIDE);
        }

        return;            // We don't have to wait as we've copied the buffer already
    }

    // Store render data
    m_RenderData.buffer = buffer;
    m_RenderData.width = width;
    m_RenderData.height = height;
    m_RenderData.dirtyRects = dirtyRects;
    m_RenderData.changed = true;

    // Wait for the main thread to handle drawing the texture
    m_RenderData.cefThreadState = ECefThreadState::Wait;
    m_RenderData.cefThreadCv.wait(lock, [&]() { return m_RenderData.cefThreadState == ECefThreadState::Running; });
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadStart                    //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadStart(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType)
{
    SString strURL = UTF16ToMbUTF8(frame->GetURL());
    if (strURL == "blank")
        return;

    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnLoadingStart, m_pEventsInterface, strURL, frame->IsMain());
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnLoadStart");
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadEnd                      //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadEnd(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,int) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    // Set browser volume once again
    SetAudioVolume(m_fVolume);

    if (frame->IsMain())
    {
        SString strURL = UTF16ToMbUTF8(frame->GetURL());

        // Queue event to run on the main thread
        auto func = std::bind(&CWebBrowserEventsInterface::Events_OnDocumentReady, m_pEventsInterface, strURL);
        g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnLoadEnd");
    }
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadError                    //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadError(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,ErrorCode,constCefString&,constCefString&)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText,
                           const CefString& failedURL)
{
    SString strURL = UTF16ToMbUTF8(frame->GetURL());

    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnLoadingFailed, m_pEventsInterface, strURL, errorCode, SString(errorText));
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnLoadError");
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRequestHandler::OnBeforeBrowe               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeBrowse(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E,bool)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool userGesture, bool isRedirect)
{
    /*
        From documentation:
        The |request| object cannot be modified in this callback.
        CefLoadHandler::OnLoadingStateChange will be called twice in all cases. If the navigation is allowed CefLoadHandler::OnLoadStart and
       CefLoadHandler::OnLoadEnd will be called. If the navigation is canceled CefLoadHandler::OnLoadError will be called with an |errorCode| value of
       ERR_ABORTED.
    */

    CefURLParts urlParts;
    if (!CefParseURL(request->GetURL(), urlParts))
        return true;            // Cancel if invalid URL (this line will normally not be executed)

    bool    bResult;
    WString scheme = urlParts.scheme.str;
    if (scheme == L"http" || scheme == L"https")
    {
        SString host = UTF16ToMbUTF8(urlParts.host.str);
        if (host != "mta")
        {
            if (IsLocal() || g_pCore->GetWebCore()->GetDomainState(host, true) != eURLState::WEBPAGE_ALLOWED)
                bResult = true;            // Block remote here
            else
                bResult = false;            // Allow
        }
        else
            bResult = false;
    }
    else
        bResult = true;            // Block other schemes

    // Check if we're in the browser's main frame or only a frame element of the current page
    bool bIsMainFrame = frame->IsMain();

    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnNavigate, m_pEventsInterface, SString(request->GetURL()), bResult, bIsMainFrame);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnNavigate");

    // Return execution to CEF
    return bResult;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRequestHandler::OnBeforeResourceLoad        //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeResourceLoad(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
CefResourceRequestHandler::ReturnValue CWebView::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request,
                                                                      CefRefPtr<CefCallback> callback)
{
    // Mostly the same as CWebView::OnBeforeBrowse
    CefURLParts urlParts;
    if (!CefParseURL(request->GetURL(), urlParts))
        return RV_CANCEL;            // Cancel if invalid URL (this line will normally not be executed)

    SString domain = UTF16ToMbUTF8(urlParts.host.str);

    // Add some information to the HTTP header
    {
        CefRequest::HeaderMap headerMap;
        request->GetHeaderMap(headerMap);
        auto iter = headerMap.find("User-Agent");

        if (iter != headerMap.end())
        {
            // Add MTA:SA "watermark"
            iter->second = iter->second.ToString() + "; " MTA_CEF_USERAGENT;

            // Add 'Android' to get the mobile version
            SString strPropertyValue;
            if (GetProperty("mobile", strPropertyValue) && strPropertyValue == "1")
                iter->second = iter->second.ToString() + "; Mobile Android";

            // Allow YouTube TV to work (#1162)
            if (domain == "www.youtube.com" && UTF16ToMbUTF8(urlParts.path.str) == "/tv")
                iter->second = iter->second.ToString() + "; SMART-TV; Tizen 4.0";

            request->SetHeaderMap(headerMap);
        }
    }

    WString scheme = urlParts.scheme.str;
    if (scheme == L"http" || scheme == L"https")
    {
        if (domain != "mta")
        {
            if (IsLocal())
                return RV_CANCEL;            // Block remote requests in local mode generally

            eURLState urlState = g_pCore->GetWebCore()->GetDomainState(domain, true);
            if (urlState != eURLState::WEBPAGE_ALLOWED)
            {
                // Trigger onClientBrowserResourceBlocked event
                auto func = std::bind(&CWebBrowserEventsInterface::Events_OnResourceBlocked, m_pEventsInterface, SString(request->GetURL()), domain,
                                      urlState == eURLState::WEBPAGE_NOT_LISTED ? 0 : 1);
                g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnResourceBlocked");

                return RV_CANCEL;            // Block if explicitly forbidden
            }

            // Allow
            return RV_CONTINUE;
        }
        else
            return RV_CONTINUE;
    }
    else if (scheme == L"blob")
    {
        return RV_CONTINUE;
    }

    // Trigger onClientBrowserResourceBlocked event
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnResourceBlocked, m_pEventsInterface, SString(request->GetURL()), "",
                          2);            // reason 1 := blocked protocol scheme
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnResourceBlocked");

    // Block everything else
    return RV_CANCEL;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforeClose              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforeClose(CefRefPtr%3CCefBrowser%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    // Remove events owned by this webview and invoke left callbacks
    g_pCore->GetWebCore()->RemoveWebViewEvents(this);

    m_pWebView = nullptr;

    // Remove focused web view reference
    if (g_pCore->GetWebCore()->GetFocusedWebView() == this)
        g_pCore->GetWebCore()->SetFocusedWebView(nullptr);
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforePopup              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforePopup(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,constCefString&,constCefString&,constCefPopupFeatures&,CefWindowInfo&,CefRefPtr%3CCefClient%3E&,CefBrowserSettings&,bool*)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int popup_id, const CefString& target_url,
                             const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture,
                             const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings,
                             CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access)
{
    // ATTENTION: This method is called on the IO thread

    // Trigger the popup/new tab event
    SString strTagetURL = UTF16ToMbUTF8(target_url);
    SString strOpenerURL = UTF16ToMbUTF8(frame->GetURL());

    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnPopup, m_pEventsInterface, strTagetURL, strOpenerURL);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnBeforePopup");

    // Block popups generally
    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnAfterCreated             //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnAfterCreated(CefRefPtr<CefBrowser>) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    // Set web view reference
    m_pWebView = browser;

    // Call created event callback
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnCreated, m_pEventsInterface);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnAfterCreated");
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefJSDialogHandler::OnJSDialog                 //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefJSDialogHandler.html#OnJSDialog(CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,JSDialogType,constCefString&,constCefString&,CefRefPtr%3CCefJSDialogCallback%3E,bool&)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, CefJSDialogHandler::JSDialogType dialog_type,
                          const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message)
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
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefDialogHandler.html#OnFileDialog(CefRefPtr%3CCefBrowser%3E,FileDialogMode,constCefString&,constCefString&,conststd::vector%3CCefString%3E&,CefRefPtr%3CCefFileDialogCallback%3E)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnFileDialog(CefRefPtr<CefBrowser> browser, FileDialogMode mode, const CefString& title, const CefString& default_file_path,
        const std::vector<CefString>& accept_filters, const std::vector<CefString>& accept_extensions, const std::vector<CefString>& accept_descriptions,
        CefRefPtr<CefFileDialogCallback> callback)
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
void CWebView::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    m_CurrentTitle = UTF16ToMbUTF8(title);
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnTooltip                   //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefDisplayHandler.html#OnTooltip(CefRefPtr%3CCefBrowser%3E,CefString&) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& title)
{
    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnTooltip, m_pEventsInterface, UTF16ToMbUTF8(title));
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnTooltip");

    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnConsoleMessage            //
// http://magpcss.org/ceforum/apidocs/projects/%28default%29/CefDisplayHandler.html#OnConsoleMessage%28CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,int%29
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line)
{
    // Redirect console message to debug window (if development mode is enabled)
    if (g_pCore->GetWebCore()->IsTestModeEnabled())
    {
        g_pCore->GetWebCore()->AddEventToEventQueue(
            [message, source]() {
                g_pCore->DebugPrintfColor("[BROWSER] Console: %s (%s)", 255, 0, 0, UTF16ToMbUTF8(message).c_str(), UTF16ToMbUTF8(source).c_str());
            },
            this, "OnConsoleMessage");
    }

    return true;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnCursorChange              //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnCursorChange(CefRefPtr%3CCefBrowser%3E,CefCursorHandle) //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& cursorInfo)
{
    // Find the cursor index by the cursor handle
    unsigned char cursorIndex = static_cast<unsigned char>(type);

    // Queue event to run on the main thread
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnChangeCursor, m_pEventsInterface, cursorIndex);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnCursorChange");

    return false;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefContextMenuHandler::OnBeforeContextMenu     //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefContextMenuHandler.html#OnBeforeContextMenu(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefContextMenuParams%3E,CefRefPtr%3CCefMenuModel%3E)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model)
{
    // Show no context menu
    model->Clear();
}

void CWebView::ResumeCefThread()
{
    {
        // It's recommended to unlock a mutex before the cv notifying to avoid a possible pessimization
        std::unique_lock<std::mutex> lock(m_RenderData.dataMutex);
        m_RenderData.cefThreadState = ECefThreadState::Running;
    }

    m_RenderData.cefThreadCv.notify_all();
}

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
#include <chrono>

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
    m_bBeingDestroyed = true;

    if (IsMainThread())
    {
        if (auto pWebCore = g_pCore->GetWebCore(); pWebCore)
        {
            if (pWebCore->GetFocusedWebView() == this)
                pWebCore->SetFocusedWebView(nullptr);
        }
    }

    // Make sure we don't dead lock the CEF render thread
    ResumeCefThread();

    // Clean up AJAX handlers to prevent accumulation
    m_AjaxHandlers.clear();

    // Break circular reference: ensure browser reference is cleared
    // This is to prevent memory leaks from CWebView <-> CefBrowser cycles
    if (m_pWebView)
    {
        // Stop any loading immediately
        m_pWebView->StopLoad();

        // Navigate to blank page to force V8/DOM cleanup and release video/audio resources
        // We do this BEFORE hiding to ensure the navigation request is processed
        m_pWebView->GetMainFrame()->LoadURL("about:blank");

        // Notify that the browser is hidden and lost focus to release rendering resources
        m_pWebView->GetHost()->WasHidden(true);
        m_pWebView->GetHost()->SetFocus(false);

        // Force close the browser host to ensure the renderer process terminates immediately
        m_pWebView->GetHost()->CloseBrowser(true);
        m_pWebView = nullptr;
    }

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
        const auto pWebCore = g_pCore->GetWebCore();
        const bool bEnabledJavascript = pWebCore ? pWebCore->GetRemoteJavascriptEnabled() : false;
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

    // Clear AJAX handlers early to prevent late event processing
    m_AjaxHandlers.clear();

    if (m_pWebView)
    {
        // Stop any loading immediately
        m_pWebView->StopLoad();

        // Navigate to blank page to force V8/DOM cleanup and release video/audio resources
        // We do this BEFORE hiding to ensure the navigation request is processed
        m_pWebView->GetMainFrame()->LoadURL("about:blank");

        // Notify that the browser is hidden and lost focus to release rendering resources
        m_pWebView->GetHost()->WasHidden(true);
        m_pWebView->GetHost()->SetFocus(false);

        m_pWebView->GetHost()->CloseBrowser(true);
        m_pWebView = nullptr;
    }
}

bool CWebView::LoadURL(const SString& strURL, bool bFilterEnabled, const SString& strPostData, bool bURLEncoded)
{
    if (!m_pWebView)
        return false;

    CefURLParts urlParts;
    if (strURL.empty() || !CefParseURL(strURL, urlParts))
        return false;            // Invalid URL

    // Are we allowed to browse this website?
    if (bFilterEnabled)
    {
        auto pWebCore = g_pCore->GetWebCore();
        if (pWebCore && pWebCore->GetDomainState(UTF16ToMbUTF8(urlParts.host.str), true) != eURLState::WEBPAGE_ALLOWED)
            return false;
    }

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

        if (bPaused)
        {
            // Free memory held by render data when paused
            std::lock_guard<std::mutex> lock{m_RenderData.dataMutex};
            m_RenderData.changed = false;
            m_RenderData.popupShown = false;
            m_RenderData.buffer = nullptr;
            m_RenderData.dirtyRects.clear();
            m_RenderData.dirtyRects.shrink_to_fit();
            m_RenderData.popupBuffer.reset();

            // Release any waiting CEF thread
            m_RenderData.cefThreadState = ECefThreadState::Running;
            m_RenderData.cefThreadCv.notify_all();
        }
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

    auto pWebCore = g_pCore->GetWebCore();
    if (!pWebCore)
        return;
    
    if (state)
        pWebCore->SetFocusedWebView(this);
    else if (pWebCore->GetFocusedWebView() == this)
        pWebCore->SetFocusedWebView(nullptr);
}

void CWebView::ClearTexture()
{
    if (!m_pWebBrowserRenderItem) [[unlikely]]
        return;
    
    auto* const pD3DSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if (!pD3DSurface) [[unlikely]]
        return;

    D3DSURFACE_DESC SurfaceDesc;
    if (FAILED(pD3DSurface->GetDesc(&SurfaceDesc))) [[unlikely]]
        return;

    D3DLOCKED_RECT LockedRect;
    if (SUCCEEDED(pD3DSurface->LockRect(&LockedRect, nullptr, D3DLOCK_DISCARD)))
    {
        // Check for integer overflow in size calculation: height * pitch must fit in size_t
        // Ensure both are positive and that multiplication won't overflow
        if (SurfaceDesc.Height > 0 && LockedRect.Pitch > 0 && 
            static_cast<size_t>(SurfaceDesc.Height) <= SIZE_MAX / static_cast<size_t>(LockedRect.Pitch)) [[likely]]
        {
            const auto memsetSize = static_cast<size_t>(SurfaceDesc.Height) * static_cast<size_t>(LockedRect.Pitch);
            std::memset(LockedRect.pBits, 0xFF, memsetSize);
        }
        pD3DSurface->UnlockRect();
    }
}

void CWebView::UpdateTexture()
{
    const std::lock_guard lock(m_RenderData.dataMutex);

    // Validate render item exists before accessing
    if (!m_pWebBrowserRenderItem) [[unlikely]]
    {
        m_RenderData.changed = m_RenderData.popupShown = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
        m_RenderData.cefThreadCv.notify_all();
        return;
    }

    auto* const pSurface = m_pWebBrowserRenderItem->m_pD3DRenderTargetSurface;
    if (m_bBeingDestroyed || !pSurface) [[unlikely]]
    {
        m_RenderData.changed = m_RenderData.popupShown = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
        m_RenderData.cefThreadCv.notify_all();
        return;
    }

    // Discard current buffer if size doesn't match
    // This happens when resizing the browser as OnPaint is called asynchronously
    if (m_RenderData.changed && (m_pWebBrowserRenderItem->m_uiSizeX != m_RenderData.width || m_pWebBrowserRenderItem->m_uiSizeY != m_RenderData.height))
    {
        m_RenderData.changed = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
    }

    if (m_RenderData.changed || m_RenderData.popupShown) [[likely]]
    {
        // Lock surface
        D3DLOCKED_RECT LockedRect;
        if (SUCCEEDED(pSurface->LockRect(&LockedRect, nullptr, 0)))
        {
            // Dirty rect implementation, don't use this as loops are significantly slower than memcpy
            auto* const destData = static_cast<byte*>(LockedRect.pBits);
            const auto* const sourceData = static_cast<const byte*>(m_RenderData.buffer);
            const auto destPitch = LockedRect.Pitch;

            // Validate destination pitch
            if (destPitch <= 0) [[unlikely]]
            {
                pSurface->UnlockRect();
                m_RenderData.changed = false;
                m_RenderData.popupShown = false;
                m_RenderData.buffer = nullptr;
                m_RenderData.dirtyRects.clear();
                m_RenderData.dirtyRects.shrink_to_fit();
                m_RenderData.cefThreadState = ECefThreadState::Running;
                m_RenderData.cefThreadCv.notify_all();
                return;
            }
            
            // Validate sourcePitch calculation won't overflow
            constexpr auto maxWidthForPitch = INT_MAX / CEF_PIXEL_STRIDE;
            if (m_RenderData.width > maxWidthForPitch) [[unlikely]]
            {
                pSurface->UnlockRect();
                m_RenderData.changed = false;
                m_RenderData.popupShown = false;
                m_RenderData.buffer = nullptr;
                m_RenderData.dirtyRects.clear();
                m_RenderData.dirtyRects.shrink_to_fit();
                m_RenderData.cefThreadState = ECefThreadState::Running;
                m_RenderData.cefThreadCv.notify_all();
                return;
            }
            const auto sourcePitch = m_RenderData.width * CEF_PIXEL_STRIDE;

            // Validate source buffer exists before accessing it
            if (!sourceData) [[unlikely]]
            {
                pSurface->UnlockRect();
                m_RenderData.changed = false;
                m_RenderData.popupShown = false;
                m_RenderData.buffer = nullptr;
                m_RenderData.dirtyRects.clear();
                m_RenderData.dirtyRects.shrink_to_fit();
                m_RenderData.cefThreadState = ECefThreadState::Running;
                m_RenderData.cefThreadCv.notify_all();
                return;
            }

            // Update view area
            if (m_RenderData.changed) [[likely]]
            {
                // Update changed state
                m_RenderData.changed = false;

                const auto& dirtyRects = m_RenderData.dirtyRects;
                if (!dirtyRects.empty() && dirtyRects[0].width == m_RenderData.width &&
                    dirtyRects[0].height == m_RenderData.height)
                {
                    // Note that D3D texture size can be hardware dependent(especially with dynamic texture)
                    // When destination and source pitches differ we must copy pixels row by row
                    if (destPitch == sourcePitch) [[likely]]
                    {
                        // Check for integer overflow in size calculation: height * pitch must fit in size_t
                        if (m_RenderData.height > 0 && 
                            static_cast<size_t>(m_RenderData.height) > SIZE_MAX / static_cast<size_t>(destPitch)) [[unlikely]]
                        {
                            pSurface->UnlockRect();
                            m_RenderData.changed = false;
                            m_RenderData.popupShown = false;
                            m_RenderData.buffer = nullptr;
                            m_RenderData.dirtyRects.clear();
                            m_RenderData.dirtyRects.shrink_to_fit();
                            m_RenderData.cefThreadState = ECefThreadState::Running;
                            m_RenderData.cefThreadCv.notify_all();
                            return;
                        }
                        std::memcpy(destData, sourceData, static_cast<size_t>(destPitch) * static_cast<size_t>(m_RenderData.height));
                    }
                    else
                    {
                        // Ensure both pitches are positive before row-by-row copy
                        if (destPitch <= 0 || sourcePitch <= 0) [[unlikely]]
                        {
                            pSurface->UnlockRect();
                            m_RenderData.changed = false;
                            m_RenderData.popupShown = false;
                            m_RenderData.buffer = nullptr;
                            m_RenderData.dirtyRects.clear();
                            m_RenderData.dirtyRects.shrink_to_fit();
                            m_RenderData.cefThreadState = ECefThreadState::Running;
                            m_RenderData.cefThreadCv.notify_all();
                            return;
                        }

                        // Check for integer overflow in size calculation for row-by-row copy
                        if (m_RenderData.height > 0 && 
                            (static_cast<size_t>(m_RenderData.height) > SIZE_MAX / static_cast<size_t>(destPitch) ||
                             static_cast<size_t>(m_RenderData.height) > SIZE_MAX / static_cast<size_t>(sourcePitch))) [[unlikely]]
                        {
                            pSurface->UnlockRect();
                            m_RenderData.changed = false;
                            m_RenderData.popupShown = false;
                            m_RenderData.buffer = nullptr;
                            m_RenderData.dirtyRects.clear();
                            m_RenderData.dirtyRects.shrink_to_fit();
                            m_RenderData.cefThreadState = ECefThreadState::Running;
                            m_RenderData.cefThreadCv.notify_all();
                            return;
                        }
                        
                        for (int y = 0; y < m_RenderData.height; ++y)
                        {
                            // Use size_t for all calculations to prevent overflow
                            const auto sourceIndex = static_cast<size_t>(y) * static_cast<size_t>(sourcePitch);
                            const auto destIndex = static_cast<size_t>(y) * static_cast<size_t>(destPitch);
                            const auto copySize = std::min(static_cast<size_t>(sourcePitch), static_cast<size_t>(destPitch));

                            std::memcpy(&destData[destIndex], &sourceData[sourceIndex], copySize);
                        }
                    }
                }
                else
                {
                    // Check for integer overflow in destination size calculation
                    if (m_RenderData.height > 0 && 
                        static_cast<size_t>(m_RenderData.height) > SIZE_MAX / static_cast<size_t>(destPitch)) [[unlikely]]
                    {
                        pSurface->UnlockRect();
                        m_RenderData.changed = false;
                        m_RenderData.popupShown = false;
                        m_RenderData.buffer = nullptr;
                        m_RenderData.dirtyRects.clear();
                        m_RenderData.dirtyRects.shrink_to_fit();
                        m_RenderData.cefThreadState = ECefThreadState::Running;
                        m_RenderData.cefThreadCv.notify_all();
                        return;
                    }

                    // Update dirty rects
                    for (const auto& rect : dirtyRects)
                    {
                        // Validate dirty rect bounds to prevent buffer overflow
                        if (rect.x < 0 || rect.y < 0 || rect.width <= 0 || rect.height <= 0) [[unlikely]]
                            continue;
                        
                        // Check bounds using addition to prevent subtraction underflow
                        // rect.x + rect.width could overflow, so check rect.x and rect.width separately
                        if (rect.x >= m_RenderData.width || rect.y >= m_RenderData.height ||
                            rect.width > m_RenderData.width || rect.height > m_RenderData.height ||
                            rect.x > m_RenderData.width - rect.width || rect.y > m_RenderData.height - rect.height) [[unlikely]]
                            continue;

                        // Pre-calculate end to prevent overflow in loop condition
                        const auto rectEndY = rect.y + rect.height;

                        // Ensure we don't write past the destination pitch
                        if (static_cast<size_t>(destPitch) < static_cast<size_t>(rect.x + rect.width) * CEF_PIXEL_STRIDE) [[unlikely]]
                            continue;

                        for (int y = rect.y; y < rectEndY; ++y)
                        {
                            // Note that D3D texture size can be hardware dependent(especially with dynamic texture)
                            // We cannot be sure that source and destination pitches are the same
                            // Use size_t for all calculations to prevent integer overflow
                            const auto sourceIndex = static_cast<size_t>(y) * static_cast<size_t>(sourcePitch) + 
                                                      static_cast<size_t>(rect.x) * CEF_PIXEL_STRIDE;
                            const auto destIndex = static_cast<size_t>(y) * static_cast<size_t>(destPitch) + 
                                                    static_cast<size_t>(rect.x) * CEF_PIXEL_STRIDE;

                            std::memcpy(&destData[destIndex], &sourceData[sourceIndex], static_cast<size_t>(rect.width) * CEF_PIXEL_STRIDE);
                        }
                    }
                }
            }

            // Update popup area (override certain areas of the view texture)
            // Validate popup rect bounds to prevent integer overflow and out-of-bounds access
            const auto& popupRect = m_RenderData.popupRect;
            const auto renderWidth = static_cast<int>(m_pWebBrowserRenderItem->m_uiSizeX);
            const auto renderHeight = static_cast<int>(m_pWebBrowserRenderItem->m_uiSizeY);
            const auto popupSizeMismatches = 
                popupRect.x < 0 || popupRect.y < 0 ||
                popupRect.width <= 0 || popupRect.height <= 0 ||
                popupRect.x >= renderWidth || popupRect.y >= renderHeight ||
                popupRect.width > renderWidth || popupRect.height > renderHeight ||
                popupRect.x > renderWidth - popupRect.width ||
                popupRect.y > renderHeight - popupRect.height;

            // Verify popup buffer exists before accessing it
            if (m_RenderData.popupShown && !popupSizeMismatches && m_RenderData.popupBuffer) [[likely]]
            {
                // Validate popup pitch calculation won't overflow
                constexpr auto maxWidthForPopupPitch = INT_MAX / CEF_PIXEL_STRIDE;
                if (popupRect.width > maxWidthForPopupPitch) [[unlikely]]
                {
                    pSurface->UnlockRect();
                    m_RenderData.popupShown = false;
                    m_RenderData.buffer = nullptr;
                    m_RenderData.dirtyRects.clear();
                    m_RenderData.dirtyRects.shrink_to_fit();
                    m_RenderData.cefThreadState = ECefThreadState::Running;
                    m_RenderData.cefThreadCv.notify_all();
                    return;
                }
                const auto popupPitch = popupRect.width * CEF_PIXEL_STRIDE;

                // Ensure we don't write past the destination pitch
                if (static_cast<size_t>(destPitch) < static_cast<size_t>(popupRect.x + popupRect.width) * CEF_PIXEL_STRIDE) [[unlikely]]
                {
                    pSurface->UnlockRect();
                    m_RenderData.popupShown = false;
                    m_RenderData.buffer = nullptr;
                    m_RenderData.dirtyRects.clear();
                    m_RenderData.dirtyRects.shrink_to_fit();
                    m_RenderData.cefThreadState = ECefThreadState::Running;
                    m_RenderData.cefThreadCv.notify_all();
                    return;
                }
                
                for (int y = 0; y < popupRect.height; ++y)
                {
                    // Use size_t for all calculations to prevent integer overflow
                    const auto sourceIndex = static_cast<size_t>(y) * static_cast<size_t>(popupPitch);
                    // Calculate destination y coordinate safely
                    const auto destY = static_cast<size_t>(popupRect.y) + static_cast<size_t>(y);
                    const auto destIndex = destY * static_cast<size_t>(destPitch) + 
                                            static_cast<size_t>(popupRect.x) * CEF_PIXEL_STRIDE;

                    std::memcpy(&destData[destIndex], &m_RenderData.popupBuffer[sourceIndex], static_cast<size_t>(popupPitch));
                }
            }

            // Unlock surface
            pSurface->UnlockRect();
        }
        else
        {
            OutputDebugLine("[CWebView] UpdateTexture: LockRect failed");
            // Clear flags to prevent re-attempting to render stale buffer
            m_RenderData.changed = false;
            m_RenderData.popupShown = false;
        }
        
        // Clear buffer pointer - it's only valid during OnPaint callback and we've used it
        m_RenderData.buffer = nullptr;
        
        // Clear dirty rects and release capacity to prevent memory accumulation
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
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

    // Note: GetFrameNames is deprecated, but no modern alternative exists for audio volume control
    // This is a legacy thing that works with CEF3
    std::vector<CefString> frameNames;
    m_pWebView->GetFrameNames(frameNames);

    for (auto& name : frameNames)
    {
        auto frame = m_pWebView->GetFrameByName(name);
        if (frame)
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
            // Avoid UAF
            if (webView->IsBeingDestroyed())
                return;
            
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
    // Validate render item exists
    if (!m_pWebBrowserRenderItem) [[unlikely]]
        return;
    
    // Resize underlying texture
    m_pWebBrowserRenderItem->Resize(size);

    // Send resize event to CEF
    if (m_pWebView)
        m_pWebView->GetHost()->WasResized();

    ResumeCefThread();
}

CVector2D CWebView::GetSize()
{
    if (!m_pWebBrowserRenderItem) [[unlikely]]
        return CVector2D(0.0f, 0.0f);
    
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
    // Only queue event if not being destroyed to prevent UAF
    if (!m_bBeingDestroyed)
    {
        auto func = std::bind(&CWebBrowserEventsInterface::Events_OnAjaxRequest, m_pEventsInterface, pHandler, strURL);
        g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "AjaxResourceRequest");
    }
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html#OnProcessMessageReceived(CefRefPtr%3CCefBrowser%3E,CefProcessId,CefRefPtr%3CCefProcessMessage%3E)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message)
{
    if (m_bBeingDestroyed)
        return false;

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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#GetViewRect(CefRefPtr%3CCefBrowser%3E,CefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;

    if (m_bBeingDestroyed || !m_pWebBrowserRenderItem) [[unlikely]]
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPopupShow(CefRefPtr<CefBrowser>,bool) //
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPopupSize(CefRefPtr<CefBrowser>,constCefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    std::lock_guard<std::mutex> lock{m_RenderData.dataMutex};

    // If dimensions change, the current popup buffer is no longer valid for the new size
    // We must release it to prevent UpdateTexture from reading past the end of the buffer
    // using the new (larger) dimensions
    if (m_RenderData.popupRect.width != rect.width || m_RenderData.popupRect.height != rect.height)
    {
        m_RenderData.popupBuffer.reset();
    }

    // Update rect
    m_RenderData.popupRect = rect;

    // Note: Don't allocate buffer here - OnPaint may provide different dimensions
    // Buffer allocation moved to OnPaint to prevent dimension mismatch
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefRenderHandler::OnPaint                      //
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPaint(CefRefPtr%3CCefBrowser%3E,PaintElementType,constRectList&,constvoid*,int,int)
// //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer, int width, int height)
{
    if (m_bBeingDestroyed) [[unlikely]]
        return;

    std::unique_lock lock(m_RenderData.dataMutex);

    // Copy popup buffer
    if (paintType == PET_POPUP)
    {
        // Validate buffer parameter from CEF
        if (!buffer || width <= 0 || height <= 0) [[unlikely]]
            return;

        // Allocate buffer based on actual paint dimensions, not OnPopupSize rect
        // This prevents buffer overflow when CEF provides different dimensions
        // Check for integer overflow in size calculation: width * height * CEF_PIXEL_STRIDE must fit in size_t
        constexpr auto maxDimension = INT_MAX / CEF_PIXEL_STRIDE;
        if (width > maxDimension || height > maxDimension) [[unlikely]]
            return;  // Individual dimension too large
        if (static_cast<size_t>(width) > SIZE_MAX / (static_cast<size_t>(height) * CEF_PIXEL_STRIDE)) [[unlikely]]
            return;  // width * height * stride would overflow
        
        const auto requiredSize = static_cast<size_t>(width) * static_cast<size_t>(height) * CEF_PIXEL_STRIDE;
        
        // Calculate current size safely to avoid overflow
        size_t currentSize = 0;
        const auto& popupRect = m_RenderData.popupRect;
        if (popupRect.width > 0 && popupRect.height > 0 &&
            popupRect.width <= maxDimension && popupRect.height <= maxDimension &&
            static_cast<size_t>(popupRect.width) <= SIZE_MAX / (static_cast<size_t>(popupRect.height) * CEF_PIXEL_STRIDE)) [[likely]]
        {
            currentSize = static_cast<size_t>(popupRect.width) * 
                         static_cast<size_t>(popupRect.height) * CEF_PIXEL_STRIDE;
        }
        
        // Reallocate if size changed or buffer doesn't exist
        if (!m_RenderData.popupBuffer || requiredSize != currentSize) [[unlikely]]
        {
            m_RenderData.popupBuffer = std::make_unique<byte[]>(requiredSize);
            // Update rect to reflect actual dimensions
            m_RenderData.popupRect.width = width;
            m_RenderData.popupRect.height = height;
        }
        
        std::memcpy(m_RenderData.popupBuffer.get(), buffer, requiredSize);

        // Popup path doesn't wait, so no need to signal
        return;
    }

    // Validate main frame buffer parameter
    if (!buffer || width <= 0 || height <= 0) [[unlikely]]
    {
        m_RenderData.changed = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
        m_RenderData.cefThreadCv.notify_all();
        return;
    }

    // Check for integer overflow in size calculation
    constexpr auto maxDimension = INT_MAX / CEF_PIXEL_STRIDE;
    if (width > maxDimension || height > maxDimension) [[unlikely]]
    {
        m_RenderData.changed = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
        m_RenderData.cefThreadCv.notify_all();
        return;
    }
    if (static_cast<size_t>(width) > SIZE_MAX / (static_cast<size_t>(height) * CEF_PIXEL_STRIDE)) [[unlikely]]
    {
        m_RenderData.changed = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
        m_RenderData.cefThreadCv.notify_all();
        return;
    }

    // Store render data
    m_RenderData.buffer = buffer;
    m_RenderData.width = width;
    m_RenderData.height = height;
    m_RenderData.dirtyRects = dirtyRects;
    // Prevent vector capacity growth memory leak - shrink excess capacity
    m_RenderData.dirtyRects.shrink_to_fit();
    m_RenderData.changed = true;

    // Wait for the main thread to handle drawing the texture
    m_RenderData.cefThreadState = ECefThreadState::Wait;
    if (!m_RenderData.cefThreadCv.wait_for(lock, std::chrono::milliseconds(250), [&]() { return m_RenderData.cefThreadState == ECefThreadState::Running; }))
    {
        // Timed out - rendering is likely stalled or stopped
        // Clear data to prevent UpdateTexture from using stale buffer and allow CEF to free it
        m_RenderData.changed = false;
        m_RenderData.buffer = nullptr;
        m_RenderData.dirtyRects.clear();
        m_RenderData.dirtyRects.shrink_to_fit();
        m_RenderData.cefThreadState = ECefThreadState::Running;
    }
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLoadHandler::OnLoadStart                    //
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadStart(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType)
{
    // Note: TransitionType parameter is deprecated in CEF3 but still required by virtual interface override
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadEnd(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,int) //
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLoadHandler.html#OnLoadError(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,ErrorCode,constCefString&,constCefString&)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeBrowse(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E,bool)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html#OnBeforeResourceLoad(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefRequest%3E)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforeClose(CefRefPtr%3CCefBrowser%3E) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    // Remove events owned by this webview and invoke left callbacks
    if (auto pWebCore = g_pCore->GetWebCore(); pWebCore) [[likely]]
    {
        pWebCore->RemoveWebViewEvents(this);

        // Remove focused web view reference
        if (pWebCore->GetFocusedWebView() == this)
            pWebCore->SetFocusedWebView(nullptr);
    }

    m_pWebView = nullptr;
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefLifeSpanHandler::OnBeforePopup              //
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnBeforePopup(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,constCefString&,constCefString&,constCefPopupFeatures&,CefWindowInfo&,CefRefPtr%3CCefClient%3E&,CefBrowserSettings&,bool*)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html#OnAfterCreated(CefRefPtr<CefBrowser>) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (m_bBeingDestroyed)
    {
        browser->GetHost()->CloseBrowser(true);
        return;
    }

    // Set web view reference
    m_pWebView = browser;

    // Call created event callback
    auto func = std::bind(&CWebBrowserEventsInterface::Events_OnCreated, m_pEventsInterface);
    g_pCore->GetWebCore()->AddEventToEventQueue(func, this, "OnAfterCreated");
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefJSDialogHandler::OnJSDialog                 //
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefJSDialogHandler.html#OnJSDialog(CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,JSDialogType,constCefString&,constCefString&,CefRefPtr%3CCefJSDialogCallback%3E,bool&)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefDialogHandler.html#OnFileDialog(CefRefPtr%3CCefBrowser%3E,FileDialogMode,constCefString&,constCefString&,conststd::vector%3CCefString%3E&,CefRefPtr%3CCefFileDialogCallback%3E)
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefDisplayHandler.html#OnTitleChange(CefRefPtr%3CCefBrowser%3E,constCefString&) //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    m_CurrentTitle = UTF16ToMbUTF8(title);
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: CefDisplayHandler::OnTooltip                   //
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefDisplayHandler.html#OnTooltip(CefRefPtr%3CCefBrowser%3E,CefString&) //
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
// https://magpcss.org/ceforum/apidocs/projects/%28default%29/CefDisplayHandler.html#OnConsoleMessage%28CefRefPtr%3CCefBrowser%3E,constCefString&,constCefString&,int%29
// //
//                                                                //
////////////////////////////////////////////////////////////////////
bool CWebView::OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line)
{
    // Note: cef_log_severity_t parameter is deprecated in CEF3 but required for virtual override
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnCursorChange(CefRefPtr%3CCefBrowser%3E,CefCursorHandle) //
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
// https://magpcss.org/ceforum/apidocs3/projects/(default)/CefContextMenuHandler.html#OnBeforeContextMenu(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefContextMenuParams%3E,CefRefPtr%3CCefMenuModel%3E)
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

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebView.h
 *  PURPOSE:     Web view class
 *
 *****************************************************************************/

#pragma once

#undef GetNextSibling
#undef GetFirstChild
#include <core/CWebViewInterface.h>
#include <core/CWebBrowserEventsInterface.h>
#include <cef3/cef/include/cef_app.h>
#include <cef3/cef/include/cef_browser.h>
#include <cef3/cef/include/cef_client.h>
#include <cef3/cef/include/cef_render_handler.h>
#include <cef3/cef/include/cef_life_span_handler.h>
#include <cef3/cef/include/cef_context_menu_handler.h>
#include <cef3/cef/include/cef_resource_request_handler.h>
#include <SString.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <condition_variable>
#define GetNextSibling(hwnd) GetWindow(hwnd, GW_HWNDNEXT) // Re-define the conflicting macro
#define GetFirstChild(hwnd) GetTopWindow(hwnd)

#define MTA_CEF_USERAGENT "Multi Theft Auto: San Andreas Client " MTA_DM_BUILDTAG_LONG

enum class ECefThreadState
{
    Running = 0,            // CEF thread is currently running
    Wait                    // CEF thread is waiting for the main thread
};

class CWebView : public CWebViewInterface,
                 private CefClient,
                 private CefRenderHandler,
                 private CefLoadHandler,
                 private CefRequestHandler,
                 private CefResourceRequestHandler,
                 private CefLifeSpanHandler,
                 private CefJSDialogHandler,
                 private CefDialogHandler,
                 private CefDisplayHandler,
                 private CefContextMenuHandler
{
public:
    CWebView(bool bIsLocal, CWebBrowserItem* pWebBrowserRenderItem, bool bTransparent = false);
    virtual ~CWebView();
    void                  Initialise();
    void                  SetWebBrowserEvents(CWebBrowserEventsInterface* pInterface) { m_pEventsInterface = pInterface; };
    void                  CloseBrowser();
    CefRefPtr<CefBrowser> GetCefBrowser() { return m_pWebView; };

    bool IsBeingDestroyed() { return m_bBeingDestroyed; }
    void SetBeingDestroyed(bool state) { m_bBeingDestroyed = state; }

    // Exported methods
    bool               LoadURL(const SString& strURL, bool bFilterEnabled = true, const SString& strPostData = SString(), bool bURLEncoded = true);
    bool               IsLoading();
    SString            GetURL();
    const SString&     GetTitle();
    void               SetRenderingPaused(bool bPaused);
    const bool         GetRenderingPaused() const;
    void               Focus(bool state = true);
    IDirect3DTexture9* GetTexture() { return static_cast<IDirect3DTexture9*>(m_pWebBrowserRenderItem->m_pD3DTexture); }
    void               ClearTexture();

    void UpdateTexture();

    bool HasInputFocus() { return m_bHasInputFocus; }

    void ExecuteJavascript(const SString& strJavascriptCode);

    bool SetProperty(const SString& strKey, const SString& strValue);
    bool GetProperty(const SString& strKey, SString& outProperty);

    void InjectMouseMove(int iPosX, int iPosY);
    void InjectMouseDown(eWebBrowserMouseButton mouseButton, int count);
    void InjectMouseUp(eWebBrowserMouseButton mouseButton);
    void InjectMouseWheel(int iScrollVert, int iScrollHorz);
    void InjectKeyboardEvent(const CefKeyEvent& keyEvent);

    bool IsLocal() { return m_bIsLocal; };

    float GetAudioVolume() { return m_fVolume; };
    bool  SetAudioVolume(float fVolume);

    void GetSourceCode(const std::function<void(const std::string& code)>& callback);

    virtual void      Resize(const CVector2D& size) override;
    virtual CVector2D GetSize() override;

    bool GetFullPathFromLocal(SString& strPath);
    bool VerifyFile(const SString& strPath, CBuffer& outFileData);

    virtual bool RegisterAjaxHandler(const SString& strURL) override;
    virtual bool UnregisterAjaxHandler(const SString& strURL) override;
    virtual bool HasAjaxHandler(const SString& strURL);
    virtual void HandleAjaxRequest(const SString& strURL, class CAjaxResourceHandler* pHandler);

    virtual bool ToggleDevTools(bool visible) override;

    bool CanGoBack();
    bool CanGoForward();
    bool GoBack();
    bool GoForward();
    void Refresh(bool ignoreCache);

    // CefClient methods
    virtual CefRefPtr<CefRenderHandler>      GetRenderHandler() override { return this; };
    virtual CefRefPtr<CefLoadHandler>        GetLoadHandler() override { return this; };
    virtual CefRefPtr<CefRequestHandler>     GetRequestHandler() override { return this; };
    virtual CefRefPtr<CefLifeSpanHandler>    GetLifeSpanHandler() override { return this; };
    virtual CefRefPtr<CefJSDialogHandler>    GetJSDialogHandler() override { return this; };
    virtual CefRefPtr<CefDialogHandler>      GetDialogHandler() override { return this; };
    virtual CefRefPtr<CefDisplayHandler>     GetDisplayHandler() override { return this; };
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; };
    virtual bool                             OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
                                                                      CefRefPtr<CefProcessMessage> message) override;

    // CefRenderHandler methods
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects,
                         const void* buffer, int width, int height) override;

    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText,
                             const CefString& failedURL) override;

    // CefRequestHandler methods
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool userGesture,
                                bool isRedirect) override;
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                                                           CefRefPtr<CefRequest> request, bool is_navigation, bool is_download,
                                                                           const CefString& request_initiator, bool& disable_default_handling) override
    {
        return this;
    };

    // CefResourceRequestHandler
    virtual CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request,
                                                                        CefRefPtr<CefCallback> callback) override;

    // CefLifeSpawnHandler methods
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int popup_id, const CefString& target_url,
                               const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture,
                               const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings,
                               CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access) override;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    // CefJSDialogHandler methods
    virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, CefJSDialogHandler::JSDialogType dialog_type,
                            const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback,
                            bool& suppress_message) override;

    // CefDialogHandler methods
    virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser, FileDialogMode mode, const CefString& title, const CefString& default_file_path,
        const std::vector<CefString>& accept_filters, const std::vector<CefString>& accept_extensions, const std::vector<CefString>& accept_descriptions,
        CefRefPtr<CefFileDialogCallback> callback) override;

    // CefDisplayHandler methods
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source,
                                  int line) override;
    virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& cursorInfo) override;

    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) override;

private:
    void ResumeCefThread();

    CefRefPtr<CefBrowser> m_pWebView;
    CWebBrowserItem*      m_pWebBrowserRenderItem;

    std::atomic_bool           m_bBeingDestroyed;
    bool                       m_bIsLocal;
    bool                       m_bIsRenderingPaused;
    bool                       m_bIsTransparent;
    POINT                      m_vecMousePosition;
    bool                       m_mouseButtonStates[3];
    SString                    m_CurrentTitle;
    float                      m_fVolume;
    std::map<SString, SString> m_Properties;
    bool                       m_bHasInputFocus;
    std::set<std::string>      m_AjaxHandlers;

    struct
    {
        bool                    changed = false;
        std::mutex              dataMutex;
        ECefThreadState         cefThreadState = ECefThreadState::Running;
        std::condition_variable cefThreadCv;

        const void*                buffer;
        int                        width, height;
        CefRenderHandler::RectList dirtyRects;

        CefRect                 popupRect;
        bool                    popupShown = false;
        std::unique_ptr<byte[]> popupBuffer;
    } m_RenderData;

    CWebBrowserEventsInterface* m_pEventsInterface;

public:
    // Implement smartpointer methods (all Cef-classes require that since they are derived from CefBase)
    IMPLEMENT_REFCOUNTING(CWebView);
};

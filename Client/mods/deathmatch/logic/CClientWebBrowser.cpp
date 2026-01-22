/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientWebBrowser.cpp
 *  PURPOSE:     ClientEntity webbrowser tab class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientWebBrowser.h"

CClientWebBrowser::CClientWebBrowser(CClientManager* pManager, ElementID ID, CWebBrowserItem* pWebBrowserItem, bool bLocal, bool bTransparent)
    : ClassInit(this), CClientTexture(pManager, ID, pWebBrowserItem)
{
    m_pResource = nullptr;
    m_pManager = pManager;
    m_pWebView = nullptr;
    SetTypeName("webbrowser");

    // Create the web view
    const auto pWebCore = g_pCore->GetWebCore();
    if (!pWebCore)
    {
        return;
    }
    m_pWebView = pWebCore->CreateWebView(pWebBrowserItem->m_uiSizeX, pWebBrowserItem->m_uiSizeY, bLocal, pWebBrowserItem, bTransparent);

    if (!m_pWebView)
        return;

    // Set events interface
    m_pWebView->SetWebBrowserEvents(this);

    // Initialise the webview after setting the events interface
    m_pWebView->Initialise();
}

CClientWebBrowser::~CClientWebBrowser()
{
    // Mark as being destroyed to prevent event callbacks from executing
    m_bBeingDestroyed = true;

    if (m_pWebView)
        m_pWebView->ClearWebBrowserEvents(this);

    // Unlink from tree first to prevent event callbacks from reaching
    Unlink();

    // Clear AJAX callback map to release any held references
    m_mapAjaxCallback.clear();

    if (m_pWebView)
    {
        // Use GetWebCoreUnchecked() to ensure cleanup even if initialization failed
        const auto pWebCore = g_pCore->GetWebCoreUnchecked();
        if (pWebCore)
            pWebCore->DestroyWebView(m_pWebView);
        m_pWebView = nullptr;
    }
}

void CClientWebBrowser::Unlink()
{
    // Mark as being destroyed (DirectX surfaces won't be available anymore)
    if (m_pWebView)
        m_pWebView->SetBeingDestroyed(true);

    CClientRenderElement::Unlink();
}

bool CClientWebBrowser::IsLoading()
{
    return m_pWebView ? m_pWebView->IsLoading() : false;
}

bool CClientWebBrowser::LoadURL(const SString& strURL, bool bFilterEnabled, const SString& strPostData, bool bURLEncoded)
{
    return m_pWebView ? m_pWebView->LoadURL(strURL, bFilterEnabled, strPostData, bURLEncoded) : false;
}

const SString& CClientWebBrowser::GetTitle()
{
    static SString empty;
    return m_pWebView ? m_pWebView->GetTitle() : empty;
}

SString CClientWebBrowser::GetURL()
{
    return m_pWebView ? m_pWebView->GetURL() : SString();
}

void CClientWebBrowser::SetRenderingPaused(bool bPaused)
{
    if (m_pWebView)
        m_pWebView->SetRenderingPaused(bPaused);
}

void CClientWebBrowser::Focus()
{
    if (m_pWebView)
        m_pWebView->Focus();
}

bool CClientWebBrowser::ExecuteJavascript(const SString& strJavascriptCode)
{
    if (!m_pWebView)
        return false;
    
    // Don't allow javascript code execution on remote websites
    if (!m_pWebView->IsLocal())
        return false;

    m_pWebView->ExecuteJavascript(strJavascriptCode);
    return true;
}

bool CClientWebBrowser::SetProperty(const SString& strKey, const SString& strValue)
{
    return m_pWebView ? m_pWebView->SetProperty(strKey, strValue) : false;
}

bool CClientWebBrowser::GetProperty(const SString& strKey, SString& outValue)
{
    return m_pWebView ? m_pWebView->GetProperty(strKey, outValue) : false;
}

void CClientWebBrowser::InjectMouseMove(int iPosX, int iPosY)
{
    if (m_pWebView)
        m_pWebView->InjectMouseMove(iPosX, iPosY);
}

void CClientWebBrowser::InjectMouseDown(eWebBrowserMouseButton mouseButton, int count)
{
    if (m_pWebView)
        m_pWebView->InjectMouseDown(mouseButton, count);
}

void CClientWebBrowser::InjectMouseUp(eWebBrowserMouseButton mouseButton)
{
    if (m_pWebView)
        m_pWebView->InjectMouseUp(mouseButton);
}

void CClientWebBrowser::InjectMouseWheel(int iScrollVert, int iScrollHorz)
{
    if (m_pWebView)
        m_pWebView->InjectMouseWheel(iScrollVert, iScrollHorz);
}

bool CClientWebBrowser::IsLocal()
{
    return m_pWebView ? m_pWebView->IsLocal() : false;
}

float CClientWebBrowser::GetAudioVolume()
{
    return m_pWebView ? m_pWebView->GetAudioVolume() : 0.0f;
}

bool CClientWebBrowser::SetAudioVolume(float fVolume)
{
    return m_pWebView ? m_pWebView->SetAudioVolume(fVolume) : false;
}

void CClientWebBrowser::GetSourceCode(const std::function<void(const std::string& code)>& callback)
{
    if (m_pWebView)
        m_pWebView->GetSourceCode(callback);
}

void CClientWebBrowser::Resize(const CVector2D& size)
{
    if (m_pWebView)
        m_pWebView->Resize(size);
}

bool CClientWebBrowser::CanGoBack()
{
    return m_pWebView ? m_pWebView->CanGoBack() : false;
}

bool CClientWebBrowser::CanGoForward()
{
    return m_pWebView ? m_pWebView->CanGoForward() : false;
}

bool CClientWebBrowser::GoBack()
{
    return m_pWebView ? m_pWebView->GoBack() : false;
}

bool CClientWebBrowser::GoForward()
{
    return m_pWebView ? m_pWebView->GoForward() : false;
}

void CClientWebBrowser::Refresh(bool bIgnoreCache)
{
    if (m_pWebView)
        m_pWebView->Refresh(bIgnoreCache);
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//            CWebBrowserEventsInterface implementation                   //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
void CClientWebBrowser::Events_OnCreated()
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    CallEvent("onClientBrowserCreated", Arguments, false);
}

void CClientWebBrowser::Events_OnLoadingStart(const SString& strURL, bool bMainFrame)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushBoolean(bMainFrame);
    CallEvent("onClientBrowserLoadingStart", Arguments, false);
}

void CClientWebBrowser::Events_OnDocumentReady(const SString& strURL)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    CallEvent("onClientBrowserDocumentReady", Arguments, false);
}

void CClientWebBrowser::Events_OnLoadingFailed(const SString& strURL, int errorCode, const SString& errorDescription)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushNumber(errorCode);
    Arguments.PushString(errorDescription);
    CallEvent("onClientBrowserLoadingFailed", Arguments, false);
}

void CClientWebBrowser::Events_OnNavigate(const SString& strURL, bool bIsBlocked, bool bIsMainFrame)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushBoolean(bIsBlocked);
    Arguments.PushBoolean(bIsMainFrame);
    CallEvent("onClientBrowserNavigate", Arguments, false);
}

void CClientWebBrowser::Events_OnPopup(const SString& strTargetURL, const SString& strOpenerURL)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strTargetURL);
    Arguments.PushString(strOpenerURL);
    CallEvent("onClientBrowserPopup", Arguments, false);
}

void CClientWebBrowser::Events_OnChangeCursor(unsigned char ucCursor)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushNumber(ucCursor);
    CallEvent("onClientBrowserCursorChange", Arguments, false);
}

void CClientWebBrowser::Events_OnTriggerEvent(const SString& strEventName, const std::vector<std::string>& arguments)
{
    CLuaArguments Arguments;
    for (std::vector<std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
        Arguments.PushString(*iter);
    }

    bool bWasCancelled;
    CStaticFunctionDefinitions::TriggerEvent(strEventName, *this, Arguments, bWasCancelled);
}

void CClientWebBrowser::Events_OnTooltip(const SString& strTooltip)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushString(strTooltip);
    CallEvent("onClientBrowserTooltip", Arguments, false);
}

void CClientWebBrowser::Events_OnInputFocusChanged(bool bGainedFocus)
{
    if (m_bBeingDestroyed)
        return;
    
    CLuaArguments Arguments;
    Arguments.PushBoolean(bGainedFocus);
    CallEvent("onClientBrowserInputFocusChanged", Arguments, false);
}

bool CClientWebBrowser::Events_OnResourcePathCheck(SString& strURL)
{
    if (m_bBeingDestroyed)
        return false;

    // If no resource is set, we are allowed to use the requested file
    if (!m_pResource)
        return true;

    CResource* pTempResource = m_pResource;            // Make a copy to ignore a changed resource

    if (CResourceManager::ParseResourcePathInput(strURL, pTempResource, &strURL))
        return true;

    return false;
}

bool CClientWebBrowser::Events_OnResourceFileCheck(const SString& strPath, CBuffer& outFileData)
{
    if (m_bBeingDestroyed)
        return false;

    // If no resource is set, we do not require to verify the file
    if (!m_pResource)
        return true;

    auto pFile = g_pClientGame->GetResourceManager()->GetDownloadableResourceFile(strPath.ToLower());

    // If we did not download this file, it has been script or user generated, nothing to verify for us
    if (pFile == nullptr)
        return true;

    pFile->GenerateClientChecksum(outFileData);
    return pFile->DoesClientAndServerChecksumMatch();
}

void CClientWebBrowser::Events_OnResourceBlocked(const SString& strURL, const SString& strDomain, unsigned char reason)
{
    if (m_bBeingDestroyed)
        return;

    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushString(strDomain);
    Arguments.PushNumber(reason);
    CallEvent("onClientBrowserResourceBlocked", Arguments, false);
}

void CClientWebBrowser::Events_OnAjaxRequest(CAjaxResourceHandlerInterface* pHandler, const SString& strURL)
{
    if (m_bBeingDestroyed)
    {
        pHandler->SetResponse("");
        return;
    }

    auto callbackMapEntry = m_mapAjaxCallback.find(strURL);

    if (callbackMapEntry == m_mapAjaxCallback.end())
    {
        pHandler->SetResponse("");
        return;
    }

    auto&  callback = callbackMapEntry->second;
    SString result = callback(pHandler->GetGetData(), pHandler->GetPostData());
    pHandler->SetResponse(std::move(result));
}

void CClientWebBrowser::Events_OnConsoleMessage(const std::string& message, const std::string& source, int line, std::int16_t level)
{
    if (m_bBeingDestroyed)
        return;

    CLuaArguments arguments;
    arguments.PushString(message);
    arguments.PushString(source);
    arguments.PushNumber(line);
    arguments.PushNumber(level);
    CallEvent("onClientBrowserConsoleMessage", arguments, false);
}

bool CClientWebBrowser::AddAjaxHandler(const SString& strURL, ajax_callback_t& handler)
{
    if (!m_pWebView)
        return false;
    
    if (!m_pWebView->RegisterAjaxHandler(strURL))
        return false;

    m_mapAjaxCallback.insert(std::make_pair(strURL, handler));
    return true;
}

bool CClientWebBrowser::RemoveAjaxHandler(const SString& strURL)
{
    if (!m_pWebView)
        return false;
    
    if (!m_pWebView->UnregisterAjaxHandler(strURL))
        return false;

    return m_mapAjaxCallback.erase(strURL) == 1;
}

bool CClientWebBrowser::ToggleDevTools(bool visible)
{
    return m_pWebView ? m_pWebView->ToggleDevTools(visible) : false;
}

CClientGUIWebBrowser::CClientGUIWebBrowser(bool isLocal, bool isTransparent, uint width, uint height, CClientManager* pManager, CLuaMain* pLuaMain,
                                           CGUIElement* pCGUIElement, ElementID ID)
    : CClientGUIElement(pManager, pLuaMain, pCGUIElement, ID), m_pBrowser(nullptr)
{
    m_pManager = pManager;

    if (!g_pClientGame || !pLuaMain)
        return;

    CClientWebBrowser* pBrowser = g_pClientGame->GetManager()->GetRenderElementManager()->CreateWebBrowser(width, height, isLocal, isTransparent);

    if (pBrowser)
    {
        // Store immediately so destructor can always find it
        m_pBrowser = pBrowser;

        // Set our owner resource BEFORE transferring ownership to element tree
        pBrowser->SetResource(pLuaMain->GetResource());

        // Transfer ownership to element tree by setting parent
        // After this call, the element tree is responsible for cleanup
        pBrowser->SetParent(this);
    }
}

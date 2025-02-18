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
    SetTypeName("webbrowser");

    // Create the web view
    m_pWebView = g_pCore->GetWebCore()->CreateWebView(pWebBrowserItem->m_uiSizeX, pWebBrowserItem->m_uiSizeY, bLocal, pWebBrowserItem, bTransparent);

    // Set events interface
    m_pWebView->SetWebBrowserEvents(this);

    // Initialise the webview after setting the events interface
    m_pWebView->Initialise();
}

CClientWebBrowser::~CClientWebBrowser()
{
    g_pCore->GetWebCore()->DestroyWebView(m_pWebView);
    m_pWebView = nullptr;

    // Unlink from tree
    Unlink();
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
    return m_pWebView->IsLoading();
}

bool CClientWebBrowser::LoadURL(const SString& strURL, bool bFilterEnabled, const SString& strPostData, bool bURLEncoded)
{
    return m_pWebView->LoadURL(strURL, bFilterEnabled, strPostData, bURLEncoded);
}

const SString& CClientWebBrowser::GetTitle()
{
    return m_pWebView->GetTitle();
}

SString CClientWebBrowser::GetURL()
{
    return m_pWebView->GetURL();
}

void CClientWebBrowser::SetRenderingPaused(bool bPaused)
{
    m_pWebView->SetRenderingPaused(bPaused);
}

void CClientWebBrowser::Focus()
{
    m_pWebView->Focus();
}

bool CClientWebBrowser::ExecuteJavascript(const SString& strJavascriptCode)
{
    // Don't allow javascript code execution on remote websites
    if (!m_pWebView->IsLocal())
        return false;

    m_pWebView->ExecuteJavascript(strJavascriptCode);
    return true;
}

bool CClientWebBrowser::SetProperty(const SString& strKey, const SString& strValue)
{
    return m_pWebView->SetProperty(strKey, strValue);
}

bool CClientWebBrowser::GetProperty(const SString& strKey, SString& outValue)
{
    return m_pWebView->GetProperty(strKey, outValue);
}

void CClientWebBrowser::InjectMouseMove(int iPosX, int iPosY)
{
    m_pWebView->InjectMouseMove(iPosX, iPosY);
}

void CClientWebBrowser::InjectMouseDown(eWebBrowserMouseButton mouseButton, int count)
{
    m_pWebView->InjectMouseDown(mouseButton, count);
}

void CClientWebBrowser::InjectMouseUp(eWebBrowserMouseButton mouseButton)
{
    m_pWebView->InjectMouseUp(mouseButton);
}

void CClientWebBrowser::InjectMouseWheel(int iScrollVert, int iScrollHorz)
{
    m_pWebView->InjectMouseWheel(iScrollVert, iScrollHorz);
}

bool CClientWebBrowser::IsLocal()
{
    return m_pWebView->IsLocal();
}

float CClientWebBrowser::GetAudioVolume()
{
    return m_pWebView->GetAudioVolume();
}

bool CClientWebBrowser::SetAudioVolume(float fVolume)
{
    return m_pWebView->SetAudioVolume(fVolume);
}

void CClientWebBrowser::GetSourceCode(const std::function<void(const std::string& code)>& callback)
{
    return m_pWebView->GetSourceCode(callback);
}

void CClientWebBrowser::Resize(const CVector2D& size)
{
    m_pWebView->Resize(size);
}

bool CClientWebBrowser::CanGoBack()
{
    return m_pWebView->CanGoBack();
}

bool CClientWebBrowser::CanGoForward()
{
    return m_pWebView->CanGoForward();
}

bool CClientWebBrowser::GoBack()
{
    return m_pWebView->GoBack();
}

bool CClientWebBrowser::GoForward()
{
    return m_pWebView->GoForward();
}

void CClientWebBrowser::Refresh(bool bIgnoreCache)
{
    m_pWebView->Refresh(bIgnoreCache);
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//            CWebBrowserEventsInterface implementation                   //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
void CClientWebBrowser::Events_OnCreated()
{
    CLuaArguments Arguments;
    CallEvent("onClientBrowserCreated", Arguments, false);
}

void CClientWebBrowser::Events_OnLoadingStart(const SString& strURL, bool bMainFrame)
{
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushBoolean(bMainFrame);
    CallEvent("onClientBrowserLoadingStart", Arguments, false);
}

void CClientWebBrowser::Events_OnDocumentReady(const SString& strURL)
{
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    CallEvent("onClientBrowserDocumentReady", Arguments, false);
}

void CClientWebBrowser::Events_OnLoadingFailed(const SString& strURL, int errorCode, const SString& errorDescription)
{
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushNumber(errorCode);
    Arguments.PushString(errorDescription);
    CallEvent("onClientBrowserLoadingFailed", Arguments, false);
}

void CClientWebBrowser::Events_OnNavigate(const SString& strURL, bool bIsBlocked, bool bIsMainFrame)
{
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushBoolean(bIsBlocked);
    Arguments.PushBoolean(bIsMainFrame);
    CallEvent("onClientBrowserNavigate", Arguments, false);
}

void CClientWebBrowser::Events_OnPopup(const SString& strTargetURL, const SString& strOpenerURL)
{
    CLuaArguments Arguments;
    Arguments.PushString(strTargetURL);
    Arguments.PushString(strOpenerURL);
    CallEvent("onClientBrowserPopup", Arguments, false);
}

void CClientWebBrowser::Events_OnChangeCursor(unsigned char ucCursor)
{
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
    CLuaArguments Arguments;
    Arguments.PushString(strTooltip);
    CallEvent("onClientBrowserTooltip", Arguments, false);
}

void CClientWebBrowser::Events_OnInputFocusChanged(bool bGainedFocus)
{
    CLuaArguments Arguments;
    Arguments.PushBoolean(bGainedFocus);
    CallEvent("onClientBrowserInputFocusChanged", Arguments, false);
}

bool CClientWebBrowser::Events_OnResourcePathCheck(SString& strURL)
{
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
    CLuaArguments Arguments;
    Arguments.PushString(strURL);
    Arguments.PushString(strDomain);
    Arguments.PushNumber(reason);
    CallEvent("onClientBrowserResourceBlocked", Arguments, false);
}

void CClientWebBrowser::Events_OnAjaxRequest(CAjaxResourceHandlerInterface* pHandler, const SString& strURL)
{
    auto callbackMapEntry = m_mapAjaxCallback.find(strURL);

    if (callbackMapEntry == m_mapAjaxCallback.end())
    {
        pHandler->SetResponse("");
        return;
    }

    auto    callback = callbackMapEntry->second;
    SString result = callback(pHandler->GetGetData(), pHandler->GetPostData());
    pHandler->SetResponse(result);
}

void CClientWebBrowser::Events_OnConsoleMessage(const std::string& message, const std::string& source, int line, std::int16_t level)
{
    CLuaArguments arguments;
    arguments.PushString(message);
    arguments.PushString(source);
    arguments.PushNumber(line);
    arguments.PushNumber(level);
    CallEvent("onClientBrowserConsoleMessage", arguments, false);
}

bool CClientWebBrowser::AddAjaxHandler(const SString& strURL, ajax_callback_t& handler)
{
    if (!m_pWebView->RegisterAjaxHandler(strURL))
        return false;

    m_mapAjaxCallback.insert(std::make_pair(strURL, handler));
    return true;
}

bool CClientWebBrowser::RemoveAjaxHandler(const SString& strURL)
{
    if (!m_pWebView->UnregisterAjaxHandler(strURL))
        return false;

    return m_mapAjaxCallback.erase(strURL) == 1;
}

bool CClientWebBrowser::ToggleDevTools(bool visible)
{
    return m_pWebView->ToggleDevTools(visible);
}

CClientGUIWebBrowser::CClientGUIWebBrowser(bool isLocal, bool isTransparent, uint width, uint height, CClientManager* pManager, CLuaMain* pLuaMain,
                                           CGUIElement* pCGUIElement, ElementID ID)
    : CClientGUIElement(pManager, pLuaMain, pCGUIElement, ID)
{
    m_pManager = pManager;
    m_pBrowser = g_pClientGame->GetManager()->GetRenderElementManager()->CreateWebBrowser(width, height, isLocal, isTransparent);

    if (m_pBrowser)
    {
        m_pBrowser->SetParent(this);            // m_pBrowser gets deleted automatically by the element tree logic

        // Set our owner resource
        m_pBrowser->SetResource(pLuaMain->GetResource());
    }
}

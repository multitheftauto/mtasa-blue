/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBrowserDefs.cpp
 *  PURPOSE:     Lua browser definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include <regex>

void CLuaBrowserDefs::LoadFunctions()
{
    // Define browser functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createBrowser", ArgumentParserWarn<false, CreateBrowser>},
        {"requestBrowserDomains", ArgumentParserWarn<false, RequestBrowserDomains>},
        {"loadBrowserURL", ArgumentParserWarn<false, LoadBrowserURL>},
        {"isBrowserLoading", ArgumentParserWarn<nullptr, IsBrowserLoading>},
        {"injectBrowserMouseMove", ArgumentParserWarn<false, InjectBrowserMouseMove>},
        {"injectBrowserMouseDown", ArgumentParserWarn<false, InjectBrowserMouseDown>},
        {"injectBrowserMouseUp", ArgumentParserWarn<false, InjectBrowserMouseUp>},
        {"injectBrowserMouseWheel", ArgumentParserWarn<false, InjectBrowserMouseWheel>},
        {"getBrowserTitle", ArgumentParserWarn<false, GetBrowserTitle>},
        {"getBrowserURL", ArgumentParserWarn<false, GetBrowserURL>},
        {"setBrowserRenderingPaused", ArgumentParserWarn<false, SetBrowserRenderingPaused>},
        {"isBrowserRenderingPaused", ArgumentParser<IsBrowserRenderingPaused>},
        {"executeBrowserJavascript", ArgumentParserWarn<false, ExecuteBrowserJavascript>},
        {"getBrowserVolume", GetBrowserVolume},
        {"setBrowserVolume", ArgumentParserWarn<false, SetBrowserVolume>},
        {"isBrowserDomainBlocked", ArgumentParserWarn<nullptr, IsBrowserDomainBlocked>},
        {"focusBrowser", ArgumentParserWarn<false, FocusBrowser>},
        {"isBrowserFocused", ArgumentParserWarn<nullptr, IsBrowserFocused>},
        {"setBrowserProperty", ArgumentParserWarn<false, SetBrowserProperty>},
        {"getBrowserProperty", ArgumentParserWarn<nullptr, GetBrowserProperty>},
        {"getBrowserSettings", ArgumentParserWarn<false, GetBrowserSettings>},
        {"getBrowserSource", ArgumentParserWarn<false, GetBrowserSource>},
        {"setBrowserAjaxHandler", ArgumentParserWarn<false, SetBrowserAjaxHandler>},
        {"canBrowserNavigateBack", ArgumentParserWarn<nullptr, CanBrowserNavigateBack>},
        {"canBrowserNavigateForward", ArgumentParserWarn<nullptr, CanBrowserNavigateForward>},
        {"navigateBrowserBack", ArgumentParserWarn<nullptr, NavigateBrowserBack>},
        {"navigateBrowserForward", ArgumentParserWarn<nullptr, NavigateBrowserForward>},
        {"reloadBrowserPage", ArgumentParserWarn<nullptr, ReloadBrowserPage>},
        {"toggleBrowserDevTools", ArgumentParserWarn<false, ToggleBrowserDevTools>},
        {"resizeBrowser", ArgumentParserWarn<false, ResizeBrowser>},
        {"guiCreateBrowser", GUICreateBrowser},
        {"guiGetBrowser", GUIGetBrowser},
        {"isBrowserGPUEnabled", ArgumentParser<IsBrowserGPUEnabled>},
    };

    // Add browser functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaBrowserDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createBrowser");
    lua_classfunction(luaVM, "loadURL", "loadBrowserURL");
    lua_classfunction(luaVM, "isLoading", "isBrowserLoading");
    lua_classfunction(luaVM, "injectMouseMove", "injectBrowserMouseMove");
    lua_classfunction(luaVM, "injectMouseDown", "injectBrowserMouseDown");
    lua_classfunction(luaVM, "injectMouseUp", "injectBrowserMouseUp");
    lua_classfunction(luaVM, "injectMouseWheel", "injectBrowserMouseWheel");
    lua_classfunction(luaVM, "getTitle", "getBrowserTitle");
    lua_classfunction(luaVM, "getURL", "getBrowserURL");
    lua_classfunction(luaVM, "setRenderingPaused", "setBrowserRenderingPaused");
    lua_classfunction(luaVM, "isRenderingPaused", "isBrowserRenderingPaused");
    lua_classfunction(luaVM, "executeJavascript", "executeBrowserJavascript");
    lua_classfunction(luaVM, "getVolume", "getBrowserVolume");
    lua_classfunction(luaVM, "setVolume", "setBrowserVolume");
    lua_classfunction(luaVM, "focus", "focusBrowser");
    lua_classfunction(luaVM, "isFocused", "isBrowserFocused");
    lua_classfunction(luaVM, "setProperty", "setBrowserProperty");
    lua_classfunction(luaVM, "getProperty", "getBrowserProperty");
    lua_classfunction(luaVM, "getSource", "getBrowserSource");
    lua_classfunction(luaVM, "setAjaxHandler", "setBrowserAjaxHandler");
    lua_classfunction(luaVM, "canNavigateBack", "canBrowserNavigateBack");
    lua_classfunction(luaVM, "canNavigateForward", "canBrowserNavigateForward");
    lua_classfunction(luaVM, "navigateBack", "navigateBrowserBack");
    lua_classfunction(luaVM, "navigateForward", "navigateBrowserForward");
    lua_classfunction(luaVM, "reloadPage", "reloadBrowserPage");
    lua_classfunction(luaVM, "toggleDevTools", "toggleBrowserDevTools");
    lua_classfunction(luaVM, "resize", "resizeBrowser");

    lua_classfunction(luaVM, "requestDomains", "requestBrowserDomains");
    lua_classfunction(luaVM, "isDomainBlocked", "isBrowserDomainBlocked");

    lua_classvariable(luaVM, "url", "loadBrowserURL", "getBrowserURL");
    lua_classvariable(luaVM, "loading", nullptr, "isBrowserLoading");
    lua_classvariable(luaVM, "title", nullptr, "getBrowserTitle");
    lua_classvariable(luaVM, "renderingPaused", "setBrowserRenderingPaused", "isBrowserRenderingPaused");
    lua_classvariable(luaVM, "volume", "setBrowserVolume", "getBrowserVolume");
    lua_classvariable(luaVM, "devTools", "toggleBrowserDevTools", nullptr);
    lua_classvariable(luaVM, "gpuEnabled", nullptr, "isBrowserGPUEnabled");

    lua_registerclass(luaVM, "Browser", "DxTexture");

    // Add GUI browser class
    lua_newclass(luaVM);
    lua_classfunction(luaVM, "create", "guiCreateBrowser");
    lua_classfunction(luaVM, "getBrowser", "guiGetBrowser");
    lua_classvariable(luaVM, "browser", nullptr, "guiGetBrowser");
    lua_registerclass(luaVM, "GuiBrowser", "GuiElement");
}

std::variant<CClientWebBrowser*, bool> CLuaBrowserDefs::CreateBrowser(lua_State* luaVM, CVector2D vecSize, bool bIsLocal, std::optional<bool> bTransparent)
{
    //  texture createBrowser ( int width, int height, bool isLocal [, bool transparent = false] )
    if (vecSize.fX < 0)
        throw std::invalid_argument("Browser width is smaller than 0");
    else if (vecSize.fY < 0)
        throw std::invalid_argument("Browser height is smaller than 0");
    else if (vecSize.fX == 0 || vecSize.fY == 0)
        throw std::invalid_argument("A browser must be at least 1x1 in size.");

    const auto pWebCore = g_pCore->GetWebCore();
    if (!pWebCore)
        return false;

    if (!bIsLocal && !pWebCore->GetRemotePagesEnabled())
        return false;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pParentResource = pLuaMain->GetResource();

        CClientWebBrowser* pBrowserTexture =
            g_pClientGame->GetManager()->GetRenderElementManager()->CreateWebBrowser((int)vecSize.fX, (int)vecSize.fY, bIsLocal, bTransparent.value_or(false));
        if (pBrowserTexture)
        {
            // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            pBrowserTexture->SetParent(pParentResource->GetResourceDynamicEntity());

            // Set our owner resource
            pBrowserTexture->SetResource(pParentResource);
        }
        return pBrowserTexture;
    }

    return false;
}

bool CLuaBrowserDefs::RequestBrowserDomains(lua_State* luaVM, std::vector<std::string> pages, std::optional<bool> bIsURL,
                                            std::optional<CLuaFunctionRef> callbackFunction)
{
    //  bool requestBrowserDomains ( table domains, bool isURL [, function callback ] )
    std::vector<SString> sPages(pages.begin(), pages.end());

    // Remove whitespaces
    for (auto& url : sPages)
        url.erase(std::remove_if(url.begin(), url.end(), [](unsigned char c) { return std::isspace(c); }), url.end());

    // Remove empty and invalid URLs
    std::regex invalidSynmbolsRegex("[^A-Za-z0-9._~!#$&'()*+,;=:@/?%-]");

    sPages.erase(std::remove_if(sPages.begin(), sPages.end(),
                                [&invalidSynmbolsRegex](const auto& url) { return url.empty() || std::regex_search(url, invalidSynmbolsRegex); }),
                 sPages.end());

    // Convert to domains if we got a list of URLs
    if (bIsURL.value_or(false))
    {
        auto pWebCore = g_pCore->GetWebCore();
        if (!pWebCore)
            return false;
        std::transform(sPages.begin(), sPages.end(), sPages.begin(), [pWebCore](const auto& url) { return pWebCore->GetDomainFromURL(url); });
    }

    CLuaFunctionRef functionRef = callbackFunction.value_or(CLuaFunctionRef());
    WebRequestCallback callback = [=](bool bAllow, const std::unordered_set<SString>& domains)
    {
        // Test if luaVM is still available
        if (m_pLuaManager->IsLuaVMValid(luaVM) && VERIFY_FUNCTION(functionRef))
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (!pLuaMain)
                return;

            CLuaArguments arguments;
            arguments.PushBoolean(bAllow);

            CLuaArguments LuaTable;
            int           i = 0;
            for (const auto& domain : domains)
            {
                LuaTable.PushNumber(++i);
                LuaTable.PushString(domain);
            }
            arguments.PushTable(&LuaTable);
            arguments.Call(pLuaMain, functionRef);
        }
    };

    auto pWebCore = g_pCore->GetWebCore();
    if (!pWebCore)
        return false;
    pWebCore->RequestPages(sPages, VERIFY_FUNCTION(functionRef) ? &callback : nullptr);
    return true;
}

bool CLuaBrowserDefs::LoadBrowserURL(CClientWebBrowser* pWebBrowser, const std::string strURL, std::optional<std::string> strPostData,
                                    std::optional<bool> bURLEncoded)
{
    //  bool loadBrowserURL ( browser webBrowser, string url [, string postData = "", bool postURLEncoded = true ] )
    // Are we dealing with a remote website?
    if (strURL.substr(0, 7) == "http://" || strURL.substr(0, 8) == "https://")
    {
        bool isLocalURL = strURL.substr(0, 11) == "http://mta/";
        if (pWebBrowser->IsLocal() != isLocalURL)
            return false;

        return pWebBrowser->LoadURL(strURL, !isLocalURL, strPostData.value_or(""), bURLEncoded.value_or(true));
    }

    throw std::invalid_argument("Invalid URL scheme provided. Only http:// and https:// is supported.");
}

bool CLuaBrowserDefs::IsBrowserLoading(CClientWebBrowser* pWebBrowser)
{
    //  bool isBrowserLoading(browser webBrowser)
    return pWebBrowser->IsLoading();
}

bool CLuaBrowserDefs::InjectBrowserMouseMove(CClientWebBrowser* pWebBrowser, CVector2D vecPosition)
{
    //  bool injectBrowserMouseMove(browser webBrowser, int x, int y)
    pWebBrowser->InjectMouseMove((int)vecPosition.fX, (int)vecPosition.fY);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseDown(CClientWebBrowser* pWebBrowser, eWebBrowserMouseButton mouseButton, std::optional<bool> doubleClick)
{
    //  bool injectBrowserMouseDown ( browser webBrowser, string mouseButton [ , bool doubleClick = false ] )
    pWebBrowser->InjectMouseDown(mouseButton, doubleClick.value_or(false) ? 2 : 1);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseUp(CClientWebBrowser* pWebBrowser, eWebBrowserMouseButton mouseButton)
{
    //  bool injectBrowserMouseUp ( browser webBrowser, string mouseButton )
    pWebBrowser->InjectMouseUp(mouseButton);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseWheel(CClientWebBrowser* pWebBrowser, int iScrollVert, int iScrollHorz)
{
    //  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    pWebBrowser->InjectMouseWheel(iScrollVert, iScrollHorz);
    return true;
}

SString CLuaBrowserDefs::GetBrowserTitle(CClientWebBrowser* pWebBrowser)
{
    //  string getBrowserPageTitle ( browser webBrowser )
    return pWebBrowser->GetTitle();
}

SString CLuaBrowserDefs::GetBrowserURL(CClientWebBrowser* pWebBrowser)
{
    //  string getBrowserURL ( browser webBrowser )
    return pWebBrowser->GetURL();
}

bool CLuaBrowserDefs::SetBrowserRenderingPaused(CClientWebBrowser* pWebBrowser, bool bPaused)
{
    //  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    pWebBrowser->SetRenderingPaused(bPaused);
    return true;
}

bool CLuaBrowserDefs::IsBrowserRenderingPaused(CClientWebBrowser* browser)
{
    return browser->GetRenderingPaused();
}

bool CLuaBrowserDefs::ExecuteBrowserJavascript(CClientWebBrowser* pWebBrowser, const std::string strJavascriptCode)
{
    //  bool executeBrowserJavascript ( browser webBrowser, string jsCode )
    if (strJavascriptCode.empty() || pWebBrowser->ExecuteJavascript(strJavascriptCode))
        return true;

    throw std::invalid_argument("This function does not work with remote browsers");
}

int CLuaBrowserDefs::GetBrowserVolume(lua_State* luaVM)
{
    //  float getBrowserVolume ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pWebBrowser->GetAudioVolume());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaBrowserDefs::SetBrowserVolume(std::variant<CClientWebBrowser*, float> webBrowserOrVolume, std::optional<float> fVolume)
{
    //  bool setBrowserVolume ( float volume )
    //  bool setBrowserVolume ( browser webBrowser, float volume )
    if (std::holds_alternative<float>(webBrowserOrVolume))
    {
        auto pWebCore = g_pCore->GetWebCore();
        return pWebCore ? pWebCore->SetGlobalAudioVolume(std::get<float>(webBrowserOrVolume)) : false;
    }

    return std::get<CClientWebBrowser*>(webBrowserOrVolume)->SetAudioVolume(fVolume.value_or(0.0f));
}

std::optional<bool> CLuaBrowserDefs::IsBrowserDomainBlocked(const std::string strURL, std::optional<bool> bIsURL)
{
    //  bool isBrowserDomainBlocked ( string domain, bool isURL )
    auto pWebCore = g_pCore->GetWebCore();
    if (!pWebCore)
        return std::nullopt;

    SString strDomain = SString(strURL);
    if (bIsURL.value_or(false))
        strDomain = pWebCore->GetDomainFromURL(strDomain);

    if (!strDomain.empty())
        return pWebCore->GetDomainState(strDomain) != eURLState::WEBPAGE_ALLOWED;

    return std::nullopt;
}

bool CLuaBrowserDefs::FocusBrowser(std::optional<CClientWebBrowser*> pWebBrowser)
{
    //  focusBrowser ( browser webBrowser )
    if (!pWebBrowser.has_value())
    {
        auto pWebCore = g_pCore->GetWebCore();
        if (pWebCore)
            pWebCore->SetFocusedWebView(NULL);
        return true;
    }

    pWebBrowser.value()->Focus();
    return true;
}

bool CLuaBrowserDefs::IsBrowserFocused(CClientWebBrowser* pWebBrowser)
{
    //  browser isBrowserFocused ( browser webBrowser )
    auto               pWebCore = g_pCore->GetWebCore();
    CWebViewInterface* pWebView = pWebCore ? pWebCore->GetFocusedWebView() : nullptr;
    return pWebBrowser->GetWebView() == pWebView;
}

bool CLuaBrowserDefs::SetBrowserProperty(CClientWebBrowser* pWebBrowser, const std::string strKey, const std::string strValue)
{
    //  bool setBrowserProperty ( browser webBrowser, string key, string value )
    return pWebBrowser->SetProperty(strKey, strValue);
}

std::optional<SString> CLuaBrowserDefs::GetBrowserProperty(CClientWebBrowser* pWebBrowser, const std::string strKey)
{
    //  string getBrowserProperty ( browser webBrowser, string key )
    SString strValue;
    if (pWebBrowser->GetProperty(strKey, strValue))
        return strValue;

    return std::nullopt;
}

std::unordered_map<std::string, bool> CLuaBrowserDefs::GetBrowserSettings()
{
    //  table getBrowserSettings ()
    auto pWebCore = g_pCore->GetWebCore();

    return {
        {"RemoteEnabled", pWebCore ? pWebCore->GetRemotePagesEnabled() : false},
        {"RemoteJavascript", pWebCore ? pWebCore->GetRemoteJavascriptEnabled() : false},
        {"PluginsEnabled", false},
    };
}

bool CLuaBrowserDefs::GetBrowserSource(lua_State* luaVM, CClientWebBrowser* pWebBrowser, CLuaFunctionRef callbackFunction)
{
    //  bool getBrowserSource ( browser webBrowser, function callback )
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain && VERIFY_FUNCTION(callbackFunction))
    {
        pWebBrowser->GetSourceCode(
            [callbackFunction, pLuaMain, pWebBrowser](const std::string& code)
            {
                /*
                This function should not be called when the resource is about to stop as
                stopping the resource destroys the browser element and thus cancels the
                CefStringVisitor callback class (see CWebView::GetSourceCode::MyStringVisitor)
                */
                if (VERIFY_FUNCTION(callbackFunction))
                {
                    CLuaArguments arguments;
                    // TODO: Use SCharStringRef/direct string access instead of copying strings around
                    arguments.PushString(code);
                    arguments.PushElement(pWebBrowser);
                    arguments.Call(pLuaMain, callbackFunction);
                }
            });

        return true;
    }

    return false;
}

bool CLuaBrowserDefs::ToggleBrowserDevTools(CClientWebBrowser* pWebBrowser, bool visible)
{
    //  bool toggleBrowserDevTools ( browser webBrowser, bool visible )
    auto pWebCore = g_pCore->GetWebCore();
    if (pWebCore && pWebCore->IsTestModeEnabled())
        return pWebBrowser->ToggleDevTools(visible);

    throw std::invalid_argument("toggleBrowserDevtools can only be used in development mode");
}

bool CLuaBrowserDefs::ResizeBrowser(CClientWebBrowser* pWebBrowser, CVector2D size)
{
    //  bool resizeBrowser(browser webBrowser, float width, float height)
    pWebBrowser->Resize(size);
    return true;
}

bool CLuaBrowserDefs::CanBrowserNavigateBack(CClientWebBrowser* pWebBrowser)
{
    //  bool canBrowserNavigateBack( browser webBrowser )
    return pWebBrowser->CanGoBack();
}

bool CLuaBrowserDefs::CanBrowserNavigateForward(CClientWebBrowser* pWebBrowser)
{
    //  bool canBrowserNavigateForward( browser webBrowser )
    return pWebBrowser->CanGoForward();
}

bool CLuaBrowserDefs::NavigateBrowserBack(CClientWebBrowser* pWebBrowser)
{
    //  bool navigateBrowserBack( browser webBrowser )
    return pWebBrowser->GoBack();
}

bool CLuaBrowserDefs::NavigateBrowserForward(CClientWebBrowser* pWebBrowser)
{
    //  bool navigateBrowserForward( browser webBrowser )
    return pWebBrowser->GoForward();
}

bool CLuaBrowserDefs::ReloadBrowserPage(CClientWebBrowser* pWebBrowser, std::optional<bool> bIgnoreCache)
{
    //  bool reloadBrowserPage( browser webBrowser [, bool ignoreCache = false] )
    pWebBrowser->Refresh(bIgnoreCache.value_or(false));
    return true;
}

int CLuaBrowserDefs::GUICreateBrowser(lua_State* luaVM)
{
    //  element guiCreateBrowser ( float x, float y, float width, float height, bool isLocal, bool isTransparent, bool relative, [element parent = nil] )
    CVector2D          position;
    CVector2D          size;
    bool               bIsLocal;
    bool               bIsTransparent;
    bool               bIsRelative;
    CClientGUIElement* parent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(position);
    argStream.ReadVector2D(size);
    argStream.ReadBool(bIsLocal);
    argStream.ReadBool(bIsTransparent);
    argStream.ReadBool(bIsRelative, false);
    argStream.ReadUserData(parent, nullptr);

    if (!argStream.HasErrors())
    {
        if (size.fX < 0)
        {
            argStream.SetCustomError("Browser width is smaller than 0", "Invalid parameter");
        }
        else if (size.fY < 0)
        {
            argStream.SetCustomError("Browser height is smaller than 0", "Invalid parameter");
        }
        else if (size.fX == 0 || size.fY == 0)
        {
            argStream.SetCustomError("A browser must be at least 1x1 in size.", "Invalid parameter");
        }
    }

    if (!argStream.HasErrors())
    {
        auto pWebCore = g_pCore->GetWebCore();
        if (!bIsLocal && (!pWebCore || !pWebCore->GetRemotePagesEnabled()))
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CClientGUIElement* pGUIElement =
                CStaticFunctionDefinitions::GUICreateBrowser(*pLuaMain, position, size, bIsLocal, bIsTransparent, bIsRelative, parent);

            if (pGUIElement)
            {
                lua_pushelement(luaVM, pGUIElement);
                return 1;
            }
            else
            {
                argStream.SetCustomError("Failed to create browser element", "Create browser");
            }
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::GUIGetBrowser(lua_State* luaVM)  // Or rather guiGetBrowserBrowser?
{
    //  webbrowser guiGetBrowser ( gui-webbrowser browser )
    CClientGUIElement* pGUIElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData<CGUIWebBrowser>(pGUIElement);

    if (!argStream.HasErrors())
    {
        if (IS_GUI(pGUIElement) && pGUIElement->GetCGUIType() == CGUI_WEBBROWSER)
        {
            CClientGUIWebBrowser* pGUIBrowser = static_cast<CClientGUIWebBrowser*>(pGUIElement);
            lua_pushelement(luaVM, pGUIBrowser->GetBrowser());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaBrowserDefs::SetBrowserAjaxHandler(lua_State* luaVM, CClientWebBrowser* pWebBrowser, const std::string strURL,
                                            std::optional<CLuaFunctionRef> callbackFunction)
{
    //  bool setBrowserAjaxHandler ( browser browser, string URL[, function callback] )
    if (!callbackFunction.has_value())
        return pWebBrowser->RemoveAjaxHandler(strURL);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain && VERIFY_FUNCTION(callbackFunction.value()))
    {
        CResource*        pResource = pLuaMain->GetResource();
        CResourceManager* pResourceManager = m_pResourceManager;
        auto              netId = pResource->GetNetID();

        bool bResult = pWebBrowser->AddAjaxHandler(strURL,
                                                   [=](std::vector<std::string>& vecGet, std::vector<std::string>& vecPost) -> const std::string
                                                   {
                                                       // Make sure the resource is still running
                                                       if (!pResourceManager->Exists(pResource) || pResource->GetNetID() != netId)
                                                       {
                                                           return "";
                                                       }

                                                       // Make sure the function is valid
                                                       if (VERIFY_FUNCTION(callbackFunction.value()))
                                                       {
                                                           CLuaArguments arguments;
                                                           CLuaArguments getArguments;
                                                           CLuaArguments postArguments;

                                                           for (auto&& param : vecGet)
                                                               getArguments.PushString(param);

                                                           for (auto&& param : vecPost)
                                                               postArguments.PushString(param);

                                                           arguments.PushTable(&getArguments);
                                                           arguments.PushTable(&postArguments);

                                                           CLuaArguments result;

                                                           arguments.Call(pLuaMain, callbackFunction.value(), &result);

                                                           if (result.IsEmpty())
                                                               return "";

                                                           CLuaArgument* returnedValue = *result.begin();
                                                           if (returnedValue->GetType() == LUA_TSTRING)
                                                               return std::string(returnedValue->GetString());
                                                           else
                                                               return "";
                                                       }
                                                       else
                                                           return "";
                                                   });

        return bResult;
    }

    return false;
}

bool CLuaBrowserDefs::IsBrowserGPUEnabled() noexcept
{
    auto pWebCore = g_pCore->GetWebCore();
    return pWebCore ? pWebCore->GetGPUEnabled() : false;
}

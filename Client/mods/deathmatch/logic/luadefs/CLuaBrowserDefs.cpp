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

std::variant<CClientWebBrowser*, bool> CLuaBrowserDefs::CreateBrowser(lua_State* luaVM, CVector2D size, bool isLocal, std::optional<bool> transparent)
{
    //  texture createBrowser ( int width, int height, bool isLocal [, bool transparent = false] )
    if (size.fX < 0)
        throw std::invalid_argument("Browser width is smaller than 0");
    else if (size.fY < 0)
        throw std::invalid_argument("Browser height is smaller than 0");
    else if (size.fX == 0 || size.fY == 0)
        throw std::invalid_argument("A browser must be at least 1x1 in size.");

    const auto webCore = g_pCore->GetWebCore();
    if (!webCore)
        return false;

    if (!isLocal && !webCore->GetRemotePagesEnabled())
        return false;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pParentResource = pLuaMain->GetResource();

        CClientWebBrowser* browserTexture =
            g_pClientGame->GetManager()->GetRenderElementManager()->CreateWebBrowser((int)size.fX, (int)size.fY, isLocal, transparent.value_or(false));
        if (browserTexture)
        {
            // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            browserTexture->SetParent(pParentResource->GetResourceDynamicEntity());

            // Set our owner resource
            browserTexture->SetResource(pParentResource);
        }
        return browserTexture;
    }

    return false;
}

bool CLuaBrowserDefs::RequestBrowserDomains(lua_State* luaVM, std::vector<std::string> pages, std::optional<bool> isURL,
                                            std::optional<CLuaFunctionRef> callbackFunction)
{
    //  bool requestBrowserDomains ( table domains, bool isURL [, function callback ] )
    std::vector<SString> urls(pages.begin(), pages.end());

    // Remove whitespaces
    for (auto& url : urls)
        url.erase(std::remove_if(url.begin(), url.end(), [](unsigned char c) { return std::isspace(c); }), url.end());

    // Remove empty and invalid URLs
    std::regex invalidSynmbolsRegex("[^A-Za-z0-9._~!#$&'()*+,;=:@/?%-]");

    urls.erase(std::remove_if(urls.begin(), urls.end(),
                              [&invalidSynmbolsRegex](const auto& url) { return url.empty() || std::regex_search(url, invalidSynmbolsRegex); }),
               urls.end());

    // Convert to domains if we got a list of URLs
    if (isURL.value_or(false))
    {
        auto webCore = g_pCore->GetWebCore();
        if (!webCore)
            return false;
        std::transform(urls.begin(), urls.end(), urls.begin(), [webCore](const auto& url) { return webCore->GetDomainFromURL(url); });
    }

    CLuaFunctionRef    functionRef = callbackFunction.value_or(CLuaFunctionRef());
    WebRequestCallback callback = [=](bool allow, const std::unordered_set<SString>& domains)
    {
        // Test if luaVM is still available
        if (m_pLuaManager->IsLuaVMValid(luaVM) && VERIFY_FUNCTION(functionRef))
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (!pLuaMain)
                return;

            CLuaArguments arguments;
            arguments.PushBoolean(allow);

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

    auto webCore = g_pCore->GetWebCore();
    if (!webCore)
        return false;
    webCore->RequestPages(urls, VERIFY_FUNCTION(functionRef) ? &callback : nullptr);
    return true;
}

bool CLuaBrowserDefs::LoadBrowserURL(CClientWebBrowser* browser, const std::string url, std::optional<std::string> postData, std::optional<bool> urlEncoded)
{
    //  bool loadBrowserURL ( browser webBrowser, string url [, string postData = "", bool postURLEncoded = true ] )
    // Are we dealing with a remote website?
    if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://")
    {
        bool isLocalURL = url.substr(0, 11) == "http://mta/";
        if (browser->IsLocal() != isLocalURL)
            return false;

        return browser->LoadURL(url, !isLocalURL, postData.value_or(""), urlEncoded.value_or(true));
    }

    throw std::invalid_argument("Invalid URL scheme provided. Only http:// and https:// is supported.");
}

bool CLuaBrowserDefs::IsBrowserLoading(CClientWebBrowser* browser)
{
    //  bool isBrowserLoading(browser webBrowser)
    return browser->IsLoading();
}

bool CLuaBrowserDefs::InjectBrowserMouseMove(CClientWebBrowser* browser, CVector2D position)
{
    //  bool injectBrowserMouseMove(browser webBrowser, int x, int y)
    browser->InjectMouseMove((int)position.fX, (int)position.fY);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseDown(CClientWebBrowser* browser, eWebBrowserMouseButton mouseButton, std::optional<bool> doubleClick)
{
    //  bool injectBrowserMouseDown ( browser webBrowser, string mouseButton [ , bool doubleClick = false ] )
    browser->InjectMouseDown(mouseButton, doubleClick.value_or(false) ? 2 : 1);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseUp(CClientWebBrowser* browser, eWebBrowserMouseButton mouseButton)
{
    //  bool injectBrowserMouseUp ( browser webBrowser, string mouseButton )
    browser->InjectMouseUp(mouseButton);
    return true;
}

bool CLuaBrowserDefs::InjectBrowserMouseWheel(CClientWebBrowser* browser, int scrollVert, int scrollHorz)
{
    //  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    browser->InjectMouseWheel(scrollVert, scrollHorz);
    return true;
}

std::string CLuaBrowserDefs::GetBrowserTitle(CClientWebBrowser* browser)
{
    //  string getBrowserPageTitle ( browser webBrowser )
    return browser->GetTitle();
}

std::string CLuaBrowserDefs::GetBrowserURL(CClientWebBrowser* browser)
{
    //  string getBrowserURL ( browser webBrowser )
    return browser->GetURL();
}

bool CLuaBrowserDefs::SetBrowserRenderingPaused(CClientWebBrowser* browser, bool paused)
{
    //  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    browser->SetRenderingPaused(paused);
    return true;
}

bool CLuaBrowserDefs::IsBrowserRenderingPaused(CClientWebBrowser* browser)
{
    return browser->GetRenderingPaused();
}

bool CLuaBrowserDefs::ExecuteBrowserJavascript(CClientWebBrowser* browser, const std::string javascriptCode)
{
    //  bool executeBrowserJavascript ( browser webBrowser, string jsCode )
    if (javascriptCode.empty() || browser->ExecuteJavascript(javascriptCode))
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

bool CLuaBrowserDefs::SetBrowserVolume(std::variant<CClientWebBrowser*, float> webBrowserOrVolume, std::optional<float> volume)
{
    //  bool setBrowserVolume ( float volume )
    //  bool setBrowserVolume ( browser webBrowser, float volume )
    if (std::holds_alternative<float>(webBrowserOrVolume))
    {
        auto webCore = g_pCore->GetWebCore();
        return webCore ? webCore->SetGlobalAudioVolume(std::get<float>(webBrowserOrVolume)) : false;
    }

    return std::get<CClientWebBrowser*>(webBrowserOrVolume)->SetAudioVolume(volume.value_or(0.0f));
}

std::optional<bool> CLuaBrowserDefs::IsBrowserDomainBlocked(const std::string url, std::optional<bool> isURL)
{
    //  bool isBrowserDomainBlocked ( string domain, bool isURL )
    auto webCore = g_pCore->GetWebCore();
    if (!webCore)
        return std::nullopt;

    SString domain = SString(url);
    if (isURL.value_or(false))
        domain = webCore->GetDomainFromURL(domain);

    if (!domain.empty())
        return webCore->GetDomainState(domain) != eURLState::WEBPAGE_ALLOWED;

    return std::nullopt;
}

bool CLuaBrowserDefs::FocusBrowser(std::optional<CClientWebBrowser*> browser)
{
    //  focusBrowser ( browser webBrowser )
    if (!browser.has_value())
    {
        auto webCore = g_pCore->GetWebCore();
        if (webCore)
            webCore->SetFocusedWebView(nullptr);
        return true;
    }

    browser.value()->Focus();
    return true;
}

bool CLuaBrowserDefs::IsBrowserFocused(CClientWebBrowser* browser)
{
    //  browser isBrowserFocused ( browser webBrowser )
    auto               webCore = g_pCore->GetWebCore();
    CWebViewInterface* webView = webCore ? webCore->GetFocusedWebView() : nullptr;
    return browser->GetWebView() == webView;
}

bool CLuaBrowserDefs::SetBrowserProperty(CClientWebBrowser* browser, const std::string key, const std::string value)
{
    //  bool setBrowserProperty ( browser webBrowser, string key, string value )
    return browser->SetProperty(key, value);
}

std::optional<std::string> CLuaBrowserDefs::GetBrowserProperty(CClientWebBrowser* browser, const std::string key)
{
    //  string getBrowserProperty ( browser webBrowser, string key )
    SString value;
    if (browser->GetProperty(key, value))
        return value;

    return std::nullopt;
}

auto CLuaBrowserDefs::GetBrowserSettings()
{
    //  table getBrowserSettings ()
    auto webCore = g_pCore->GetWebCore();

    std::unordered_map<std::string, bool> settings{
        {"RemoteEnabled", webCore ? webCore->GetRemotePagesEnabled() : false},
        {"RemoteJavascript", webCore ? webCore->GetRemoteJavascriptEnabled() : false},
        {"PluginsEnabled", false},
    };
    return settings;
}

bool CLuaBrowserDefs::GetBrowserSource(lua_State* luaVM, CClientWebBrowser* browser, CLuaFunctionRef callbackFunction)
{
    //  bool getBrowserSource ( browser webBrowser, function callback )
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain && VERIFY_FUNCTION(callbackFunction))
    {
        browser->GetSourceCode(
            [callbackFunction, pLuaMain, browser](const std::string& code)
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
                    arguments.PushElement(browser);
                    arguments.Call(pLuaMain, callbackFunction);
                }
            });

        return true;
    }

    return false;
}

bool CLuaBrowserDefs::ToggleBrowserDevTools(CClientWebBrowser* browser, bool visible)
{
    //  bool toggleBrowserDevTools ( browser webBrowser, bool visible )
    auto webCore = g_pCore->GetWebCore();
    if (webCore && webCore->IsTestModeEnabled())
        return browser->ToggleDevTools(visible);

    throw std::invalid_argument("toggleBrowserDevtools can only be used in development mode");
}

bool CLuaBrowserDefs::ResizeBrowser(CClientWebBrowser* browser, CVector2D size)
{
    //  bool resizeBrowser(browser webBrowser, float width, float height)
    browser->Resize(size);
    return true;
}

bool CLuaBrowserDefs::CanBrowserNavigateBack(CClientWebBrowser* browser)
{
    //  bool canBrowserNavigateBack( browser webBrowser )
    return browser->CanGoBack();
}

bool CLuaBrowserDefs::CanBrowserNavigateForward(CClientWebBrowser* browser)
{
    //  bool canBrowserNavigateForward( browser webBrowser )
    return browser->CanGoForward();
}

bool CLuaBrowserDefs::NavigateBrowserBack(CClientWebBrowser* browser)
{
    //  bool navigateBrowserBack( browser webBrowser )
    return browser->GoBack();
}

bool CLuaBrowserDefs::NavigateBrowserForward(CClientWebBrowser* browser)
{
    //  bool navigateBrowserForward( browser webBrowser )
    return browser->GoForward();
}

bool CLuaBrowserDefs::ReloadBrowserPage(CClientWebBrowser* browser, std::optional<bool> ignoreCache)
{
    //  bool reloadBrowserPage( browser webBrowser [, bool ignoreCache = false] )
    browser->Refresh(ignoreCache.value_or(false));
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

bool CLuaBrowserDefs::SetBrowserAjaxHandler(lua_State* luaVM, CClientWebBrowser* browser, const std::string url,
                                            std::optional<CLuaFunctionRef> callbackFunction)
{
    //  bool setBrowserAjaxHandler ( browser browser, string URL[, function callback] )
    if (!callbackFunction.has_value())
        return browser->RemoveAjaxHandler(url);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain && VERIFY_FUNCTION(callbackFunction.value()))
    {
        CResource*        pResource = pLuaMain->GetResource();
        CResourceManager* pResourceManager = m_pResourceManager;
        auto              netId = pResource->GetNetID();

        bool bResult = browser->AddAjaxHandler(url,
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
    auto webCore = g_pCore->GetWebCore();
    return webCore ? webCore->GetGPUEnabled() : false;
}

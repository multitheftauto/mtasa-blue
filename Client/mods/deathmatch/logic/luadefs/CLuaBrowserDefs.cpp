/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBrowserDefs.cpp
 *  PURPOSE:     Lua browser definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include <regex>

void CLuaBrowserDefs::LoadFunctions()
{
    // Define browser functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createBrowser", CreateBrowser},
        {"requestBrowserDomains", RequestBrowserDomains},
        {"loadBrowserURL", LoadBrowserURL},
        {"isBrowserLoading", IsBrowserLoading},
        {"injectBrowserMouseMove", InjectBrowserMouseMove},
        {"injectBrowserMouseDown", InjectBrowserMouseDown},
        {"injectBrowserMouseUp", InjectBrowserMouseUp},
        {"injectBrowserMouseWheel", InjectBrowserMouseWheel},
        {"getBrowserTitle", GetBrowserTitle},
        {"getBrowserURL", GetBrowserURL},
        {"setBrowserRenderingPaused", SetBrowserRenderingPaused},
        {"isBrowserRenderingPaused", ArgumentParser<IsBrowserRenderingPaused>},
        {"executeBrowserJavascript", ExecuteBrowserJavascript},
        {"getBrowserVolume", GetBrowserVolume},
        {"setBrowserVolume", SetBrowserVolume},
        {"isBrowserDomainBlocked", IsBrowserDomainBlocked},
        {"focusBrowser", FocusBrowser},
        {"isBrowserFocused", IsBrowserFocused},
        {"setBrowserProperty", SetBrowserProperty},
        {"getBrowserProperty", GetBrowserProperty},
        {"getBrowserSettings", GetBrowserSettings},
        {"getBrowserSource", GetBrowserSource},
        {"setBrowserAjaxHandler", SetBrowserAjaxHandler},
        {"canBrowserNavigateBack", CanBrowserNavigateBack},
        {"canBrowserNavigateForward", CanBrowserNavigateForward},
        {"navigateBrowserBack", NavigateBrowserBack},
        {"navigateBrowserForward", NavigateBrowserForward},
        {"reloadBrowserPage", ReloadBrowserPage},
        {"toggleBrowserDevTools", ToggleBrowserDevTools},
        {"resizeBrowser", ResizeBrowser},
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

int CLuaBrowserDefs::CreateBrowser(lua_State* luaVM)
{
    //  texture createBrowser ( int width, int height, bool isLocal [, bool transparent = false] )
    CVector2D vecSize;
    bool      bIsLocal;
    bool      bTransparent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecSize);
    argStream.ReadBool(bIsLocal);
    argStream.ReadBool(bTransparent, false);

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0)
        {
            argStream.SetCustomError("Browser width is smaller than 0", "Invalid parameter");
        }
        else if (vecSize.fY < 0)
        {
            argStream.SetCustomError("Browser height is smaller than 0", "Invalid parameter");
        }
        else if (vecSize.fX == 0 || vecSize.fY == 0)
        {
            argStream.SetCustomError("A browser must be at least 1x1 in size.", "Invalid parameter");
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (!bIsLocal && !g_pCore->GetWebCore()->GetRemotePagesEnabled())
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pParentResource = pLuaMain->GetResource();

        CClientWebBrowser* pBrowserTexture =
            g_pClientGame->GetManager()->GetRenderElementManager()->CreateWebBrowser((int)vecSize.fX, (int)vecSize.fY, bIsLocal, bTransparent);
        if (pBrowserTexture)
        {
            // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            pBrowserTexture->SetParent(pParentResource->GetResourceDynamicEntity());

            // Set our owner resource
            pBrowserTexture->SetResource(pParentResource);
        }
        lua_pushelement(luaVM, pBrowserTexture);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::RequestBrowserDomains(lua_State* luaVM)
{
    //  bool requestBrowserDomains ( table domains, bool isURL [, function callback ] )
    std::vector<SString> pages;
    bool                 bIsURL;
    CLuaFunctionRef      callbackFunction;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringTable(pages);
    argStream.ReadBool(bIsURL, false);
    argStream.ReadFunction(callbackFunction, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Remove empty and invalid URLs
        std::regex invalidSynmbolsRegex("[^A-Za-z0-9\-._~!#$&'()*+,;=:@\/?%]");

        pages.erase(std::remove_if(pages.begin(), pages.end(),
                                   [&invalidSynmbolsRegex](const auto& url) { return url.empty() || std::regex_search(url, invalidSynmbolsRegex); }),
                    pages.end());

        // Convert to domains if we got a list of URLs
        if (bIsURL)
            std::transform(pages.begin(), pages.end(), pages.begin(), [](const auto& url) { return g_pCore->GetWebCore()->GetDomainFromURL(url); });

        WebRequestCallback callback = [=](bool bAllow, const std::unordered_set<SString>& domains) {
            // Test if luaVM is still available
            if (m_pLuaManager->IsLuaVMValid(luaVM) && VERIFY_FUNCTION(callbackFunction))
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
                arguments.Call(pLuaMain, callbackFunction);
            }
        };

        g_pCore->GetWebCore()->RequestPages(pages, VERIFY_FUNCTION(callbackFunction) ? &callback : nullptr);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::LoadBrowserURL(lua_State* luaVM)
{
    //  bool loadBrowserURL ( browser webBrowser, string url [, string postData = "", bool postURLEncoded = true ] )
    CClientWebBrowser* pWebBrowser;
    SString            strURL;
    SString            strPostData;
    bool               bURLEncoded;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadString(strURL);
    argStream.ReadString(strPostData, "");
    argStream.ReadBool(bURLEncoded, true);

    if (!argStream.HasErrors())
    {
        // Are we dealing with a remote website?
        if (strURL.substr(0, 7) == "http://" || strURL.substr(0, 8) == "https://")
        {
            bool isLocalURL = strURL.substr(0, 11) == "http://mta/";
            if (pWebBrowser->IsLocal() != isLocalURL)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }

            lua_pushboolean(luaVM, pWebBrowser->LoadURL(strURL, !isLocalURL, strPostData, bURLEncoded));
            return 1;
        }
        else
        {
            argStream.SetCustomError("Invalid URL scheme provided. Only http:// and https:// is supported.", "Invalid parameter");
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::IsBrowserLoading(lua_State* luaVM)
{
    //  bool isBrowserLoading(browser webBrowser)
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->IsLoading());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseMove(lua_State* luaVM)
{
    //  bool injectBrowserMouseMove(browser webBrowser, int x, int y)
    CClientWebBrowser* pWebBrowser;
    CVector2D          vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadVector2D(vecPosition);

    if (!argStream.HasErrors())
    {
        pWebBrowser->InjectMouseMove((int)vecPosition.fX, (int)vecPosition.fY);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseDown(lua_State* luaVM)
{
    //  bool injectBrowserMouseDown ( browser webBrowser, string mouseButton [ , bool doubleClick = false ] )
    CClientWebBrowser*     pWebBrowser{};
    eWebBrowserMouseButton mouseButton{};
    bool                   doubleClick{};

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadEnumString(mouseButton);
    argStream.ReadBool(doubleClick, false);

    if (!argStream.HasErrors())
    {
        pWebBrowser->InjectMouseDown(mouseButton, doubleClick ? 2 : 1);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseUp(lua_State* luaVM)
{
    //  bool injectBrowserMouseUp ( browser webBrowser, string mouseButton )
    CClientWebBrowser*     pWebBrowser;
    eWebBrowserMouseButton mouseButton;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadEnumString(mouseButton);

    if (!argStream.HasErrors())
    {
        pWebBrowser->InjectMouseUp(mouseButton);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseWheel(lua_State* luaVM)
{
    //  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    CClientWebBrowser* pWebBrowser;
    int                iScrollVert;
    int                iScrollHorz;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadNumber(iScrollVert);
    argStream.ReadNumber(iScrollHorz);

    if (!argStream.HasErrors())
    {
        pWebBrowser->InjectMouseWheel(iScrollVert, iScrollHorz);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::GetBrowserTitle(lua_State* luaVM)
{
    //  string getBrowserPageTitle ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pWebBrowser->GetTitle());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::GetBrowserURL(lua_State* luaVM)
{
    //  string getBrowserURL ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pWebBrowser->GetURL());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::SetBrowserRenderingPaused(lua_State* luaVM)
{
    //  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    CClientWebBrowser* pWebBrowser;
    bool               bPaused;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadBool(bPaused);

    if (!argStream.HasErrors())
    {
        pWebBrowser->SetRenderingPaused(bPaused);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaBrowserDefs::IsBrowserRenderingPaused(CClientWebBrowser* browser)
{
    return browser->GetRenderingPaused();
}

int CLuaBrowserDefs::ExecuteBrowserJavascript(lua_State* luaVM)
{
    //  bool executeBrowserJavascript ( browser webBrowser, string jsCode )
    CClientWebBrowser* pWebBrowser;
    SString            strJavascriptCode;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadString(strJavascriptCode);

    if (!argStream.HasErrors())
    {
        if (strJavascriptCode.empty() || pWebBrowser->ExecuteJavascript(strJavascriptCode))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
            argStream.SetCustomError("This function does not work with remote browsers");
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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

int CLuaBrowserDefs::SetBrowserVolume(lua_State* luaVM)
{
    //  bool setBrowserVolume ( float volume )
    //  bool setBrowserVolume ( browser webBrowser, float volume )
    CClientWebBrowser* pWebBrowser;
    float              fVolume;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsNumber())
    {
        argStream.ReadNumber(fVolume);
        lua_pushboolean(luaVM, g_pCore->GetWebCore()->SetGlobalAudioVolume(fVolume));
        return 1;
    }

    argStream.ReadUserData(pWebBrowser);
    argStream.ReadNumber(fVolume);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->SetAudioVolume(fVolume));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::IsBrowserDomainBlocked(lua_State* luaVM)
{
    //  bool isBrowserDomainBlocked ( string domain, bool isURL )
    SString strURL;
    bool    bIsURL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strURL);
    argStream.ReadBool(bIsURL, false);

    if (!argStream.HasErrors())
    {
        if (bIsURL)
            strURL = g_pCore->GetWebCore()->GetDomainFromURL(strURL);

        if (!strURL.empty())
        {
            lua_pushboolean(luaVM, g_pCore->GetWebCore()->GetDomainState(strURL) != eURLState::WEBPAGE_ALLOWED);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::FocusBrowser(lua_State* luaVM)
{
    //  focusBrowser ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsNil() || argStream.NextIsNone())
    {
        g_pCore->GetWebCore()->SetFocusedWebView(NULL);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        pWebBrowser->Focus();
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::IsBrowserFocused(lua_State* luaVM)
{
    //  browser isBrowserFocused ()
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        CWebViewInterface* pWebView = g_pCore->GetWebCore()->GetFocusedWebView();
        lua_pushboolean(luaVM, pWebBrowser->GetWebView() == pWebView);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::SetBrowserProperty(lua_State* luaVM)
{
    //  bool setBrowserProperty ( browser webBrowser, string key, string value )
    CClientWebBrowser* pWebBrowser;
    SString            strKey;
    SString            strValue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadString(strKey);
    argStream.ReadString(strValue);

    if (!argStream.HasErrors())
    {
        if (pWebBrowser->SetProperty(strKey, strValue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::GetBrowserProperty(lua_State* luaVM)
{
    //  string getBrowserProperty ( browser webBrowser, string key )
    CClientWebBrowser* pWebBrowser;
    SString            strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        SString strValue;
        if (pWebBrowser->GetProperty(strKey, strValue))
        {
            lua_pushstring(luaVM, strValue);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::GetBrowserSettings(lua_State* luaVM)
{
    //  table getBrowserSettings ()
    lua_createtable(luaVM, 0, 3);

    lua_pushstring(luaVM, "RemoteEnabled");
    lua_pushboolean(luaVM, g_pCore->GetWebCore()->GetRemotePagesEnabled());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "RemoteJavascript");
    lua_pushboolean(luaVM, g_pCore->GetWebCore()->GetRemoteJavascriptEnabled());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "PluginsEnabled");
    lua_pushboolean(luaVM, false);
    lua_settable(luaVM, -3);

    return 1;
}

int CLuaBrowserDefs::GetBrowserSource(lua_State* luaVM)
{
    //  bool getBrowserSource ( function callback )
    CClientWebBrowser* pWebBrowser;
    CLuaFunctionRef    callbackFunction;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadFunction(callbackFunction);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain && VERIFY_FUNCTION(callbackFunction))
        {
            pWebBrowser->GetSourceCode([callbackFunction, pLuaMain, pWebBrowser](const std::string& code) {
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

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::ToggleBrowserDevTools(lua_State* luaVM)
{
    //  bool toggleBrowserDevTools ( browser webBrowser, bool visible )
    CClientWebBrowser* pWebBrowser;
    bool               visible;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadBool(visible);

    if (!argStream.HasErrors())
    {
        if (g_pCore->GetWebCore()->IsTestModeEnabled())
        {
            lua_pushboolean(luaVM, pWebBrowser->ToggleDevTools(visible));
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, "toggleBrowserDevtools can only be used in development mode");
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::ResizeBrowser(lua_State* luaVM)
{
    //  bool resizeBrowser(browser webBrowser, float width, float height)
    CClientWebBrowser* pWebBrowser;
    CVector2D          size;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadVector2D(size);

    if (!argStream.HasErrors())
    {
        pWebBrowser->Resize(size);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaBrowserDefs::CanBrowserNavigateBack(lua_State* luaVM)
{
    //  bool canBrowserNavigateBack( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->CanGoBack());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::CanBrowserNavigateForward(lua_State* luaVM)
{
    //  bool canBrowserNavigateForward( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->CanGoForward());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::NavigateBrowserBack(lua_State* luaVM)
{
    //  bool navigateBrowserBack( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->GoBack());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::NavigateBrowserForward(lua_State* luaVM)
{
    //  bool navigateBrowserForward( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pWebBrowser->GoForward());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaBrowserDefs::ReloadBrowserPage(lua_State* luaVM)
{
    //  bool reloadBrowserPage( browser webBrowser )
    CClientWebBrowser* pWebBrowser;
    bool               bIgnoreCache;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadIfNextIsBool(bIgnoreCache, false);

    if (!argStream.HasErrors())
    {
        pWebBrowser->Refresh(bIgnoreCache);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
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
        if (!bIsLocal && !g_pCore->GetWebCore()->GetRemotePagesEnabled())
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

int CLuaBrowserDefs::GUIGetBrowser(lua_State* luaVM)            // Or rather guiGetBrowserBrowser?
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

int CLuaBrowserDefs::SetBrowserAjaxHandler(lua_State* luaVM)
{
    //  bool setBrowserAjaxHandler ( browser browser, string URL[, function callback] )
    CClientWebBrowser* pWebBrowser;
    SString            strURL;
    CLuaFunctionRef    callbackFunction;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWebBrowser);
    argStream.ReadString(strURL);

    if (argStream.NextIsNil() || argStream.NextIsNone())
    {
        if (!argStream.HasErrors())
        {
            lua_pushboolean(luaVM, pWebBrowser->RemoveAjaxHandler(strURL));
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    else
    {
        argStream.ReadFunction(callbackFunction);
        argStream.ReadFunctionComplete();
        if (!argStream.HasErrors())
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain && VERIFY_FUNCTION(callbackFunction))
            {
                CResource*        pResource = pLuaMain->GetResource();
                CResourceManager* pResourceManager = m_pResourceManager;
                auto              netId = pResource->GetNetID();

                bool bResult = pWebBrowser->AddAjaxHandler(strURL, [=](std::vector<SString>& vecGet, std::vector<SString>& vecPost) -> const SString {
                    // Make sure the resource is still running
                    if (!pResourceManager->Exists(pResource) || pResource->GetNetID() != netId)
                    {
                        return "";
                    }

                    // Make sure the function is valid
                    if (VERIFY_FUNCTION(callbackFunction))
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

                        arguments.Call(pLuaMain, callbackFunction, &result);

                        if (result.IsEmpty())
                            return "";

                        CLuaArgument* returnedValue = *result.begin();
                        if (returnedValue->GetType() == LUA_TSTRING)
                            return returnedValue->GetString();
                        else
                            return "";
                    }
                    else
                        return "";
                });

                lua_pushboolean(luaVM, bResult);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaBrowserDefs::IsBrowserGPUEnabled() noexcept
{
    return g_pCore->GetWebCore()->GetGPUEnabled();
}

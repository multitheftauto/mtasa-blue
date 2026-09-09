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

#pragma once
#include "CLuaDefs.h"
#include <core/CWebCoreInterface.h>

class CLuaBrowserDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<CClientWebBrowser*, bool> CreateBrowser(lua_State* luaVM, CVector2D size, bool isLocal, std::optional<bool> transparent);
    static bool                                   RequestBrowserDomains(lua_State* luaVM, std::vector<std::string> pages, std::optional<bool> isURL,
                                                                        std::optional<CLuaFunctionRef> callbackFunction);
    static bool        LoadBrowserURL(CClientWebBrowser* browser, const std::string url, std::optional<std::string> postData, std::optional<bool> urlEncoded);
    static bool        IsBrowserLoading(CClientWebBrowser* browser);
    static bool        InjectBrowserMouseMove(CClientWebBrowser* browser, CVector2D position);
    static bool        InjectBrowserMouseDown(CClientWebBrowser* browser, eWebBrowserMouseButton mouseButton, std::optional<bool> doubleClick);
    static bool        InjectBrowserMouseUp(CClientWebBrowser* browser, eWebBrowserMouseButton mouseButton);
    static bool        InjectBrowserMouseWheel(CClientWebBrowser* browser, int scrollVert, int scrollHorz);
    static std::string GetBrowserTitle(CClientWebBrowser* browser);
    static std::string GetBrowserURL(CClientWebBrowser* browser);
    static bool        SetBrowserRenderingPaused(CClientWebBrowser* browser, bool paused);
    static bool        IsBrowserRenderingPaused(CClientWebBrowser* browser);
    static bool        ExecuteBrowserJavascript(CClientWebBrowser* browser, const std::string javascriptCode);
    LUA_DECLARE(GetBrowserVolume);
    static std::variant<CClientWebBrowser*, float> SetBrowserVolume(std::variant<CClientWebBrowser*, float> webBrowserOrVolume, std::optional<float> volume);
    static std::optional<bool>                     IsBrowserDomainBlocked(const std::string url, std::optional<bool> isURL);
    static bool                                    FocusBrowser(std::optional<CClientWebBrowser*> browser);
    static bool                                    IsBrowserFocused(CClientWebBrowser* browser);
    static bool                                    SetBrowserProperty(CClientWebBrowser* browser, const std::string key, const std::string value);
    static std::optional<std::string>              GetBrowserProperty(CClientWebBrowser* browser, const std::string key);
    static auto                                    GetBrowserSettings();
    static bool                                    GetBrowserSource(lua_State* luaVM, CClientWebBrowser* browser, CLuaFunctionRef callbackFunction);
    static bool SetBrowserAjaxHandler(lua_State* luaVM, CClientWebBrowser* browser, const std::string url, std::optional<CLuaFunctionRef> callbackFunction);
    static bool CanBrowserNavigateBack(CClientWebBrowser* browser);
    static bool CanBrowserNavigateForward(CClientWebBrowser* browser);
    static bool NavigateBrowserBack(CClientWebBrowser* browser);
    static bool NavigateBrowserForward(CClientWebBrowser* browser);
    static bool ReloadBrowserPage(CClientWebBrowser* browser, std::optional<bool> ignoreCache);
    static bool ToggleBrowserDevTools(CClientWebBrowser* browser, bool visible);
    static bool ResizeBrowser(CClientWebBrowser* browser, CVector2D size);
    LUA_DECLARE(GUICreateBrowser);
    LUA_DECLARE(GUIGetBrowser);
    static bool IsBrowserGPUEnabled() noexcept;
};

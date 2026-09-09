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

    static std::variant<CClientWebBrowser*, bool> CreateBrowser(lua_State* luaVM, CVector2D vecSize, bool bIsLocal, std::optional<bool> bTransparent);
    static bool                                   RequestBrowserDomains(lua_State* luaVM, std::vector<std::string> pages, std::optional<bool> bIsURL,
                                                                         std::optional<CLuaFunctionRef> callbackFunction);
    static bool LoadBrowserURL(CClientWebBrowser* pWebBrowser, const std::string strURL, std::optional<std::string> strPostData,
                               std::optional<bool> bURLEncoded);
    static bool IsBrowserLoading(CClientWebBrowser* pWebBrowser);
    static bool InjectBrowserMouseMove(CClientWebBrowser* pWebBrowser, CVector2D vecPosition);
    static bool InjectBrowserMouseDown(CClientWebBrowser* pWebBrowser, eWebBrowserMouseButton mouseButton, std::optional<bool> doubleClick);
    static bool InjectBrowserMouseUp(CClientWebBrowser* pWebBrowser, eWebBrowserMouseButton mouseButton);
    static bool InjectBrowserMouseWheel(CClientWebBrowser* pWebBrowser, int iScrollVert, int iScrollHorz);
    static SString GetBrowserTitle(CClientWebBrowser* pWebBrowser);
    static SString GetBrowserURL(CClientWebBrowser* pWebBrowser);
    static bool    SetBrowserRenderingPaused(CClientWebBrowser* pWebBrowser, bool bPaused);
    static bool    IsBrowserRenderingPaused(CClientWebBrowser* browser);
    static bool    ExecuteBrowserJavascript(CClientWebBrowser* pWebBrowser, const std::string strJavascriptCode);
    LUA_DECLARE(GetBrowserVolume);
    static bool    SetBrowserVolume(std::variant<CClientWebBrowser*, float> webBrowserOrVolume, std::optional<float> fVolume);
    static std::optional<bool> IsBrowserDomainBlocked(const std::string strURL, std::optional<bool> bIsURL);
    static bool                FocusBrowser(std::optional<CClientWebBrowser*> pWebBrowser);
    static bool                IsBrowserFocused(CClientWebBrowser* pWebBrowser);
    static bool                SetBrowserProperty(CClientWebBrowser* pWebBrowser, const std::string strKey, const std::string strValue);
    static std::optional<SString> GetBrowserProperty(CClientWebBrowser* pWebBrowser, const std::string strKey);
    static std::unordered_map<std::string, bool> GetBrowserSettings();
    static bool GetBrowserSource(lua_State* luaVM, CClientWebBrowser* pWebBrowser, CLuaFunctionRef callbackFunction);
    static bool SetBrowserAjaxHandler(lua_State* luaVM, CClientWebBrowser* pWebBrowser, const std::string strURL,
                                      std::optional<CLuaFunctionRef> callbackFunction);
    static bool CanBrowserNavigateBack(CClientWebBrowser* pWebBrowser);
    static bool CanBrowserNavigateForward(CClientWebBrowser* pWebBrowser);
    static bool NavigateBrowserBack(CClientWebBrowser* pWebBrowser);
    static bool NavigateBrowserForward(CClientWebBrowser* pWebBrowser);
    static bool ReloadBrowserPage(CClientWebBrowser* pWebBrowser, std::optional<bool> bIgnoreCache);
    static bool ToggleBrowserDevTools(CClientWebBrowser* pWebBrowser, bool visible);
    static bool ResizeBrowser(CClientWebBrowser* pWebBrowser, CVector2D size);
    LUA_DECLARE(GUICreateBrowser);
    LUA_DECLARE(GUIGetBrowser);
    static bool IsBrowserGPUEnabled() noexcept;
};

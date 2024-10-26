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

#pragma once
#include "CLuaDefs.h"

class CLuaBrowserDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateBrowser);
    LUA_DECLARE(IsBrowserSupported);
    LUA_DECLARE(RequestBrowserDomains);
    LUA_DECLARE(LoadBrowserURL);
    LUA_DECLARE(IsBrowserLoading);
    LUA_DECLARE(InjectBrowserMouseMove);
    LUA_DECLARE(InjectBrowserMouseDown);
    LUA_DECLARE(InjectBrowserMouseUp);
    LUA_DECLARE(InjectBrowserMouseWheel);
    LUA_DECLARE(GetBrowserTitle);
    LUA_DECLARE(GetBrowserURL);
    LUA_DECLARE(SetBrowserRenderingPaused);
    static bool IsBrowserRenderingPaused(CClientWebBrowser* browser);
    LUA_DECLARE(ExecuteBrowserJavascript);
    LUA_DECLARE(GetBrowserVolume);
    LUA_DECLARE(SetBrowserVolume);
    LUA_DECLARE(IsBrowserDomainBlocked);
    LUA_DECLARE(FocusBrowser);
    LUA_DECLARE(IsBrowserFocused);
    LUA_DECLARE(SetBrowserProperty);
    LUA_DECLARE(GetBrowserProperty);
    LUA_DECLARE(GetBrowserSettings);
    LUA_DECLARE(GetBrowserSource);
    LUA_DECLARE(SetBrowserAjaxHandler);
    LUA_DECLARE(CanBrowserNavigateBack);
    LUA_DECLARE(CanBrowserNavigateForward);
    LUA_DECLARE(NavigateBrowserForward);
    LUA_DECLARE(NavigateBrowserBack);
    LUA_DECLARE(ReloadBrowserPage);
    LUA_DECLARE(ToggleBrowserDevTools);
    LUA_DECLARE(ResizeBrowser);
    LUA_DECLARE(GUICreateBrowser);
    LUA_DECLARE(GUIGetBrowser);
    static bool IsBrowserGPUEnabled() noexcept;
};

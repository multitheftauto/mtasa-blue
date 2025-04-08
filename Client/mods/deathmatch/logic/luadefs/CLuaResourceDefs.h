/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaResourceDefs.cpp
 *  PURPOSE:     Lua resource definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaResourceDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(Call);
    LUA_DECLARE(GetThisResource);
    LUA_DECLARE(GetResourceConfig);
    LUA_DECLARE(GetResourceFromName);
    LUA_DECLARE(GetResourceRootElement);
    LUA_DECLARE(GetResourceGUIElement);
    LUA_DECLARE(GetResourceDynamicElementRoot);
    LUA_DECLARE(GetResourceExportedFunctions);
    LUA_DECLARE(GetResourceState);
    LUA_DECLARE(LoadString);
    LUA_DECLARE(Load);

    static std::string GetResourceName(lua_State* luaVM, std::optional<CResource*> resourceElement);
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPointLightDefs.cpp
 *  PURPOSE:     Lua browser definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPointLightDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<bool, CClientPointLights*> CreateLight(lua_State* luaVM, int mode, CVector pos, std::optional<float> radius,
                                                               std::optional<SColor> color, std::optional<CVector> direction) noexcept;
    LUA_DECLARE(GetLightType);
    LUA_DECLARE(GetLightRadius);
    LUA_DECLARE(GetLightColor);
    LUA_DECLARE(GetLightDirection);
    LUA_DECLARE(SetLightRadius);
    LUA_DECLARE(SetLightColor);
    LUA_DECLARE(SetLightDirection);
};

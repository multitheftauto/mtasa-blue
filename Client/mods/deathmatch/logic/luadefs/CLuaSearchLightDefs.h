/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaSearchLightDefs.h
 *  PURPOSE:     Lua searchlight class functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaSearchLightDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<bool, CClientSearchLight*> CreateSearchLight(lua_State* luaVM, CVector start, CVector end, float startRadius, float endRadius,
                                                                     std::optional<bool> renderSpot) noexcept;

    LUA_DECLARE_OOP(GetSearchLightStartPosition);
    LUA_DECLARE_OOP(GetSearchLightEndPosition);
    LUA_DECLARE(GetSearchLightStartRadius);
    LUA_DECLARE(GetSearchLightEndRadius);

    LUA_DECLARE(SetSearchLightStartPosition);
    LUA_DECLARE(SetSearchLightEndPosition);
    LUA_DECLARE(SetSearchLightStartRadius);
    LUA_DECLARE(SetSearchLightEndRadius);
};

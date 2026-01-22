/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaMarkerDefs.h
 *  PURPOSE:     Lua marker definitions class header
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include "lua/CLuaMultiReturn.h"

class CLuaMarkerDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateMarker);

    LUA_DECLARE(GetMarkerCount);
    LUA_DECLARE(GetMarkerType);
    LUA_DECLARE(GetMarkerSize);
    LUA_DECLARE(GetMarkerColor);
    LUA_DECLARE_OOP(GetMarkerTarget);
    LUA_DECLARE(GetMarkerIcon);

    LUA_DECLARE(SetMarkerType);
    LUA_DECLARE(SetMarkerSize);
    LUA_DECLARE(SetMarkerColor);
    LUA_DECLARE(SetMarkerTarget);
    LUA_DECLARE(SetMarkerIcon);

    static bool SetCoronaReflectionEnabled(CClientMarker* pMarker, bool bEnabled);
    static bool IsCoronaReflectionEnabled(CClientMarker* pMarker);

    static bool SetMarkerTargetArrowProperties(CClientMarker* marker, std::optional<std::uint8_t> r, std::optional<std::uint8_t> g, std::optional<std::uint8_t> b, std::optional<std::uint8_t> a, std::optional<float> size);
    static std::variant<CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t, float>, bool> GetMarkerTargetArrowProperties(CClientMarker* marker) noexcept;
};

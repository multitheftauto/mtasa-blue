/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaRadarAreaDefs.h
 *  PURPOSE:     Lua radar area definitions class header
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaRadarAreaDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<bool, CClientRadarArea*> CreateRadarArea(lua_State* luaVM, CVector2D pos, CVector2D size, std::optional<SColor> color) noexcept;
    LUA_DECLARE(GetRadarAreaColor);
    LUA_DECLARE_OOP(GetRadarAreaSize);
    LUA_DECLARE(IsRadarAreaFlashing);
    LUA_DECLARE(SetRadarAreaColor);
    LUA_DECLARE(SetRadarAreaFlashing);
    LUA_DECLARE(SetRadarAreaSize);
    LUA_DECLARE(IsInsideRadarArea);
};

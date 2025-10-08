/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWaterDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaWaterDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateWater);
    LUA_DECLARE(ResetWaterLevel);
    LUA_DECLARE(ResetWaterColor);
    LUA_DECLARE(TestLineAgainstWater);

    LUA_DECLARE(SetWaterLevel);
    LUA_DECLARE(SetWaterDrawnLast);
    LUA_DECLARE(SetWaterVertexPosition);
    LUA_DECLARE(SetWaterColor);

    LUA_DECLARE(GetWaterColor);
    LUA_DECLARE(GetWaterLevel);
    LUA_DECLARE(IsWaterDrawnLast);
    LUA_DECLARE(GetWaterVertexPosition);
};

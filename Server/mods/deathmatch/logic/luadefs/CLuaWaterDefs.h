/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaWaterDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CWater;

class CLuaWaterDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateWater);
    LUA_DECLARE(SetWaterLevel);
    LUA_DECLARE(ResetWaterLevel);
    LUA_DECLARE(GetWaterVertexPosition);
    static std::variant<CLuaMultiReturn<float, float, float>, CVector, bool> OOP_GetWaterVertexPosition(lua_State* luaVM, CWater* water, int vertexIndex);
    LUA_DECLARE(SetWaterVertexPosition);
    LUA_DECLARE(GetWaterColor);
    LUA_DECLARE(SetWaterColor);
    LUA_DECLARE(ResetWaterColor);
};

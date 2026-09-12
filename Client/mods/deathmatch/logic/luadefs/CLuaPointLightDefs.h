/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPointLightDefs.cpp
 *  PURPOSE:     Lua browser definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CClientPointLights;

class CLuaPointLightDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateLight);
    LUA_DECLARE(GetLightType);
    LUA_DECLARE(GetLightRadius);
    LUA_DECLARE(GetLightColor);
    LUA_DECLARE(GetLightDirection);
    static std::variant<CLuaMultiReturn<float, float, float>, CVector, bool> OOP_GetLightDirection(lua_State* luaVM, CClientPointLights* light);
    LUA_DECLARE(SetLightRadius);
    LUA_DECLARE(SetLightColor);
    LUA_DECLARE(SetLightDirection);
};

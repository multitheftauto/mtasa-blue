/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPrimitiveBufferDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPrimitiveBufferDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(PrimitiveBufferCreate);
    LUA_DECLARE(PrimitiveBufferDraw);
    LUA_DECLARE(PrimitiveBufferDraw3D);

private:
    //static void AddPrimitiveBufferClass(lua_State* luaVM);
};

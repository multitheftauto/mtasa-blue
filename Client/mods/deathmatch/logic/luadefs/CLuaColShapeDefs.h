/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaColShapeDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateColCircle);
    LUA_DECLARE(CreateColCuboid);
    LUA_DECLARE(CreateColSphere);
    LUA_DECLARE(CreateColRectangle);
    LUA_DECLARE(CreateColPolygon);
    LUA_DECLARE(CreateColTube);

    LUA_DECLARE(GetColShapeRadius);
    LUA_DECLARE(SetColShapeRadius);
    LUA_DECLARE(GetColShapeSize);
    LUA_DECLARE(SetColShapeSize);
    LUA_DECLARE(GetColPolygonPoints);
    LUA_DECLARE(GetColPolygonPointPosition);
    LUA_DECLARE(SetColPolygonPointPosition);
    LUA_DECLARE(AddColPolygonPoint);
    LUA_DECLARE(RemoveColPolygonPoint);

    LUA_DECLARE(IsInsideColShape);
    LUA_DECLARE(GetColShapeType);
};

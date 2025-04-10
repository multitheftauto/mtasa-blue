/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>
#include <variant>

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
    LUA_DECLARE_OOP(GetColShapeSize);
    LUA_DECLARE(SetColShapeSize);
    LUA_DECLARE_OOP(GetColPolygonPoints);
    LUA_DECLARE_OOP(GetColPolygonPointPosition);
    LUA_DECLARE(SetColPolygonPointPosition);
    LUA_DECLARE(AddColPolygonPoint);
    LUA_DECLARE(RemoveColPolygonPoint);

    LUA_DECLARE(IsInsideColShape);
    LUA_DECLARE(GetColShapeType);

    static bool SetShowCollision(bool state);
    static bool IsShowCollisionsEnabled();

    static CLuaMultiReturn<float, float> GetColPolygonHeight(CClientColPolygon* pColPolygon);
    static bool                          SetColPolygonHeight(CClientColPolygon* pColPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil);
};

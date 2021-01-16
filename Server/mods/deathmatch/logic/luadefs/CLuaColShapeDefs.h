/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CColPolygon;

class CLuaColShapeDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Shape create funcs
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

    static std::tuple<float, float> GetColPolygonHeight(CColPolygon* pColPolygon);
    static bool                     SetColPolygonHeight(CColPolygon* pColPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil);
};

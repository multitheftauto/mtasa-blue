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
    static std::variant<bool, CColCircle*>    CreateColCircle(lua_State* luaVM, CVector2D pos, float radius) noexcept;
    static std::variant<bool, CColCuboid*>    CreateColCuboid(lua_State* luaVM, CVector pos, CVector size) noexcept;
    static std::variant<bool, CColShape*>     CreateColSphere(lua_State* luaVM, CVector pos, float radius) noexcept;
    static std::variant<bool, CColRectangle*> CreateColRectangle(lua_State* luaVM, CVector2D pos, CVector2D size) noexcept;
    //static std::variant<bool, CColPolygon*>   CreateColPolygon(lua_State* luaVM, CVector2D pos1, CVector2D pos2, CVector2D pos3,
    //                                                           std::optional<CLuaArguments> dims) noexcept;
    static int CreateColPolygon(lua_State* luaVM);
    static std::variant<bool, CColTube*>      CreateColTube(lua_State* luaVM, CVector pos, float height, float radius) noexcept;

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

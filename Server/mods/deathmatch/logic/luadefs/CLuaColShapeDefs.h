/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>
#include <variant>

class CColCircle;
class CColCuboid;
class CColPolygon;
class CColRectangle;
class CColShape;
class CColSphere;
class CColTube;

class CResource;

class CLuaColShapeDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<CColCircle*, bool>    CreateColCircle(lua_State* luaVM, CVector2D position, float radius);
    static std::variant<CColCuboid*, bool>    CreateColCuboid(lua_State* luaVM, CVector position, CVector size);
    static std::variant<CColSphere*, bool>    CreateColSphere(lua_State* luaVM, CVector position, float radius);
    static std::variant<CColRectangle*, bool> CreateColRectangle(lua_State* luaVM, CVector2D position, CVector2D size);
    static std::variant<CColPolygon*, bool>   CreateColPolygon(lua_State* luaVM, CVector2D center, CVector2D pointA, CVector2D pointB, CVector2D pointC,
                                                               LuaVarArgs extraPoints);
    static std::variant<CColTube*, bool>      CreateColTube(lua_State* luaVM, CVector position, float radius, float height);

    static int                                                                                      GetColShapeType(CColShape* shape) noexcept;
    static float                                                                                    GetColShapeRadius(CColShape* shape);
    static bool                                                                                     SetColShapeRadius(CColShape* shape, float radius);
    static std::variant<CLuaMultiReturn<float, float>, CLuaMultiReturn<float, float, float>, float> GetColShapeSize(CColShape* shape);
    static std::variant<CVector2D, CVector, float>                                                  OOP_GetColShapeSize(CColShape* shape);
    static bool                                              SetColShapeSize(CColShape* shape, std::variant<CVector, CVector2D, float> size);
    static std::vector<std::tuple<float, float>>             GetColPolygonPoints(CColShape* shape);
    static std::vector<CVector2D>                            OOP_GetColPolygonPoints(CColShape* shape);
    static std::variant<CLuaMultiReturn<float, float>, bool> GetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex);
    static std::variant<CVector2D, bool>                     OOP_GetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex);
    static bool                                              SetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex, CVector2D point);
    static bool AddColPolygonPoint(lua_State* luaVM, CColShape* shape, CVector2D point, std::optional<int> pointIndex);
    static bool RemoveColPolygonPoint(lua_State* luaVM, CColShape* shape, int pointIndex);
    static bool SetColPolygonHeight(CColShape* shape, std::variant<bool, float> floor, std::variant<bool, float> ceil);

    static bool IsInsideColShape(CColShape* shape, CVector position);

    static CLuaMultiReturn<float, float> GetColPolygonHeight(CColShape* shape);

private:
    template <typename T, typename... Args>
    static T* CreateColShape(CResource& resource, Args&&... args);

    static void RefreshColliders(CColShape* shape);
};

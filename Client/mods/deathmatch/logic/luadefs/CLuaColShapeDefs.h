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

class CResource;

class CLuaColShapeDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<CClientColCircle*, bool>    CreateColCircle(lua_State* luaVM, CVector2D position, std::optional<float> radius);
    static std::variant<CClientColCuboid*, bool>    CreateColCuboid(lua_State* luaVM, CVector position, CVector size);
    static std::variant<CClientColSphere*, bool>    CreateColSphere(lua_State* luaVM, CVector position, std::optional<float> radius);
    static std::variant<CClientColRectangle*, bool> CreateColRectangle(lua_State* luaVM, CVector2D position, CVector2D size);
    static std::variant<CClientColPolygon*, bool>   CreateColPolygon(lua_State* luaVM, CVector2D position, CVector2D pointA, CVector2D pointB, CVector2D pointC,
                                                                     LuaVarArgs extraPoints);
    static std::variant<CClientColTube*, bool>      CreateColTube(lua_State* luaVM, CVector position, std::optional<float> radius, std::optional<float> height);

    static int                                                                                      GetColShapeType(CClientColShape* shape) noexcept;
    static float                                                                                    GetColShapeRadius(CClientColShape* shape);
    static bool                                                                                     SetColShapeRadius(CClientColShape* shape, float radius);
    static std::variant<CLuaMultiReturn<float, float>, CLuaMultiReturn<float, float, float>, float> GetColShapeSize(CClientColShape* shape);
    static std::variant<CVector2D, CVector, float>                                                  OOP_GetColShapeSize(CClientColShape* shape);
    static bool                                              SetColShapeSize(CClientColShape* shape, std::variant<CVector, CVector2D, float> size);
    static std::vector<std::tuple<float, float>>             GetColPolygonPoints(CClientColPolygon* polygon);
    static std::vector<CVector2D>                            OOP_GetColPolygonPoints(CClientColPolygon* polygon);
    static std::variant<CLuaMultiReturn<float, float>, bool> GetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex);
    static std::variant<CVector2D, bool>                     OOP_GetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex);
    static bool SetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex, CVector2D point);
    static bool AddColPolygonPoint(lua_State* luaVM, CClientColPolygon* polygon, CVector2D point, std::optional<int> pointIndex);
    static bool RemoveColPolygonPoint(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex);

    static bool IsInsideColShape(CClientColShape* shape, CVector position);

    static bool SetShowCollision(bool state);
    static bool IsShowCollisionsEnabled();

    static CLuaMultiReturn<float, float> GetColPolygonHeight(CClientColPolygon* colPolygon);
    static bool                          SetColPolygonHeight(CClientColPolygon* colPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil);

private:
    template <typename T, typename... Args>
    static T* CreateColShape(CResource& resource, Args&&... args);

    static void RefreshColliders(CClientColShape* shape);
};

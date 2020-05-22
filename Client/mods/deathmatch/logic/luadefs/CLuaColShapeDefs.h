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
#include <optional>
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

    LUA_DECLARE(GetColShapeType);
    LUA_DECLARE(IsInsideColShape);

    static float GetColShapeRadius(CClientColShape* const colShape);
    static bool  SetColShapeRadius(CClientColShape* const colShape, const float radius);

    static std::variant<CVector, CVector2D, float> GetColShapeSize_OOP(CClientColShape* const colShape);
    static std::variant<std::tuple<float>, std::tuple<float, float>, std::tuple<float, float, float>> GetColShapeSize(CClientColShape* const colShape);

    LUA_DECLARE(SetColShapeSize);
    LUA_DECLARE(GetColPolygonPoints)

    static const CVector2D& GetColPolygonPointPosition_OOP(CClientColShape* const colShape, unsigned int pointIndex);
    static inline std::tuple<float, float> GetColPolygonPointPosition(CClientColShape* const colShape, unsigned int pointIndex)
    {
        const auto& vector = GetColPolygonPointPosition_OOP(colShape, pointIndex);
        return { vector.fX, vector.fY };
    }

    static bool SetColPolygonPointPosition(CClientColShape* const colShape, unsigned int pointIndex, const CVector2D point);
    static bool AddColPolygonPoint(CClientColShape* const colShape, const CVector2D point, const std::optional<unsigned int> optPointIndex);
    static bool RemoveColPolygonPoint(CClientColShape* const colShape, unsigned int pointIndex);
};

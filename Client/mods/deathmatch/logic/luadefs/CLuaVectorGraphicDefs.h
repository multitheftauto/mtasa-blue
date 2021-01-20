/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaVectorGraphicDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include "CClientVectorGraphic.h"

class CLuaVectorGraphicDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static CClientVectorGraphic* SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData);

    static std::variant<bool, int> SVGAddRect(CClientVectorGraphic* pVectorGraphic, std::variant<float, std::string> x, std::variant<float, std::string> y, std::variant<float, std::string> width, std::variant<float, std::string> height, std::variant<float, std::string> rx, std::variant<float, std::string> ry, float pathLength, std::string fill);

    static std::variant<bool, int> SVGAddCircle(CClientVectorGraphic* pVectorGraphic, std::variant<float, std::string> cx, std::variant<float, std::string> cy, float radius, float pathLength, std::string fill);
};

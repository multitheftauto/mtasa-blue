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

    static CClientVectorGraphic* SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData, std::optional<CLuaFunctionRef> funcRef);

    static CXMLNode* SVGGetDocumentXML(CClientVectorGraphic* pVectorGraphic);
    static bool      SVGSetDocumentXML(CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode, std::optional<CLuaFunctionRef> luaFunctionRef);

    static CLuaMultiReturn<int, int> SVGGetSize(CClientVectorGraphic* pVectorGraphic);
    static bool                      SVGSetSize(CClientVectorGraphic* pVectorGraphic, int width, int height, std::optional<CLuaFunctionRef> luaFunctionRef);
};

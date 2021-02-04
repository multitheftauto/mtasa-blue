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
    static CXMLNode*             SVGGetDocumentXML(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic);
    static bool                  SVGSetDocumentXML(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode);
};

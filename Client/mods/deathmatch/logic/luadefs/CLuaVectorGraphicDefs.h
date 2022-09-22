/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    static CClientVectorGraphic* SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData,
                                           std::optional<CLuaFunctionRef> luaFunctionRef);

    static CXMLNode* SVGGetDocumentXML(CClientVectorGraphic* vectorGraphic);
    static bool      SVGSetDocumentXML(CClientVectorGraphic* vectorGraphic, CXMLNode* xmlNode, std::optional<CLuaFunctionRef> luaFunctionRef);

    static CLuaMultiReturn<int, int> SVGGetSize(CClientVectorGraphic* vectorGraphic);
    static bool                      SVGSetSize(CClientVectorGraphic* vectorGraphic, CVector2D size, std::optional<CLuaFunctionRef> luaFunctionRef);

private:
    static bool LoadFromData(lua_State* luaVM, CClientVectorGraphic* vectorGraphic, std::string rawData);
    static bool LoadFromFile(lua_State* luaVM, CClientVectorGraphic* vectorGraphic, CScriptFile* file, std::string path, CResource* parentResource);

    static bool SetDocument(CClientVectorGraphic* vectorGraphic, CXMLNode* xmlNode);
    static bool SetSize(CClientVectorGraphic* vectorGraphic, CVector2D size);

    static bool                                SVGSetUpdateCallback(CClientVectorGraphic* vectorGraphic, std::variant<CLuaFunctionRef, bool> luaFunctionRef);
    static std::variant<CLuaFunctionRef, bool> SVGGetUpdateCallback(CClientVectorGraphic* vectorGraphic);
};

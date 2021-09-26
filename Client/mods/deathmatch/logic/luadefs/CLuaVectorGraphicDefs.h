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

    static CXMLNode* SVGGetDocumentXML(CClientVectorGraphic* pVectorGraphic);
    static bool      SVGSetDocumentXML(CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode, std::optional<CLuaFunctionRef> luaFunctionRef);

    static CLuaMultiReturn<int, int> SVGGetSize(CClientVectorGraphic* pVectorGraphic);
    static bool                      SVGSetSize(CClientVectorGraphic* pVectorGraphic, CVector2D size, std::optional<CLuaFunctionRef> luaFunctionRef);

private:
    static bool LoadFromData(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic, std::string strRawData);
    static bool LoadFromFile(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic, CScriptFile* pFile, std::string strPath, CResource* pParentResource);

    static bool SetDocument(CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode);
    static bool SetSize(CClientVectorGraphic* pVectorGraphic, CVector2D size);
};

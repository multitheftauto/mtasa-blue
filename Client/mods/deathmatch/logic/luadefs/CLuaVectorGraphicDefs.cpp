/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include "CLuaVectorGraphicDefs.h"

void CLuaVectorGraphicDefs::LoadFunctions()
{
   constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"svgCreate", ArgumentParser<SVGCreate>},
        {"svgGetDocumentXML", ArgumentParser<SVGGetDocumentXML>},
        {"svgSetDocumentXML", ArgumentParser<SVGSetDocumentXML>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVectorGraphicDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "svgCreate");
    lua_classfunction(luaVM, "getDocumentXML", "svgGetDocumentXML");
    lua_classfunction(luaVM, "SetDocumentXML", "svgSetDocumentXML");

    lua_registerclass(luaVM, "SVG");
}

CClientVectorGraphic* CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData)
{
    if (size.fX <= 0 || size.fY <= 0)
    {
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");
    }

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* pParentResource = pLuaMain->GetResource();

    CClientVectorGraphic* pVectorGraphic = g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic((int)size.fX, (int)size.fY);

    if (pVectorGraphic)
    {
        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
        pVectorGraphic->SetParent(pParentResource->GetResourceDynamicEntity());

        // Set our owner resource
        pVectorGraphic->SetResource(pParentResource);

        if (pathOrRawData.has_value())
        {
            // Try and load from the filepath or svg data (if provided)
            std::string strPath;
            if (CResourceManager::ParseResourcePathInput(pathOrRawData.value(), pParentResource, &strPath) && FileExists(strPath))
            {
                if (!pVectorGraphic->LoadFromFile(strPath))
                {
                    pVectorGraphic->Destroy();
                    throw std::invalid_argument(SString("Unable to load SVG file [%s]", pathOrRawData.value().c_str()).c_str());
                }
            }
            else
            {
                if (!pVectorGraphic->LoadFromData(pathOrRawData.value()))
                {
                    pVectorGraphic->Destroy();
                    throw std::invalid_argument("Unable to load SVG data");
                }
            }
        }

        return pVectorGraphic;
    }

    throw std::invalid_argument("Error occurred creating SVG element");
}

CXMLNode* CLuaVectorGraphicDefs::SVGGetDocumentXML(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    if (pLuaMain)
    {
        std::string strXmlContent = pVectorGraphic->GetSVGDocumentXML();

        CXMLNode* rootNode = pLuaMain->ParseString(strXmlContent.c_str());

        if (rootNode && rootNode->IsValid())
        {
            return rootNode;
        }
        else
            throw std::invalid_argument("Unable to get SVG XML document");
    }

    throw std::invalid_argument("Unable to get SVG XML document");
}

bool CLuaVectorGraphicDefs::SVGSetDocumentXML(lua_State* luaVM, CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode)
{
    std::string strXML = pXMLNode->ToString();

    if (pVectorGraphic->LoadFromData(strXML.c_str()))
        return true;

    throw std::invalid_argument("Unable to set SVG XML document");
}

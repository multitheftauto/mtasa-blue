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
    lua_classfunction(luaVM, "setDocumentXML", "svgSetDocumentXML");

    lua_registerclass(luaVM, "SVG");
}

CClientVectorGraphic* CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData)
{
    if (size.fX <= 0 || size.fY <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    CLuaMain&  pLuaMain = lua_getownercluamain(luaVM);
    CResource* pParentResource = pLuaMain.GetResource();

    CClientVectorGraphic* pVectorGraphic = g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic(static_cast<int>(size.fX), static_cast<int>(size.fY));

    if (!pVectorGraphic)
        return false;

    if (pathOrRawData.has_value())
    {
        std::string& strRawData = pathOrRawData.value();
        std::size_t szCharPos = strRawData.find_first_not_of(" \t\n\r\f\v", 0);

        char charAuto = 0;

        if (szCharPos != std::string::npos)
            charAuto = strRawData.at(szCharPos);

        if (charAuto && charAuto == '<')
        {
            if (!pVectorGraphic->LoadFromString(pathOrRawData.value()))
            {
                delete pVectorGraphic;

                m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG data (check for XML syntax errors)");
                return false;
            }
        }
        else
        {
            std::string strPath;
            std::string strMetaPath;

            if (CResourceManager::ParseResourcePathInput(pathOrRawData.value(), pParentResource, &strPath, &strMetaPath))
            {
                eAccessType  accessType = strPath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* pFile = new CScriptFile(pParentResource->GetScriptID(), strMetaPath.c_str(), 52428800, accessType);

                if (!pFile->Load(pParentResource, CScriptFile::MODE_READ))
                {
                    delete pVectorGraphic;
                    delete pFile;

                    m_pScriptDebugging->LogCustom(luaVM, SString("Unable to load SVG (file doesn't exist) [%s]", pathOrRawData.value().c_str()));
                    return false;
                }
                else
                {
                    SString strXmlData;
                    pFile->Read(pFile->GetSize(), strXmlData);

                    // We don't need the file handler anymore
                    delete pFile;

                    if (strXmlData.empty() || !pVectorGraphic->LoadFromString(strXmlData))
                    {
                        delete pVectorGraphic;

                        m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG data (check for XML syntax errors)");
                        return false;
                    }
                }
            }
        }
    }   
  
    // Make our element a child of the resource's file root
    pVectorGraphic->SetParent(pParentResource->GetResourceDynamicEntity());

    // Set our owner resource
    pVectorGraphic->SetResource(pParentResource);

    return pVectorGraphic;
}

CXMLNode* CLuaVectorGraphicDefs::SVGGetDocumentXML(CClientVectorGraphic* pVectorGraphic)
{
    return pVectorGraphic->GetXMLDocument();
}

bool CLuaVectorGraphicDefs::SVGSetDocumentXML(CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode)
{
    return pVectorGraphic->SetDocument(pXMLNode);
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
        {"svgGetSize", ArgumentParser<SVGGetSize>},
        {"svgSetSize", ArgumentParser<SVGSetSize>},
        {"svgSetUpdateCallback", ArgumentParser<SVGSetUpdateCallback>},

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
    lua_classfunction(luaVM, "getSize", "svgGetSize");
    lua_classfunction(luaVM, "setSize", "svgSetSize");

    lua_registerclass(luaVM, "SVG");
}

bool CLuaVectorGraphicDefs::LoadFromData(lua_State* luaVM, CClientVectorGraphic* vectorGraphic, std::string rawData)
{
    if (!luaVM || !vectorGraphic)
        return false;

    if (!vectorGraphic->LoadFromString(rawData))
    {
        m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG data (check for XML syntax errors)");
        return false;
    }

    return true;
}

bool CLuaVectorGraphicDefs::SetDocument(CClientVectorGraphic* vectorGraphic, CXMLNode* xmlNode)
{
    if (!vectorGraphic || !xmlNode || !xmlNode->IsValid())
        return false;

    return vectorGraphic->SetDocument(xmlNode);
}

bool CLuaVectorGraphicDefs::LoadFromFile(lua_State* luaVM, CClientVectorGraphic* vectorGraphic, CScriptFile* file, std::string path, CResource* parentResource)
{
    if (!luaVM || !vectorGraphic || !file || !parentResource)
        return false;

    if (!file->Load(parentResource, CScriptFile::MODE_READ))
    {
        m_pScriptDebugging->LogCustom(luaVM, SString("Unable to load SVG (file doesn't exist) [%s]", path.c_str()));
        return false;
    }
    else
    {
        SString strXmlData;
        file->Read(file->GetSize(), strXmlData);

        if (strXmlData.empty() || !vectorGraphic->LoadFromString(strXmlData))
        {
            delete vectorGraphic;

            m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG file (check for XML syntax errors)");
            return false;
        }
    }

    return true;
}

bool CLuaVectorGraphicDefs::SetSize(CClientVectorGraphic* vectorGraphic, CVector2D size)
{
    if (!vectorGraphic)
        return false;

    CVectorGraphicItem* vectorGraphicItem = vectorGraphic->GetRenderItem();

    if (!vectorGraphicItem)
        return false;

    if (size.fX <= 0 || size.fY <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    if (size.fX > 4096 || size.fY > 4096)
        throw std::invalid_argument("A vector graphic cannot exceed 4096x4096 in size.");

    int intSizeX = static_cast<int>(size.fX);
    int intSizeY = static_cast<int>(size.fY);

    vectorGraphicItem->Resize(CVector2D(intSizeX, intSizeY));

    vectorGraphic->GetDisplay()->Update();
    return true;
}

CClientVectorGraphic* CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData,
                                                       std::optional<CLuaFunctionRef> luaFunctionRef)
{
    if (size.fX <= 0 || size.fY <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    if (size.fX > 4096 || size.fY > 4096)
        throw std::invalid_argument("A vector graphic cannot exceed 4096x4096 in size.");

    CLuaMain&  luaMain = lua_getownercluamain(luaVM);
    CResource* parentResource = luaMain.GetResource();

    CClientVectorGraphic* vectorGraphic =
        g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic(static_cast<int>(size.fX), static_cast<int>(size.fY));

    if (!vectorGraphic)
        return nullptr;

    if (pathOrRawData.has_value())
    {
        std::string& rawData = pathOrRawData.value();
        std::size_t  charPos = rawData.find_first_not_of(" \t\n\r\f\v", 0);

        char charAuto = 0;

        if (charPos != std::string::npos)
            charAuto = rawData.at(charPos);

        if (charAuto && charAuto == '<')
        {
            bool didLoad = LoadFromData(luaVM, vectorGraphic, pathOrRawData.value());

            // Retain support for callback functions (#2589)
            if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
            {
                vectorGraphic->SetUpdateCallback(luaFunctionRef.value());
            }

            if (!didLoad)
            {
                delete vectorGraphic;
                return nullptr;
            }
        }
        else
        {
            std::string path;
            std::string metaPath;

            if (CResourceManager::ParseResourcePathInput(pathOrRawData.value(), parentResource, &path, &metaPath))
            {
                eAccessType  accessType = path[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* file = new CScriptFile(parentResource->GetScriptID(), metaPath.c_str(), 52428800, accessType);

                bool didLoad = LoadFromFile(luaVM, vectorGraphic, file, pathOrRawData.value(), parentResource);

                delete file;

                // Retain support for callback functions (#2589)
                if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
                {
                    vectorGraphic->SetUpdateCallback(luaFunctionRef.value());
                }

                if (!didLoad)
                {
                    delete vectorGraphic;
                    return nullptr;
                }
            }
            else
            {
                delete vectorGraphic;

                m_pScriptDebugging->LogCustom(luaVM, SString("Unable to load SVG (invalid file path) [%s]", pathOrRawData.value().c_str()));
                return nullptr;
            }
        }
    }

    // Make our element a child of the resource's file root
    vectorGraphic->SetParent(parentResource->GetResourceDynamicEntity());

    // Set our owner resource
    vectorGraphic->SetResource(parentResource);

    return vectorGraphic;
}

CXMLNode* CLuaVectorGraphicDefs::SVGGetDocumentXML(CClientVectorGraphic* vectorGraphic)
{
    return vectorGraphic->GetXMLDocument();
}

bool CLuaVectorGraphicDefs::SVGSetDocumentXML(CClientVectorGraphic* vectorGraphic, CXMLNode* xmlNode, std::optional<CLuaFunctionRef> luaFunctionRef)
{
    bool didLoad = SetDocument(vectorGraphic, xmlNode);

    // Retain support for callback functions (#2589)
    if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
    {
        vectorGraphic->SetUpdateCallback(luaFunctionRef.value());
    }

    return didLoad;
}

CLuaMultiReturn<int, int> CLuaVectorGraphicDefs::SVGGetSize(CClientVectorGraphic* vectorGraphic)
{
    return {(int)vectorGraphic->GetRenderItem()->m_uiSizeX, (int)vectorGraphic->GetRenderItem()->m_uiSizeY};
}

bool CLuaVectorGraphicDefs::SVGSetSize(CClientVectorGraphic* vectorGraphic, CVector2D size, std::optional<CLuaFunctionRef> luaFunctionRef)
{
    if (size.fX <= 0 || size.fY <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    if (size.fX > 4096 || size.fY > 4096)
        throw std::invalid_argument("A vector graphic cannot exceed 4096x4096 in size.");

    bool didLoad = SetSize(vectorGraphic, size);

    // Retain support for callback functions (#2589)
    if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
    {
        vectorGraphic->SetUpdateCallback(luaFunctionRef.value());
    }

    return didLoad;
}

bool CLuaVectorGraphicDefs::SVGSetUpdateCallback(CClientVectorGraphic* vectorGraphic, std::variant<CLuaFunctionRef, bool> luaFunctionRef)
{
    if (std::holds_alternative<CLuaFunctionRef>(luaFunctionRef))
    {
        vectorGraphic->SetUpdateCallback(std::get<CLuaFunctionRef>(luaFunctionRef));
        return true;
    }
    else if (std::get<bool>(luaFunctionRef) == false)
    {
        vectorGraphic->RemoveUpdateCallback();
        return true;
    }

    return false;
}

std::variant<CLuaFunctionRef, bool> CLuaVectorGraphicDefs::SVGGetUpdateCallback(CClientVectorGraphic* vectorGraphic)
{
    auto func = vectorGraphic->GetUpdateCallback();

    if (std::holds_alternative<CLuaFunctionRef>(func))
        return func;

    return false;
}

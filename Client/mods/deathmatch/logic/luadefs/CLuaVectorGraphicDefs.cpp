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

CClientVectorGraphic* CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM, CVector2D size, std::optional<std::string> pathOrRawData,
                                                       std::optional<CLuaFunctionRef> luaFunctionRef)
{
    if (size.fX <= 0 || size.fY <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    if (size.fX > 4096 || size.fY > 4096)
        throw std::invalid_argument("A vector graphic cannot exceed 4096x4096 in size.");

    CLuaMain&  pLuaMain = lua_getownercluamain(luaVM);
    CResource* pParentResource = pLuaMain.GetResource();

    CClientVectorGraphic* pVectorGraphic =
        g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic(static_cast<int>(size.fX), static_cast<int>(size.fY));

    if (!pVectorGraphic)
        return false;

    if (pathOrRawData.has_value())
    {
        std::string& strRawData = pathOrRawData.value();
        std::size_t  szCharPos = strRawData.find_first_not_of(" \t\n\r\f\v", 0);

        char charAuto = 0;

        if (szCharPos != std::string::npos)
            charAuto = strRawData.at(szCharPos);

        if (charAuto && charAuto == '<')
        {
            if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
            {
                CLuaFunctionRef funcRef = luaFunctionRef.value();

                CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
                    [funcRef, pVectorGraphic, strRawData] {
                        lua_State* luaVM = funcRef.GetLuaVM();
                        CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

                        if (!pLuaMain || !pVectorGraphic)
                            return false;

                        if (!pVectorGraphic->LoadFromString(strRawData))
                        {
                            delete pVectorGraphic;

                            m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG data (check for XML syntax errors)");
                            return false;
                        }

                        return true;
                    },
                    [funcRef](const bool didLoad) {
                        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(funcRef.GetLuaVM());
                        if (pLuaMain)
                        {
                            CLuaArguments arguments;
                            arguments.PushBoolean(didLoad);
                            arguments.Call(pLuaMain, funcRef);
                        }
                    });
            }
            else
            {
                if (!pVectorGraphic->LoadFromString(pathOrRawData.value()))
                {
                    delete pVectorGraphic;

                    m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG data (check for XML syntax errors)");
                    return false;
                }
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

                if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
                {
                    CLuaFunctionRef funcRef = luaFunctionRef.value();
                    std::string     path = pathOrRawData.value();

                    CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
                        [funcRef, pFile, pVectorGraphic, path] {
                            lua_State* luaVM = funcRef.GetLuaVM();

                            CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                            CResource* pParentResource = pLuaMain->GetResource();

                            if (!pLuaMain || !pParentResource || !pFile || !pVectorGraphic)
                                return false;

                            if (!pFile->Load(pParentResource, CScriptFile::MODE_READ))
                            {
                                delete pVectorGraphic;
                                delete pFile;

                                m_pScriptDebugging->LogCustom(luaVM, SString("Unable to load SVG (file doesn't exist) [%s]", path.c_str()));
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

                                    m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG file (check for XML syntax errors)");
                                    return false;
                                }
                            }

                            return true;
                        },
                        [funcRef](const bool isLoaded) {
                            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(funcRef.GetLuaVM());
                            if (pLuaMain)
                            {
                                CLuaArguments arguments;
                                arguments.PushBoolean(isLoaded);
                                arguments.Call(pLuaMain, funcRef);
                            }
                        });
                }
                else
                {
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

                            m_pScriptDebugging->LogCustom(luaVM, "Unable to load SVG file (check for XML syntax errors)");
                            return false;
                        }
                    }
                }
            }
            else
            {
                delete pVectorGraphic;

                m_pScriptDebugging->LogCustom(luaVM, SString("Unable to load SVG (invalid file path) [%s]", pathOrRawData.value().c_str()));
                return false;
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

bool CLuaVectorGraphicDefs::SVGSetDocumentXML(CClientVectorGraphic* pVectorGraphic, CXMLNode* pXMLNode, std::optional<CLuaFunctionRef> luaFunctionRef)
{
    if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
    {
        CLuaFunctionRef funcRef = luaFunctionRef.value();

        CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
            [funcRef, pVectorGraphic, pXMLNode] {
                lua_State* luaVM = funcRef.GetLuaVM();
                CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

                if (!pLuaMain || !pVectorGraphic)
                    return false;

                return pVectorGraphic->SetDocument(pXMLNode);
            },
            [funcRef](const bool didLoad) {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(funcRef.GetLuaVM());
                if (pLuaMain)
                {
                    CLuaArguments arguments;
                    arguments.PushBoolean(didLoad);
                    arguments.Call(pLuaMain, funcRef);
                }
            });

        return true;
    }

    return pVectorGraphic->SetDocument(pXMLNode);
}

CLuaMultiReturn<int, int> CLuaVectorGraphicDefs::SVGGetSize(CClientVectorGraphic* pVectorGraphic)
{
    return {(int)pVectorGraphic->GetRenderItem()->m_uiSizeX, (int)pVectorGraphic->GetRenderItem()->m_uiSizeY};
}

bool CLuaVectorGraphicDefs::SVGSetSize(CClientVectorGraphic* pVectorGraphic, int width, int height, std::optional<CLuaFunctionRef> luaFunctionRef)
{
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("A vector graphic must be atleast 1x1 in size.");

    if (width > 4096 || height > 4096)
        throw std::invalid_argument("A vector graphic cannot exceed 4096x4096 in size.");

    CVectorGraphicItem* pVectorGraphicItem = pVectorGraphic->GetRenderItem();

    if (luaFunctionRef.has_value() && VERIFY_FUNCTION(luaFunctionRef.value()))
    {
        CLuaFunctionRef funcRef = luaFunctionRef.value();

        CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
            [funcRef, pVectorGraphic, pVectorGraphicItem, width, height] {
                lua_State* luaVM = funcRef.GetLuaVM();
                CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

                if (!pLuaMain || !pVectorGraphic)
                    return false;

                pVectorGraphicItem->Resize(*new CVector2D(width, height));

                if ((int)pVectorGraphicItem->m_uiSizeX != width || (int)pVectorGraphicItem->m_uiSizeY != height)
                    return false;            // failed to resize

                pVectorGraphic->GetDisplay()->Update();
                return true;
            },
            [funcRef](const bool didLoad) {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(funcRef.GetLuaVM());
                if (pLuaMain)
                {
                    CLuaArguments arguments;
                    arguments.PushBoolean(didLoad);
                    arguments.Call(pLuaMain, funcRef);
                }
            });

        return true;
    }

    pVectorGraphicItem->Resize(*new CVector2D(width, height));

    if ((int)pVectorGraphicItem->m_uiSizeX != width || (int)pVectorGraphicItem->m_uiSizeY != height)
        return false;            // failed to resize

    pVectorGraphic->GetDisplay()->Update();

    return true;
}

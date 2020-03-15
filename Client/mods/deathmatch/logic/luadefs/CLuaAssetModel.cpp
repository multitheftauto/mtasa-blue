/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaAssetModelDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../lua/CLuaAssetNode.h"
#include "../lua/CLuaAssetMesh.h"
#include "core/CLuaAssetNodeInterface.h"

void CLuaAssetModelDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"loadAssetModel", LoadAssetModel},
        {"getAssetProperties", GetAssetProperties},
        {"assetGetChilldrenNodes", AssetGetChilldrenNodes},
        {"assetGetNodeMeshes", AssetGetNodeMeshes},
        {"assetGetTextures", AssetGetTextures},
        {"assetRender", AssetRender},
        {"assetSetTexture", AssetSetTexture},
        {"assetGetMaterialProperties", AssetGetMaterialProperties},
        {"assetGetMetaData", AssetGetMetaData},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaAssetModelDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "create", "setTimer");
    // lua_classfunction(luaVM, "destroy", "killTimer");
    // lua_classfunction(luaVM, "reset", "resetTimer");
    // lua_classfunction(luaVM, "isValid", "isTimer");

    // lua_classfunction(luaVM, "getDetails", "getTimerDetails");

    // lua_classvariable(luaVM, "valid", NULL, "isTimer");

    lua_registerclass(luaVM, "AssetModel");
}

int CLuaAssetModelDefs::LoadAssetModel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        SString         strFileInput;
        CLuaFunctionRef luaFunctionRef;

        CScriptArgReader argStream(luaVM);
        argStream.ReadString(strFileInput);
        if (argStream.NextIsFunction())
        {
            argStream.ReadFunction(luaFunctionRef);
        }

        float   fGlobalScale = 1.0f;
        bool    bRawData = false;
        SString strHint = "rawdata";
        if (argStream.NextIsTable())
        {
            CStringMap optionsMap;

            argStream.ReadStringMap(optionsMap);
            optionsMap.ReadBool("rawdata", bRawData, false);
            optionsMap.ReadString("hint", strHint, "rawdata");
            optionsMap.ReadNumber("globalScale", fGlobalScale, 1.0f);
        }

        argStream.ReadFunctionComplete();
        if (!argStream.HasErrors())
        {
            if (bRawData)
            {
                CResource* pThisResource = pLuaMain->GetResource();
                CResource* pOtherResource = pThisResource;

                CResource* pResource = pLuaMain ? pLuaMain->GetResource() : nullptr;

                if (pResource)
                {
                    auto pAssetModel = CStaticFunctionDefinitions::CreateAssetModel(*pResource);
                    if (pAssetModel)
                    {
                        CElementGroup* pGroup = pResource->GetElementGroup();
                        if (pGroup)
                            pGroup->Add(pAssetModel);

                        Assimp::Importer& importer = pAssetModel->GetImporter();
                        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", fGlobalScale);

                        if (luaFunctionRef == CLuaFunctionRef{})
                        {
                            pAssetModel->LoadFromRawData(strFileInput, strHint);
                            lua_pushelement(luaVM, pAssetModel);
                        }
                        else
                        {
                            CLuaShared::GetAsyncTaskScheduler()->PushTask<const char*>(
                                [strFileInput, pAssetModel, strHint] {
                                    // Execute time-consuming task
                                    return pAssetModel->LoadFromRawData(strFileInput, strHint);
                                },
                                [luaFunctionRef, pAssetModel, pResource](const char* errorMessage) {
                                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                    if (pLuaMain)
                                    {
                                        CLuaArguments arguments;
                                        if (strcmp(errorMessage, "") == 0)
                                        {
                                            arguments.PushBoolean(true);
                                        }
                                        else
                                        {
                                            arguments.PushBoolean(false);
                                            pAssetModel->Cache();
                                            pAssetModel->CacheTextures(pResource);
                                        }
                                        arguments.PushString(errorMessage);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                });

                            lua_pushelement(luaVM, pAssetModel);
                            return 1;
                        }
                    }
                }
            }
            else
            {
                SString    strPath;
                CResource* pThisResource = pLuaMain->GetResource();
                CResource* pOtherResource = pThisResource;

                // Resolve other resource from name
                if (CResourceManager::ParseResourcePathInput(strFileInput, pOtherResource, &strPath))
                {
                    CResource* pResource = pLuaMain ? pLuaMain->GetResource() : nullptr;

                    if (pResource)
                    {
                        auto pAssetModel = CStaticFunctionDefinitions::CreateAssetModel(*pResource);
                        if (pAssetModel)
                        {
                            CElementGroup* pGroup = pResource->GetElementGroup();
                            if (pGroup)
                                pGroup->Add(pAssetModel);

                            Assimp::Importer& importer = pAssetModel->GetImporter();
                            importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR", fGlobalScale);
                            if (luaFunctionRef == CLuaFunctionRef{})
                            {
                                pAssetModel->LoadFromFile(strPath);
                                lua_pushelement(luaVM, pAssetModel);
                            }
                            else
                            {
                                CLuaShared::GetAsyncTaskScheduler()->PushTask<const char*>(
                                    [strPath, pAssetModel] {
                                        // Execute time-consuming task
                                        return pAssetModel->LoadFromFile(strPath);
                                    },
                                    [luaFunctionRef, pAssetModel, pResource](const char* errorMessage) {
                                        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                        if (pLuaMain)
                                        {
                                            CLuaArguments arguments;
                                            if (strcmp(errorMessage, "") == 0)
                                            {
                                                arguments.PushBoolean(true);
                                                pAssetModel->Cache();
                                                pAssetModel->CacheTextures(pResource);
                                            }
                                            else
                                            {
                                                arguments.PushBoolean(false);
                                            }
                                            arguments.PushString(errorMessage);
                                            arguments.Call(pLuaMain, luaFunctionRef);
                                        }
                                    });

                                lua_pushelement(luaVM, pAssetModel);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::GetAssetProperties(lua_State* luaVM)
{
    //  float GetAssetProperties ( searchlight light )
    CClientAssetModel* pAssetModel = nullptr;
    CLuaAssetNode*     pAssetNode = nullptr;
    CLuaAssetMesh*     pAssetMesh = nullptr;
    eAssetProperty     eProperty;
    CScriptArgReader   argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientAssetModel>())
        argStream.ReadUserData(pAssetModel);
    else if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);
    else if (argStream.NextIsUserDataOfType<CLuaAssetMesh>())
        argStream.ReadUserData(pAssetMesh);

    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        int i;
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
            i = pAssetModel->GetProperties(luaVM, eProperty);
        }
        else if (pAssetNode != nullptr)
            i = pAssetNode->GetProperties(luaVM, eProperty);
        else if (pAssetMesh != nullptr)
            i = pAssetMesh->GetProperties(luaVM, eProperty);

        return i;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetNodeMeshes(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel = nullptr;
    CLuaAssetNode*     pAssetNode = nullptr;
    eAssetProperty     eProperty;
    CScriptArgReader   argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientAssetModel>())
        argStream.ReadUserData(pAssetModel);
    else if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);

    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
            pAssetModel->GetMeshes(luaVM);
            return 1;
        }
        else
        {
            // std::vector<CLuaAssetNode*> childNodes = pAssetNode->GetChildNodes();
            // lua_newtable(luaVM);
            // for (int i = 0; i < childNodes.size(); i++)
            //{
            //    lua_pushnumber(luaVM, i + 1);
            //    lua_pushassetnode(luaVM, childNodes[i]);
            //    lua_settable(luaVM, -3);
            //}
            return 1;
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetTextures(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel = nullptr;
    CLuaAssetNode*     pAssetNode = nullptr;
    eAssetProperty     eProperty;
    CScriptArgReader   argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientAssetModel>())
        argStream.ReadUserData(pAssetModel);
    else if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);

    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
            pAssetModel->GetTextures(luaVM);
            return 1;
        }
        else
        {
            // std::vector<CLuaAssetNode*> childNodes = pAssetNode->GetChildNodes();
            // lua_newtable(luaVM);
            // for (int i = 0; i < childNodes.size(); i++)
            //{
            //    lua_pushnumber(luaVM, i + 1);
            //    lua_pushassetnode(luaVM, childNodes[i]);
            //    lua_settable(luaVM, -3);
            //}
            return 1;
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetChilldrenNodes(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel = nullptr;
    CLuaAssetNode*     pAssetNode = nullptr;
    eAssetProperty     eProperty;
    CScriptArgReader   argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientAssetModel>())
        argStream.ReadUserData(pAssetModel);
    else if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);

    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushassetnode(luaVM, pAssetModel->GetNode());
            lua_settable(luaVM, -3);
            return 1;
        }
        else if (pAssetNode != nullptr)
        {
            std::vector<CLuaAssetNode*> childNodes = pAssetNode->GetChildNodes();
            lua_newtable(luaVM);
            for (int i = 0; i < childNodes.size(); i++)
            {
                lua_pushnumber(luaVM, i + 1);
                lua_pushassetnode(luaVM, childNodes[i]);
                lua_settable(luaVM, -3);
            }
            return 1;
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetRender(lua_State* luaVM)
{
    //  bool assetRender ( asset-node theAssetNode, position, rotation, scale, options  )
    CLuaAssetNode*   pAssetNode = nullptr;
    CStringMap       optionsMap;
    CVector          vecPosition;
    CVector          vecRotation;
    CVector          vecScale = CVector(1, 1, 1);
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAssetNode);
    argStream.ReadVector3D(vecPosition);
    if (argStream.NextIsVector3D())
        argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    if (argStream.NextIsVector3D())
        argStream.ReadVector3D(vecScale, CVector(1, 1, 1));

    argStream.ReadStringMap(optionsMap);

    if (!argStream.HasErrors())
    {
        SRenderingSettings settings;
        settings.matrix.SetPosition(vecPosition);
        ConvertDegreesToRadiansNoWrap(vecRotation);
        settings.matrix.SetRotation(vecRotation);
        settings.matrix.SetScale(vecScale);
        settings.assetNode = (CLuaAssetNodeInterface*)pAssetNode;
        g_pCore->GetGraphics()->DrawAssetNode(settings);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetSetTexture(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel;
    CClientMaterial*   pMaterialElement;
    int                iIndex;
    bool               bResetTexture = false;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pAssetModel);
    argStream.ReadNumber(iIndex);
    if (argStream.NextIsBool())
        argStream.ReadBool(bResetTexture);
    else
        MixedReadMaterialString(argStream, pMaterialElement);

    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }

            iIndex--;
            if (bResetTexture)
            {
                lua_pushboolean(luaVM, pAssetModel->SetTexture(iIndex, nullptr));
                return 1;
            }
            else
            {
                if (pMaterialElement)
                {
                    lua_pushboolean(luaVM, pAssetModel->SetTexture(iIndex, pMaterialElement));
                    return 1;
                }
            }
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetMaterialProperties(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel;
    int                iIndex;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pAssetModel);
    argStream.ReadNumber(iIndex);
    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }

            iIndex--;
            pAssetModel->GetMaterialProperties(luaVM, iIndex);
            return 1;
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetMetaData(lua_State* luaVM)
{
    CClientAssetModel* pAssetModel = nullptr;
    CLuaAssetNode*     pAssetNode = nullptr;
    eAssetProperty     eProperty;
    CScriptArgReader   argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientAssetModel>())
        argStream.ReadUserData(pAssetModel);
    else if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);
    if (!argStream.HasErrors())
    {
        if (pAssetModel != nullptr)
        {
            if (!pAssetModel->IsLoaded())
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }

            pAssetModel->GetMetaData(luaVM);
            return 1;
        }
        else if (pAssetNode != nullptr)
        {
            pAssetNode->GetMetaData(luaVM);
            return 1;
        }
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

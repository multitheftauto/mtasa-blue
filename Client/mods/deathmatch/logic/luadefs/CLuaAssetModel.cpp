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
#include <lua/CLuaFunctionParser.h>
#include "../lua/CLuaAssetNode.h"
#include "../lua/CLuaAssetMesh.h"
#include "core/CLuaAssetNodeInterface.h"
#include "../core/CAssetInstance.h"

void CLuaAssetModelDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"loadAssetModel", LoadAssetModel},
        {"getAssetProperties", ArgumentParser<GetAssetModelProperty>},
        //{"getAssetProperties", ArgumentParser<GetAssetModelProperty, GetAssetNodeProperty, GetAssetMeshProperty>},
        {"assetGetNodes", AssetGetNodes},
        //{"assetGetNodeMeshes", ArgumentParser<AssetGetModelMeshes, AssetGetNodeMeshes>},
        {"assetGetTextures", AssetGetTextures},
        {"assetRender", AssetRender},
        {"assetSetTexture", AssetSetTexture},
        {"assetGetMaterialProperties", AssetGetMaterialProperties},
        //{"assetGetMetaData", ArgumentParser<AssetGetModelMetaData, AssetGetNodeMetaData>},
        {"assetCreateInstance", AssetCreateInstance},
        {"assetGetRenderGroupProperties", AssetGetRenderGroupProperties},
        {"assetSetRenderGroupProperties", AssetSetRenderGroupProperties},
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

unsigned int CLuaAssetModelDefs::GetAssetModelProperty(CClientAssetModel* pModel, eAssetProperty eProperty)
{
    if (!pModel->IsLoaded())
        throw std::invalid_argument("Model is not loaded yet.");

    switch (eProperty)
    {
        case ASSET_ANIMATIONS_COUNT:
            return pModel->GetAnimationsCount();
        case ASSET_CAMERAS_COUNT:
            return pModel->GetCamerasCount();
        case ASSET_LIGHTS_COUNT:
            return pModel->GetLightsCount();
        case ASSET_MATERIALS_COUNT:
            return pModel->GetMaterialsCount();
        case ASSET_MESHES_COUNT:
            return pModel->GetMeshesCount();
        case ASSET_TEXTURES_COUNT:
            return pModel->GetTexturesCount();
        case ASSET_NODES_COUNT:
            return pModel->GetNodesCount();
        default:
            throw std::invalid_argument("Unsupported property");
    }
}

std::variant<unsigned int, float, std::tuple<CVector, CVector>, const char*, CVector, CLuaAssetNode*> CLuaAssetModelDefs::GetAssetNodeProperty(
    CLuaAssetNode* pNode, eAssetProperty eProperty)
{
    switch (eProperty)
    {
        case ASSET_NAME:
            return pNode->GetName();
        case ASSET_POSITION:
            return pNode->GetPosition();
        case ASSET_ROTATION:
            return pNode->GetRotation();
        case ASSET_SCALE:
            return pNode->GetScale();
        case ASSET_MESHES_COUNT:
            return pNode->GetMeshesCount();
        case ASSET_CHILD_NODES_COUNT:
            return pNode->GetChildrenCount();
        case ASSET_PARENT_NODE:
            return pNode->GetParentNode();
        case ASSET_BOUNDING_BOX:
            return pNode->GetCVectorBoundingBox();
        default:
            throw std::invalid_argument("Unsupported property");
    }
}

std::variant<unsigned int, std::tuple<CVector, CVector>, std::tuple<CVector4D, CVector4D>> CLuaAssetModelDefs::GetAssetMeshProperty(CLuaAssetMesh* pMesh,
                                                                                                                                    eAssetProperty eProperty)
{
    switch (eProperty)
    {
        case ASSET_VERTICES_COUNT:
            return pMesh->GetVerticesCount();
        case ASSET_FACES_COUNT:
            return pMesh->GetFacesCount();
        case ASSET_UV_COMPONENTS_COUNT:
            unsigned int uvComponents[8];
            pMesh->GetUVComponentsCount(&uvComponents[0]);
            return std::make_tuple(CVector4D(uvComponents[0], uvComponents[1], uvComponents[2], uvComponents[3]),
                                   CVector4D(uvComponents[4], uvComponents[5], uvComponents[6], uvComponents[7]));
        case ASSET_UV_CHANNELS:
            return pMesh->GetUVChannels();
        case ASSET_COLOR_CHANNELS:
            return pMesh->GetColorChannelsCount();
        case ASSET_BONES_COUNT:
            return pMesh->GetBonesCount();
        case ASSET_BOUNDING_BOX:
            return pMesh->GetBoundingBox();
        default:
            throw std::invalid_argument("Unsupported property");
    }
}

std::variant<bool, std::vector<CLuaAssetMesh*>> CLuaAssetModelDefs::AssetGetModelMeshes(CClientAssetModel* pAssetModel)
{
    if (!pAssetModel->IsLoaded())
        return false;

    std::vector<std::shared_ptr<CLuaAssetMesh>> sharedMeshes = pAssetModel->GetMeshes();
    std::vector<CLuaAssetMesh*>                 meshes(sharedMeshes.size());
    for (const auto& item : sharedMeshes)
    {
        meshes.push_back(item.get());
    }
    return meshes;
}

std::vector<CLuaAssetMesh*> CLuaAssetModelDefs::AssetGetNodeMeshes(CLuaAssetNode* pAssetNode)
{
    std::vector<std::shared_ptr<CLuaAssetMesh>> sharedMeshes = pAssetNode->GetMeshes();
    std::vector<CLuaAssetMesh*>                 meshes(sharedMeshes.size());
    for (const auto& item : sharedMeshes)
    {
        meshes.push_back(item.get());
    }
    return meshes;
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
            // pAssetModel->GetTextures(luaVM);
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

int CLuaAssetModelDefs::AssetGetNodes(lua_State* luaVM)
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
    //  bool assetRender ( asset-node theAssetNode, position, rotation, scale )
    CLuaAssetNode*   pAssetNode = nullptr;
    unsigned int     uiGroup;
    CVector          vecPosition;
    CVector          vecRotation;
    CVector          vecScale = CVector(1, 1, 1);
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAssetNode);
    if (pAssetNode->GetMeshNum() == 0) // nothing will render, ignore
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    argStream.ReadVector3D(vecPosition);
    if (argStream.NextIsVector3D())
        argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    if (argStream.NextIsVector3D())
        argStream.ReadVector3D(vecScale, CVector(1, 1, 1));

    if (!argStream.HasErrors())
    {
        std::unique_ptr<SRenderAssetItem> settings = std::make_unique<SRenderAssetItem>();
        settings->matrix.SetPosition(vecPosition);
        ConvertDegreesToRadiansNoWrap(vecRotation);
        settings->matrix.SetRotation(vecRotation);
        settings->matrix.SetScale(vecScale);
        settings->assetNode = (CLuaAssetNodeInterface*)pAssetNode;
        g_pCore->GetGraphics()->DrawAssetNode(std::move(settings));
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

std::unordered_map<std::string, std::variant<bool, int, uint64_t, float, double, std::string, CVector>> CLuaAssetModelDefs::AssetGetModelMetaData(
    CClientAssetModel* pAssetModel)
{
    if (!pAssetModel->IsLoaded())
        throw std::invalid_argument("Model is not loaded yet.");

    std::unordered_map<std::string, std::variant<bool, int, uint64_t, float, double, std::string, CVector>> mapMetadata;
    for (auto const& item : pAssetModel->GetBoolMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetIntMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetInt64Metdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetFloatMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetDoubleMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetStringMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pAssetModel->GetVectorMetdata())
        mapMetadata[item.first] = item.second;

    return mapMetadata;
}

std::unordered_map<std::string, std::variant<bool, int, uint64_t, float, double, std::string, CVector>> CLuaAssetModelDefs::AssetGetNodeMetaData(
    CLuaAssetNode* pNode)
{
    std::unordered_map<std::string, std::variant<bool, int, uint64_t, float, double, std::string, CVector>> mapMetadata;
    for (auto const& item : pNode->GetBoolMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetIntMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetInt64Metdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetFloatMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetDoubleMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetStringMetdata())
        mapMetadata[item.first] = item.second;
    for (auto const& item : pNode->GetVectorMetdata())
        mapMetadata[item.first] = item.second;

    return mapMetadata;
}

int CLuaAssetModelDefs::AssetCreateInstance(lua_State* luaVM)
{
    CLuaAssetNode*   pAssetNode = nullptr;
    eAssetProperty   eProperty;
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CLuaAssetNode>())
        argStream.ReadUserData(pAssetNode);
    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetGetRenderGroupProperties(lua_State* luaVM)
{
    unsigned int            uiGroup;
    eAssetRenderingProperty eProperty;
    CScriptArgReader        argStream(luaVM);

    argStream.ReadNumber(uiGroup);
    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        // CAssetInstance* pGroup = g_pCore->GetAssetsControl()->GetRenderGroup(uiGroup);

        switch (eProperty)
        {
            case ASSET_REDNERING_PROPERTY_DRAW_DISTANCE:
            {
                // lua_pushnumber(luaVM, pGroup->GetDrawDistance());
                return 1;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAssetModelDefs::AssetSetRenderGroupProperties(lua_State* luaVM)
{
    unsigned int            uiGroup;
    eAssetRenderingProperty eProperty;
    CScriptArgReader        argStream(luaVM);

    argStream.ReadNumber(uiGroup);
    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        bool    booleanValue;
        float   floatValue;
        CVector vector;
        if (eProperty & ASSET_RENDERING_PROPERTY_TYPE_BOOL)
        {
            if (!argStream.NextIsBool())
            {
                argStream.SetCustomError(SString("Rendering property %s require boolean value", EnumToString(eProperty).c_str()).c_str());
            }
            else
            {
                argStream.ReadBool(booleanValue);
            }
        }
        else if (eProperty & ASSET_RENDERING_PROPERTY_TYPE_FLOAT)
        {
            if (!argStream.NextIsNumber())
            {
                argStream.SetCustomError(SString("Rendering property %s require float value", EnumToString(eProperty).c_str()).c_str());
            }
            else
            {
                argStream.ReadNumber(floatValue);
            }
        }
        else if (eProperty & ASSET_RENDERING_PROPERTY_TYPE_FLOAT)
        {
            if (!argStream.NextIsVector3D())
            {
                argStream.SetCustomError(SString("Rendering property %s require vector3 value", EnumToString(eProperty).c_str()).c_str());
            }
            else
            {
                argStream.ReadVector3D(vector);
            }
        }

        if (!argStream.HasErrors())
        {
            // CAssetInstance* pGroup = g_pCore->GetAssetsControl()->GetRenderGroup(uiGroup);

            switch (eProperty)
            {
                case ASSET_REDNERING_PROPERTY_DRAW_DISTANCE:
                {
                    // pGroup->SetDrawDistance(floatValue);
                    break;
                }
            }

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

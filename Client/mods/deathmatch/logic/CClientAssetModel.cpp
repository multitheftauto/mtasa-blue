/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModel.cpp
 *  PURPOSE:     AssetModel entity class
 *
 *****************************************************************************/

#include <StdInc.h>
#include "../logic/lua/CLuaAssetNode.h"
#include "../logic/lua/CLuaAssetMesh.h"

CClientAssetModel::CClientAssetModel(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pAssetModelManager = pManager->GetAssetModelManager();

    SetTypeName("asset-model");

    m_uiImportFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes;

    // Add us to the manager's list
    m_pAssetModelManager->AddToList(this);
}

CClientAssetModel::~CClientAssetModel()
{
    // Remove us from the manager's list
    Unlink();
}

void CClientAssetModel::Unlink()
{
    m_pAssetModelManager->RemoveFromList(this);
}

int CClientAssetModel::GetProperties(lua_State* luaVM, eAssetProperty assetProperty)
{
    switch (assetProperty)
    {
        case ASSET_ANIMATIONS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumAnimations);
            return 1;
        case ASSET_CAMERAS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumCameras);
            return 1;
        case ASSET_LIGHTS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumLights);
            return 1;
        case ASSET_MATERIALS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumMaterials);
            return 1;
        case ASSET_MESHES_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumMeshes);
            return 1;
        case ASSET_TEXTURES_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumTextures);
            return 1;
        case ASSET_NODES_COUNT:
            lua_pushnumber(luaVM, vecNodes.size());
            return 1;
        default:
            lua_pushboolean(luaVM, false);
    }
}

CLuaAssetNode* CClientAssetModel::GetNode(const aiNode* pNode)
{
    if (pNode == nullptr)
        pNode = m_pScene->mRootNode;

    for (const auto& a : m_vecAssetRootNode)
    {
        if (a->GetNode() == pNode)
        {
            return a;
        }
    }

    return nullptr;
}

void CClientAssetModel::CacheNodes(const aiNode* pNode)
{
    for (int i = 0; i < pNode->mNumChildren; i++)
    {
        vecNodes.push_back(pNode->mChildren[i]);
        CacheNodes(pNode->mChildren[i]);
    }
    m_vecAssetRootNode.push_back(new CLuaAssetNode(this, pNode));
}

const char* CClientAssetModel::LoadFromRawData(const SString& strPath, const SString& strHint)
{
    m_pScene = importer.ReadFileFromMemory(strPath, strPath.size(), m_uiImportFlags, strHint.c_str());
    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
    {
        return importer.GetErrorString();
    }

    CacheNodes(m_pScene->mRootNode);
    m_bModelLoaded = true;
    return "";
}

const char* CClientAssetModel::LoadFromFile(std::string strPath)
{
    m_pScene = importer.ReadFile(strPath, m_uiImportFlags);
    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
    {
        return importer.GetErrorString();
    }

    CacheNodes(m_pScene->mRootNode);
    m_bModelLoaded = true;
    return "";
}

void CClientAssetModel::DoPulse()
{
    // Update our position/rotation if we're attached
    // DoAttaching();
}
/*
void CClientAssetModel::SetDimension(unsigned short usDimension)
{
    m_usDimension = usDimension;
}

void CClientAssetModel::RelateDimension(unsigned short usDimension)
{
    m_bStreamedIn = (usDimension == m_usDimension);
}*/

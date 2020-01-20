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

void CClientAssetModel::GetProperties(lua_State* luaVM, eAssetProperty assetProperty)
{
    switch (assetProperty)
    {
        case ASSET_ANIMATIONS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumAnimations);
            break;
        case ASSET_CAMERAS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumCameras);
            break;
        case ASSET_LIGHTS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumLights);
            break;
        case ASSET_MATERIALS_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumMaterials);
            break;
        case ASSET_MESHES_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumMeshes);
            break;
        case ASSET_TEXTURES_COUNT:
            lua_pushnumber(luaVM, m_pScene->mNumTextures);
            break;
        case ASSET_NODES_COUNT:
            lua_pushnumber(luaVM, vecNodes.size());
            break;
        default:
            lua_pushboolean(luaVM, false);
    }
}

void CClientAssetModel::GetLuaNode(lua_State* luaVM, const aiNode* pNode)
{
    lua_newtable(luaVM);
    if (pNode == nullptr)
    {
        CLuaAssetNode* pAssetNode = new CLuaAssetNode(this, m_pScene->mRootNode);
        lua_pushnumber(luaVM, 1);
        lua_pushassetnode(luaVM, pAssetNode);
        lua_settable(luaVM, -3);
    }
    else
    {
        for (int i = 0; i < pNode->mNumChildren; i++)
        {
        }
    }
}

void CClientAssetModel::CacheNodes(const aiNode* pNode)
{
    for (int i = 0; i < pNode->mNumChildren; i++)
    {
        vecNodes.push_back(pNode->mChildren[i]);
        CacheNodes(pNode->mChildren[i]);
    }
}

bool CClientAssetModel::LoadFromFile(std::string strPath)
{
    m_pScene = importer.ReadFile(strPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure);

    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
    {
        return false;
    }

    vecNodes.push_back(m_pScene->mRootNode);

    return true;
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

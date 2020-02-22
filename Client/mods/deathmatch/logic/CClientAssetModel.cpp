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

class CClientTexture;

CClientMeshBuffer::~CClientMeshBuffer()
{
    m_pIndexBuffer->Release();
    delete m_pIndexBuffer;
    for (int i = 0; i < 8; i++)
    {
        m_arrayVertexBuffer[i]->Release();
    }
    delete[] m_arrayVertexBuffer;
}

CClientAssetModel::CClientAssetModel(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pAssetModelManager = pManager->GetAssetModelManager();

    SetTypeName("asset-model");

    m_uiImportFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes | aiProcess_EmbedTextures |
                      aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

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
            return 1;
    }
}

CLuaAssetNode* CClientAssetModel::GetNode(const aiNode* pNode)
{
    if (pNode == nullptr)
        pNode = m_pScene->mRootNode;

    for (const auto& pAssetNode : m_vecAssetNodes)
        if (pAssetNode->GetNode() == pNode)
            return pAssetNode;

    return nullptr;
}

void CClientAssetModel::GetMeshes(lua_State* luaVM)
{
    lua_newtable(luaVM);
    for (int i = 0; i < m_vecAssetMeshes.size(); i++)
    {
        lua_pushnumber(luaVM, i + 1);
        lua_pushassetmesh(luaVM, m_vecAssetMeshes[i]);
        lua_settable(luaVM, -3);
    }
}

void CClientAssetModel::GetTextures(lua_State* luaVM)
{
    lua_newtable(luaVM);
    for (int i = 0; i < m_vecAssetTextures.size(); i++)
    {
        lua_pushnumber(luaVM, i + 1);
        lua_pushelement(luaVM, m_vecAssetTextures[i].pClientTexture);
        lua_settable(luaVM, -3);
    }
}

void CClientAssetModel::CacheMeshes()
{
    m_vecAssetMeshes.reserve(m_pScene->mNumMeshes);
    for (int i = 0; i < m_pScene->mNumMeshes; i++)
    {
        m_vecAssetMeshes.push_back(new CLuaAssetMesh(this, m_pScene->mMeshes[i]));
    }
}

void CClientAssetModel::CacheTextures(CResource* pParentResource)
{
    m_vecAssetTextures.reserve(m_pScene->mNumTextures);
    aiTexture* pAiTexture = nullptr;
    for (int i = 0; i < m_pScene->mNumTextures; i++)
    {
        pAiTexture = m_pScene->mTextures[i];
        CPixels pixels;
        if (pAiTexture->mHeight > 0)
        {
            pixels.SetSize(pAiTexture->mHeight * pAiTexture->mWidth);
        }
        else
        {
            pixels.SetSize(pAiTexture->mWidth);
        }
        pixels.buffer = CBuffer(m_pScene->mTextures[i]->pcData, pixels.GetSize());
        CClientTexture* pTexture =
            g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", &pixels, true, RDEFAULT, RDEFAULT, RFORMAT_UNKNOWN, TADDRESS_WRAP);
        m_vecAssetTextures.push_back(SAssetTexture(pAiTexture->mFilename.C_Str(), pTexture));
        if (pTexture)
        {
            pTexture->SetParent(pParentResource->GetResourceDynamicEntity());
        }
        // m_pScene->mTextures[i]->pcData.
    }
    // m_vecAssetMeshes.push_back(new CLuaAssetMesh(this, m_pScene->mMeshes[i]));
}

void CClientAssetModel::CacheNodes(const aiNode* pNode)
{
    for (int i = 0; i < pNode->mNumChildren; i++)
    {
        vecNodes.push_back(pNode->mChildren[i]);
        CacheNodes(pNode->mChildren[i]);
    }
    m_vecAssetNodes.push_back(new CLuaAssetNode(this, pNode));
}

const char* CClientAssetModel::LoadFromRawData(const SString& strPath, const SString& strHint)
{
    m_pScene = importer.ReadFileFromMemory(strPath, strPath.size(), m_uiImportFlags, strHint.c_str());
    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
    {
        return importer.GetErrorString();
    }

    CacheNodes(m_pScene->mRootNode);
    CacheMeshes();
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
    CacheMeshes();
    m_bModelLoaded = true;
    return "";
}

void CClientAssetModel::Render(SRenderingSettings* settings)
{

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

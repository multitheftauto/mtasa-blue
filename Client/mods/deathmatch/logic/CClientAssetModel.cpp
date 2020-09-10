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
#include "CClientAssetModel.h"
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
    m_pProgressHandler = std::make_unique<CAssetProgressHandler>();
    SetTypeName("asset-model");

    m_uiImportFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes | aiProcess_EmbedTextures |
                      aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

    // Add us to the manager's list
    m_pAssetModelManager->AddToList(this);
}

CClientAssetModel::~CClientAssetModel()
{
}

void CClientAssetModel::Unlink()
{
    if (m_pProgressHandler != nullptr)
        m_pProgressHandler->bLoad = false;
    m_pAssetModelManager->RemoveFromList(this);
}

CLuaAssetNode* CClientAssetModel::GetNode(const aiNode* pNode)
{
    if (pNode == nullptr)
        pNode = m_pScene->mRootNode;

    for (const auto& pAssetNode : m_vecAssetNodes)
        if (pAssetNode->GetNode() == pNode)
            return pAssetNode.get();

    return nullptr;
}


void CClientAssetModel::CacheNodesAndMeshes(const aiNode* pNode)
{
    for (int i = 0; i < pNode->mNumChildren; i++)
    {
        vecNodes.push_back(pNode->mChildren[i]);
        CacheNodesAndMeshes(pNode->mChildren[i]);
    }
    std::shared_ptr<CLuaAssetNode> pAssetNode = std::make_shared<CLuaAssetNode>(this, pNode);
    for (int i = 0; i < pNode->mNumMeshes; i++)
    {
        std::shared_ptr<CLuaAssetMesh> pMesh = std::make_shared<CLuaAssetMesh>(this, m_pScene->mMeshes[i], pAssetNode);
        m_vecAssetMeshes.push_back(pMesh);
    }
    m_vecAssetNodes.push_back(pAssetNode);
}

void CClientAssetModel::GetMaterialProperties(lua_State* luaVM, int iMaterialIndex)
{
    if (iMaterialIndex >= 0 && iMaterialIndex < m_pScene->mNumMaterials)
    {
        lua_newtable(luaVM);
        aiMaterial* pMaterial = m_pScene->mMaterials[iMaterialIndex];
        for (int i = 0; i < pMaterial->mNumProperties; i++)
        {
            aiMaterialProperty* pProperty = pMaterial->mProperties[i];
            lua_pushstring(luaVM, pProperty->mKey.C_Str());
            aiString*   str = new aiString();
            const char* pChar;
            switch (pProperty->mType)
            {
                case aiPTI_Float:
                    lua_pushnumber(luaVM, strtof(pProperty->mData, NULL));
                    break;
                case aiPTI_Double:
                    lua_pushnumber(luaVM, atof(pProperty->mData));
                    break;
                case aiPTI_String:
                    str = (aiString*)malloc(pProperty->mDataLength);
                    memcpy(str, pProperty->mData, pProperty->mDataLength);
                    pChar = str->C_Str();
                    lua_pushstring(luaVM, str->C_Str());
                    break;
                case aiPTI_Buffer:
                    lua_pushlstring(luaVM, pProperty->mData, pProperty->mDataLength - 1);
                    break;
                case aiPTI_Integer:
                    lua_pushnumber(luaVM, atoi(pProperty->mData));
                    break;
            }
            lua_settable(luaVM, -3);
        }
    }
}

void CClientAssetModel::GetMetaData(lua_State* luaVM)
{
    lua_newtable(luaVM);
    if (m_pScene->mMetaData == nullptr)
    {
        return;
    }
    for (int i = 0; i < m_pScene->mMetaData->mNumProperties; i++)
    {
        aiString*        pKeyName = &m_pScene->mMetaData->mKeys[i];
        aiMetadataEntry* pValue = &m_pScene->mMetaData->mValues[i];
        lua_pushstring(luaVM, pKeyName->C_Str());
        bool       boolValue;
        float      floatValue;
        uint64_t   longValue;
        double     doubleValue;
        aiString   stringValue;
        int        intValue;
        aiVector3D vector3Value;
        switch (pValue->mType)
        {
            case AI_BOOL:
                m_pScene->mMetaData->Get<bool>(*pKeyName, boolValue);
                lua_pushboolean(luaVM, boolValue);
                break;
            case AI_INT32:
                m_pScene->mMetaData->Get<int>(*pKeyName, intValue);
                lua_pushnumber(luaVM, intValue);
                break;
            case AI_UINT64:
                m_pScene->mMetaData->Get<uint64_t>(*pKeyName, longValue);
                lua_pushnumber(luaVM, longValue);
                break;
            case AI_FLOAT:
                m_pScene->mMetaData->Get<float>(*pKeyName, floatValue);
                lua_pushnumber(luaVM, floatValue);
                break;
            case AI_DOUBLE:
                m_pScene->mMetaData->Get<double>(*pKeyName, doubleValue);
                lua_pushnumber(luaVM, doubleValue);
                break;
            case AI_AISTRING:
                m_pScene->mMetaData->Get<aiString>(*pKeyName, stringValue);
                lua_pushstring(luaVM, stringValue.C_Str());
                break;
            case AI_AIVECTOR3D:
                m_pScene->mMetaData->Get<aiVector3D>(*pKeyName, vector3Value);
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, vector3Value.x);
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, vector3Value.y);
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, vector3Value.z);
                lua_settable(luaVM, -3);
                break;
        }
        lua_settable(luaVM, -3);
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

const char* CClientAssetModel::LoadFromRawData(const SString& strPath, const SString& strHint)
{
    importer.SetProgressHandler(m_pProgressHandler.get());
    const aiScene* pScene = importer.ReadFileFromMemory(strPath, strPath.size(), m_uiImportFlags, strHint.c_str());
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
    {
        return importer.GetErrorString();
    }

    m_pScene.reset(pScene);
    m_bModelLoaded = true;
    return "";
}

const char* CClientAssetModel::LoadFromFile(std::string strPath)
{
    importer.SetProgressHandler(m_pProgressHandler.get());
    const aiScene* pScene = importer.ReadFile(strPath, m_uiImportFlags);
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
    {
        return importer.GetErrorString();
    }

    m_pScene.reset(pScene);
    m_bModelLoaded = true;
    return "";
}

SAssetTexture* CClientAssetModel::GetTexture(int idx)
{
    if (idx >= 0 && idx < m_vecAssetTextures.size())
        return &m_vecAssetTextures[idx];
    return nullptr;
}

bool CClientAssetModel::SetTexture(int idx, CClientMaterial* pMaterial)
{
    if (idx >= 0 && idx < m_vecAssetTextures.size())
    {
        m_vecAssetTextures[idx].pMaterialElement = pMaterial;
    }
    return false;
}

std::vector<std::shared_ptr<CLuaAssetMesh>> CClientAssetModel::GetMeshesOfNode(CLuaAssetNode* pNode)
{
    std::vector<std::shared_ptr<CLuaAssetMesh>> vecMeshes;
    for (const auto& mesh : m_vecAssetMeshes)
    {
        if (mesh->GetNode().get() == pNode)
            vecMeshes.push_back(mesh);
    }
    return vecMeshes;
}

void CClientAssetModel::Cache()
{
    CacheNodesAndMeshes(m_pScene->mRootNode);
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

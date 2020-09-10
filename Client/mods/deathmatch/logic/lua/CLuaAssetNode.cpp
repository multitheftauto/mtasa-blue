/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetNode.cpp
 *  PURPOSE:     Lua asset node class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>
using namespace Assimp;

#include "StdInc.h"
#include "CLuaAssetNode.h"
#include "CLuaAssetMesh.h"

CLuaAssetNode::CLuaAssetNode(CClientAssetModel* pAssetModel, const aiNode* pNode)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ASSETNODE);
    m_pAssetModel = pAssetModel;
    m_pNode = pNode;
    m_vecMeshes = pAssetModel->GetMeshesOfNode(this);
}

CLuaAssetNode::~CLuaAssetNode()
{
    RemoveScriptID();
}

void CLuaAssetNode::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::ASSETNODE, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaAssetNode::AddToRenderQueue(std::unique_ptr<SRenderAssetItem> renderItem)
{
    // m_vecRender.push_back(std::move(settings));
    g_pCore->GetGraphics()->DrawAssetNode(std::move(renderItem));
}

CClientMeshBuffer* CLuaAssetNode::GetMeshBuffer(int idx)
{
    return m_pAssetModel->GetMeshBuffer(m_pNode->mMeshes[idx]);
}

CMaterialItem* CLuaAssetNode::GetTexture(int idx)
{
    SAssetTexture* pAssetTexture = m_pAssetModel->GetTexture(idx);
    if (pAssetTexture)
    {
        if (pAssetTexture->pMaterialElement)
        {
            return pAssetTexture->pMaterialElement->GetMaterialItem();
        }
        if (pAssetTexture->pClientTexture)
        {
            return pAssetTexture->pClientTexture->GetMaterialItem();
        }
    }
    return nullptr;
}

size_t CLuaAssetNode::GetMeshNum()
{
    return m_pNode->mNumMeshes;
}

// void CLuaAssetNode::Render(SRenderingSettings& settings)
//{
//    CClientMeshBuffer* pMeshBuffer;
//    CLuaAssetMesh*     pLuaMesh;
//    IDirect3DDevice9*  device = g_pCore->GetGraphics()->GetDevice();
//
//    // for (int i = 0; i < m_pNode->mNumMeshes; i++)
//    //{
//    //    pLuaMesh = m_pAssetModel->GetMesh(m_pNode->mMeshes[i]);
//    //    }
//    //    else
//    //        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMeshBuffer->m_iIndicesCount, 0, pMeshBuffer->m_iFaceCount);
//    //}
//}

CLuaAssetNode* CLuaAssetNode::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaAssetNode* pLuaNode = (CLuaAssetNode*)CIdArray::FindEntry(uiScriptID, EIdClass::ASSETNODE);
    return pLuaNode;
}

CMatrix convertAiMatrixToCMatrix(const aiMatrix4x4& aiMatrix)
{
    CMatrix matrix;
    memcpy(&matrix, &aiMatrix.a1, sizeof(float) * 16);
    return matrix;
}

aiAABB CLuaAssetNode::GetBoundingBox()
{
    const aiScene* pScene = m_pAssetModel->GetScene();
    aiAABB         NodeAABB;
    aiAABB         AABB;
    for (int i = 0; i < m_pNode->mNumMeshes; i++)
    {
        // something wrong TODO
        AABB = pScene->mMeshes[m_pNode->mMeshes[i]]->mAABB;
        NodeAABB.mMin.x = std::min(NodeAABB.mMin.x, AABB.mMin.x);
        NodeAABB.mMin.y = std::min(NodeAABB.mMin.y, AABB.mMin.y);
        NodeAABB.mMin.z = std::min(NodeAABB.mMin.z, AABB.mMin.z);
        NodeAABB.mMax.x = std::max(NodeAABB.mMax.x, AABB.mMax.x);
        NodeAABB.mMax.y = std::max(NodeAABB.mMax.y, AABB.mMax.y);
        NodeAABB.mMax.z = std::max(NodeAABB.mMax.z, AABB.mMax.z);
    }
    return NodeAABB;
}

std::tuple<CVector, CVector> CLuaAssetNode::GetCVectorBoundingBox()
{
    const aiScene* pScene = m_pAssetModel->GetScene();
    aiAABB         NodeAABB;
    CVector        min;
    CVector        max;
    for (int i = 0; i < m_pNode->mNumMeshes; i++)
    {
        // something wrong TODO
        NodeAABB = pScene->mMeshes[m_pNode->mMeshes[i]]->mAABB;
        min.fX = std::min(NodeAABB.mMin.x, min.fX);
        min.fY = std::min(NodeAABB.mMin.y, min.fY);
        min.fZ = std::min(NodeAABB.mMin.z, min.fZ);
        max.fX = std::max(NodeAABB.mMax.x, max.fX);
        max.fY = std::max(NodeAABB.mMax.y, max.fY);
        max.fZ = std::max(NodeAABB.mMax.z, max.fZ);
    }
    return {min, max};
}

std::vector<CLuaAssetNode*> CLuaAssetNode::GetChildNodes()
{
    std::vector<CLuaAssetNode*> vecChildNodes;

    for (int i = 0; i < m_pNode->mNumChildren; i++)
    {
        vecChildNodes.push_back(m_pAssetModel->GetNode(m_pNode->mChildren[i]));
    }

    return vecChildNodes;
}

void CLuaAssetNode::GetMetaData(lua_State* luaVM)
{
    lua_newtable(luaVM);
    if (m_pNode->mMetaData == nullptr)
    {
        return;
    }
    for (int i = 0; i < m_pNode->mMetaData->mNumProperties; i++)
    {
        aiString*        pKeyName = &m_pNode->mMetaData->mKeys[i];
        aiMetadataEntry* pValue = &m_pNode->mMetaData->mValues[i];
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
                m_pNode->mMetaData->Get<bool>(*pKeyName, boolValue);
                lua_pushboolean(luaVM, boolValue);
                break;
            case AI_INT32:
                m_pNode->mMetaData->Get<int>(*pKeyName, intValue);
                lua_pushnumber(luaVM, intValue);
                break;
            case AI_UINT64:
                m_pNode->mMetaData->Get<uint64_t>(*pKeyName, longValue);
                lua_pushnumber(luaVM, longValue);
                break;
            case AI_FLOAT:
                m_pNode->mMetaData->Get<float>(*pKeyName, floatValue);
                lua_pushnumber(luaVM, floatValue);
                break;
            case AI_DOUBLE:
                m_pNode->mMetaData->Get<double>(*pKeyName, doubleValue);
                lua_pushnumber(luaVM, doubleValue);
                break;
            case AI_AISTRING:
                m_pNode->mMetaData->Get<aiString>(*pKeyName, stringValue);
                lua_pushstring(luaVM, stringValue.C_Str());
                break;
            case AI_AIVECTOR3D:
                m_pNode->mMetaData->Get<aiVector3D>(*pKeyName, vector3Value);
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

CVector CLuaAssetNode::GetPosition() const
{
    return convertAiMatrixToCMatrix(m_pNode->mTransformation).GetPosition();
}
CVector CLuaAssetNode::GetRotation() const
{
    return convertAiMatrixToCMatrix(m_pNode->mTransformation).GetRotation();
}
CVector CLuaAssetNode::GetScale() const
{
    return convertAiMatrixToCMatrix(m_pNode->mTransformation).GetScale();
}

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

void CLuaAssetNode::AddToRenderQueue(SRenderingSettings& settings)
{
    // m_vecRender.push_back(std::move(settings));
    g_pCore->GetGraphics()->DrawAssetNode(settings);
}

CMaterialItem* pLastMaterial;

CClientMeshBuffer* CLuaAssetNode::GetMeshBuffer(int idx)
{
    return  m_pAssetModel->GetMeshBuffer(m_pNode->mMeshes[idx]);
}

CMaterialItem* CLuaAssetNode::GetTexture(int idx)
{
    return m_pAssetModel->GetTexture(idx).pClientTexture->GetMaterialItem();
}

size_t CLuaAssetNode::GetMeshNum()
{
    return m_pNode->mNumMeshes;
}

//void CLuaAssetNode::Render(SRenderingSettings& settings)
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
    float*  pBuffer = (float*)malloc(sizeof(float) * 16);
    matrix.GetBuffer(pBuffer);
    memcpy(pBuffer, &aiMatrix.a1, sizeof(float) * 16);
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

std::vector<CLuaAssetNode*> CLuaAssetNode::GetChildNodes()
{
    std::vector<CLuaAssetNode*> vecChildNodes;

    for (int i = 0; i < m_pNode->mNumChildren; i++)
    {
        vecChildNodes.push_back(m_pAssetModel->GetNode(m_pNode->mChildren[i]));
    }

    return vecChildNodes;
}

int CLuaAssetNode::GetProperties(lua_State* luaVM, eAssetProperty assetProperty)
{
    CVector vector;
    aiAABB  AABB;
    switch (assetProperty)
    {
        case ASSET_NAME:
            lua_pushstring(luaVM, m_pNode->mName.C_Str());
            return 1;
        case ASSET_POSITION:
            vector = convertAiMatrixToCMatrix(m_pNode->mTransformation).GetPosition();
            lua_pushnumber(luaVM, vector.fX);
            lua_pushnumber(luaVM, vector.fY);
            lua_pushnumber(luaVM, vector.fZ);
            return 3;
        case ASSET_ROTATION:
            vector = convertAiMatrixToCMatrix(m_pNode->mTransformation).GetRotation();
            lua_pushnumber(luaVM, vector.fX);
            lua_pushnumber(luaVM, vector.fY);
            lua_pushnumber(luaVM, vector.fZ);
            return 3;
        case ASSET_SCALE:
            vector = convertAiMatrixToCMatrix(m_pNode->mTransformation).GetScale();
            lua_pushnumber(luaVM, vector.fX);
            lua_pushnumber(luaVM, vector.fY);
            lua_pushnumber(luaVM, vector.fZ);
            return 3;
        case ASSET_MESHES_COUNT:
            lua_pushnumber(luaVM, m_pNode->mNumMeshes);
            return 1;
        case ASSET_CHILD_NODES_COUNT:
            lua_pushnumber(luaVM, m_pNode->mNumChildren);
            return 1;
        case ASSET_PARENT_NODE:
            if (m_pNode->mParent != nullptr)
            {
                lua_pushassetnode(luaVM, m_pAssetModel->GetNode(m_pNode->mParent));
            }
            else
            {
                lua_pushboolean(luaVM, false);
            }
            return 1;
        case ASSET_BOUNDING_BOX:
            AABB = GetBoundingBox();
            lua_pushnumber(luaVM, AABB.mMin.x);
            lua_pushnumber(luaVM, AABB.mMin.y);
            lua_pushnumber(luaVM, AABB.mMin.z);
            lua_pushnumber(luaVM, AABB.mMax.x);
            lua_pushnumber(luaVM, AABB.mMax.y);
            lua_pushnumber(luaVM, AABB.mMax.z);
            return 6;
        default:
            lua_pushboolean(luaVM, false);
            return 1;
    }
}

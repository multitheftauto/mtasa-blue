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
#include "CClientMultiMaterialMeshBuffer.h"

CLuaAssetNode::CLuaAssetNode(CClientAssetModel* pAssetModel, const aiNode* pNode)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ASSETNODE);
    m_pAssetModel = pAssetModel;
    m_pNode = pNode;
}

void CLuaAssetNode::Cache()
{
    m_vecMeshes = m_pAssetModel->GetMeshesOfNode(this);
    CacheMetadata();
    CreateMeshBuffer();
}

CLuaAssetNode::~CLuaAssetNode()
{
    RemoveScriptID();
}

void CLuaAssetNode::CreateMeshBuffer()
{
    if (m_vecMeshes.size() == 0)
        return;
    /*
        std::vector<int> indices;
    aiFace*          face;
    for (int i = 0; i < m_pMesh->mNumFaces; i++)
    {
        face = &m_pMesh->mFaces[i];
        for (int idx = 0; idx < face->mNumIndices; idx++)
        {
            indices.push_back(face->mIndices[idx]);
        }
    }

    m_pMeshBuffer = std::make_unique<CClientMeshBuffer>();
    m_pMeshBuffer->AddVertexBuffer<CVector>(&m_pMesh->mVertices[0].x, m_pMesh->mNumVertices, ePrimitiveData::PRIMITIVE_DATA_XYZ);
    if (m_pMesh->GetNumUVChannels() > 0 && m_pMesh->HasTextureCoords(0))
    {
        m_pMeshBuffer->AddVertexBuffer<CVector>(m_pMesh->mTextureCoords[0], m_pMesh->mNumVertices, ePrimitiveData::PRIMITIVE_DATA_UV);
        m_pMeshBuffer->m_uiMaterialIndex = m_pMesh->mMaterialIndex;
    }
    else
    {
        m_pMeshBuffer->m_uiMaterialIndex = -1;
    }
    m_pMeshBuffer->Finalize();
    m_pMeshBuffer->CreateIndexBuffer<int>(indices);
    m_pMeshBuffer->m_iFaceCount = m_pMesh->mNumFaces;
    m_pMeshBuffer->m_iVertexCount = m_pMesh->mNumVertices;
    */

    unsigned int uiVertexCount = 0;
    unsigned int uiFaceCount = 0;
    unsigned int uiIndexCount = 0;
    for (auto const& item : m_vecMeshes)
    {
        const aiMesh* pMesh = item->GetMesh();
        uiVertexCount += pMesh->mNumVertices;
        uiFaceCount += pMesh->mNumFaces;
    }

    std::vector<CVector> vecVertices;
    std::vector<CVector> vecTexCoords;
    std::vector<int>     indices;
    std::vector<int>     vecTexturesOffsets;
    vecVertices.reserve(uiVertexCount);
    vecTexturesOffsets.reserve(uiVertexCount);

    int           currentVertexOffset = 0;
    const aiFace* face;
    for (auto const& item : m_vecMeshes)
    {
        const aiMesh* pMesh = item->GetMesh();
        unsigned int  uiTempIndexCount = 0;
        vecTexturesOffsets.emplace_back(pMesh->mNumVertices + currentVertexOffset);

        for (int i = 0; i < pMesh->mNumVertices; i++)
        {
            auto vertex = pMesh->mVertices[i];
            vecVertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        for (int i = 0; i < pMesh->mNumFaces; i++)
        {
            face = &pMesh->mFaces[i];
            uiTempIndexCount += face->mNumIndices;
            for (int idx = 0; idx < face->mNumIndices; idx++)
            {
                indices.emplace_back(face->mIndices[idx] + currentVertexOffset);
            }
        }

        if (pMesh->GetNumUVChannels() > 0 && pMesh->HasTextureCoords(0))
        {
            for (int i = 0; i < pMesh->mNumVertices; i++)
            {
                auto vertex = pMesh->mTextureCoords[0][i];
                vecTexCoords.emplace_back(vertex.x, vertex.y, vertex.z);
            }
        }
        else
        {
            for (int i = 0; i < pMesh->mNumVertices; i++)
            {
                vecTexCoords.emplace_back(0, 0, 0);
            }
        }

        uiIndexCount += uiTempIndexCount;
        currentVertexOffset += pMesh->mNumVertices;
    }

    m_pMultimaterialMeshBuffer = std::make_unique<CClientMultiMaterialMeshBuffer>();
    m_pMultimaterialMeshBuffer->CreateIndexBuffer(indices, vecTexturesOffsets);
    m_pMultimaterialMeshBuffer->AddVertexBuffer(&vecVertices[0].fX, vecVertices.size(), sizeof(CVector), ePrimitiveData::PRIMITIVE_DATA_XYZ);
    m_pMultimaterialMeshBuffer->AddVertexBuffer(&vecTexCoords[0].fX, vecVertices.size(), sizeof(CVector), ePrimitiveData::PRIMITIVE_DATA_UV);
    m_pMultimaterialMeshBuffer->m_iFaceCount = uiFaceCount;
    m_pMultimaterialMeshBuffer->m_iVertexCount = vecVertices.size();
    m_pMultimaterialMeshBuffer->Finalize();
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

CClientMultiMaterialMeshBuffer* CLuaAssetNode::GetMeshBuffer() const
{
    return m_pMultimaterialMeshBuffer.get();
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
    aiAABB bbox = GetBoundingBox();
    return {CVector(bbox.mMin.x, bbox.mMin.y, bbox.mMin.z), CVector(bbox.mMax.x, bbox.mMax.y, bbox.mMax.z)};
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

void CLuaAssetNode::CacheMetadata()
{
    if (m_pNode->mMetaData == nullptr)
        return;

    for (int i = 0; i < m_pNode->mMetaData->mNumProperties; i++)
    {
        aiString*        pKeyName = &m_pNode->mMetaData->mKeys[i];
        aiMetadataEntry* pValue = &m_pNode->mMetaData->mValues[i];
        std::string      strKey(pKeyName->C_Str());
        bool             boolValue;
        float            floatValue;
        uint64_t         longValue;
        double           doubleValue;
        aiString         stringValue;
        int              intValue;
        aiVector3D       vector3Value;
        switch (pValue->mType)
        {
            case AI_BOOL:
                m_pNode->mMetaData->Get<bool>(*pKeyName, boolValue);
                m_mapMetadataBool[strKey] = boolValue;
                break;
            case AI_INT32:
                m_pNode->mMetaData->Get<int>(*pKeyName, intValue);
                m_mapMetadataInt[strKey] = intValue;
                break;
            case AI_UINT64:
                m_pNode->mMetaData->Get<uint64_t>(*pKeyName, longValue);
                m_mapMetadataInt64[strKey] = longValue;
                break;
            case AI_FLOAT:
                m_pNode->mMetaData->Get<float>(*pKeyName, floatValue);
                m_mapMetadataFloat[strKey] = floatValue;
                break;
            case AI_DOUBLE:
                m_pNode->mMetaData->Get<double>(*pKeyName, doubleValue);
                m_mapMetadataDouble[strKey] = doubleValue;
                break;
            case AI_AISTRING:
                m_pNode->mMetaData->Get<aiString>(*pKeyName, stringValue);
                m_mapMetadataString[strKey] = std::string(stringValue.C_Str());
                break;
            case AI_AIVECTOR3D:
                m_pNode->mMetaData->Get<aiVector3D>(*pKeyName, vector3Value);
                m_mapMetadataCVector[strKey] = CVector(vector3Value.x, vector3Value.y, vector3Value.z);
                break;
        }
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

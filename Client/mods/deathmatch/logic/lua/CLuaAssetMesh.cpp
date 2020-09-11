/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetMesh.cpp
 *  PURPOSE:     Lua asset mesh class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>
#include "CClientMeshBuffer.h"
using namespace Assimp;

#include "StdInc.h"
#include "CLuaAssetMesh.h"

CLuaAssetMesh::CLuaAssetMesh(CClientAssetModel* pAssetModel, const aiMesh* pMesh, std::shared_ptr<CLuaAssetNode> pNode)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ASSETMESH);
    m_pAssetModel = pAssetModel;
    m_pMesh = pMesh;
    m_pNode = pNode;

    std::vector<int> indices;
    aiFace*          face;
    for (int i = 0; i < m_pMesh->mNumFaces; i++)
    {
        face = &m_pMesh->mFaces[i];
        for (int idx = 0; idx < face->mNumIndices; idx++)
        {
            indices.emplace_back(face->mIndices[idx]);
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
}

CLuaAssetMesh::~CLuaAssetMesh()
{
    RemoveScriptID();
}

void CLuaAssetMesh::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::ASSETMESH, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

CLuaAssetMesh* CLuaAssetMesh::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaAssetMesh* pLuaMesh = (CLuaAssetMesh*)CIdArray::FindEntry(uiScriptID, EIdClass::ASSETMESH);
    return pLuaMesh;
}

unsigned int CLuaAssetMesh::GetVerticesCount() const
{
    return m_pMesh->mNumVertices;
}
unsigned int CLuaAssetMesh::GetFacesCount() const
{
    return m_pMesh->mNumFaces;
}
void CLuaAssetMesh::GetUVComponentsCount(const unsigned int* pComponents) const
{
    pComponents = &m_pMesh->mNumUVComponents[0];
}
unsigned int CLuaAssetMesh::GetUVChannels() const
{
    return m_pMesh->GetNumUVChannels();
}
unsigned int CLuaAssetMesh::GetColorChannelsCount() const
{
    return m_pMesh->GetNumColorChannels();
}
unsigned int CLuaAssetMesh::GetBonesCount() const
{
    return m_pMesh->mNumBones;
}
std::tuple<CVector, CVector> CLuaAssetMesh::GetBoundingBox() const
{
    return {CVector(m_pMesh->mAABB.mMin.x, m_pMesh->mAABB.mMin.y, m_pMesh->mAABB.mMin.z),
            CVector(m_pMesh->mAABB.mMax.x, m_pMesh->mAABB.mMax.y, m_pMesh->mAABB.mMax.z)};
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetMesh.cpp
 *  PURPOSE:     Lua timer class
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


CLuaAssetMesh::CLuaAssetMesh(CClientAssetModel* pAssetModel, const aiMesh* pMesh)
    {
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ASSETMESH);
    m_pAssetModel = pAssetModel;
    m_pMesh = pMesh;
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

    m_pMeshBuffer = new CClientMeshBuffer();
    m_pMeshBuffer->AddVertexBuffer<CVector>(&m_pMesh->mVertices[0].x, m_pMesh->mNumVertices, ePrimitiveData::PRIMITIVE_DATA_XYZ);
    if (m_pMesh->HasTextureCoords(0))
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
    delete m_pMeshBuffer;
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

int CLuaAssetMesh::GetProperties(lua_State* luaVM, eAssetProperty assetProperty)
{
    switch (assetProperty)
    {
        case ASSET_VERTICES_COUNT:
            lua_pushnumber(luaVM, m_pMesh->mNumVertices);
            return 1;
        case ASSET_FACES_COUNT:
            lua_pushnumber(luaVM, m_pMesh->mNumFaces);
            return 1;
        case ASSET_UV_COMPONENTS_COUNT:
            lua_newtable(luaVM);
            for (int i = 0; i < 8; i++)
            {
                lua_pushnumber(luaVM, i + 1);
                lua_pushnumber(luaVM, m_pMesh->mNumUVComponents[i]);
                lua_settable(luaVM, -3);
            }
            return 1;
        case ASSET_UV_CHANNELS:
            lua_pushnumber(luaVM, m_pMesh->GetNumUVChannels());
            return 1;
        case ASSET_COLOR_CHANNELS:
            lua_pushnumber(luaVM, m_pMesh->GetNumColorChannels());
            return 1;
        case ASSET_BONES_COUNT:
            lua_pushnumber(luaVM, m_pMesh->mNumBones);
            return 1;
        case ASSET_BOUNDING_BOX:
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMin.x);
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMin.y);
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMin.z);
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMax.x);
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMax.y);
            lua_pushnumber(luaVM, m_pMesh->mAABB.mMax.z);
            return 6;
        default:
            lua_pushboolean(luaVM, false);
            return 1;
    }
}

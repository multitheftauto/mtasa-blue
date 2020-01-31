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
using namespace Assimp;

#include "StdInc.h"
#include "CLuaAssetMesh.h"

CLuaAssetMesh::CLuaAssetMesh(CClientAssetModel* pAssetModel, const aiMesh* pMesh)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ASSETMESH);
    m_pAssetModel = pAssetModel;
    m_pMesh = pMesh;
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

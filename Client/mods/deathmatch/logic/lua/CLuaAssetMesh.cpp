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
        //case ASSET_NAME:
        //    lua_pushstring(luaVM, m_pNode->mName.C_Str());
        //    break;
        default:
            lua_pushboolean(luaVM, false);
    }
    return 1;
}

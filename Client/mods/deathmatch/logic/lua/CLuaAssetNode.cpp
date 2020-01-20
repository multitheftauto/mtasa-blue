/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetNode.cpp
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
#include "CLuaAssetNode.h"

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

CLuaAssetNode* CLuaAssetNode::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaAssetNode* pLuaNode = (CLuaAssetNode*)CIdArray::FindEntry(uiScriptID, EIdClass::ASSETNODE);
    return pLuaNode;
}

void CLuaAssetNode::GetProperties(lua_State* luaVM, eAssetProperty assetProperty)
{
    switch (assetProperty)
    {
        case ASSET_NAME:
            lua_pushstring(luaVM, m_pNode->mName.C_Str());
            break;
        default:
            lua_pushboolean(luaVM, false);
    }
}

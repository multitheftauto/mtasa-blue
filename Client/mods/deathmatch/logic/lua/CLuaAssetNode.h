/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetNode.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaAssetNode;

#pragma once
class aiNode;
class CClientAssetModel;
// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaAssetNode
{
public:
    CLuaAssetNode(CClientAssetModel* pAssetModel, const aiNode* pNode);
    ~CLuaAssetNode();

    void RemoveScriptID();

    uint                 GetScriptID() const { return m_uiScriptID; }

    static CLuaAssetNode* GetFromScriptID(unsigned int uiScriptID);
    int                  GetProperties(lua_State* luaVM, eAssetProperty assetProperty);
    aiAABB                GetBoundingBox();
    const aiNode*         GetNode() const { return m_pNode; }
    std::vector<CLuaAssetNode*> GetChildNodes();
private:
    uint               m_uiScriptID;
    CClientAssetModel* m_pAssetModel;
    const aiNode*      m_pNode;
};

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

class CLuaAssetNode : public CLuaAssetNodeInterface
{
public:
    CLuaAssetNode(CClientAssetModel* pAssetModel, const aiNode* pNode);
    ~CLuaAssetNode();

    void RemoveScriptID();

    uint                 GetScriptID() const { return m_uiScriptID; }

    static CLuaAssetNode*       GetFromScriptID(unsigned int uiScriptID);
    int                         GetProperties(lua_State* luaVM, eAssetProperty assetProperty);
    void                        GetMetaData(lua_State* luaVM);
    aiAABB                      GetBoundingBox();
    const aiNode*               GetNode() const { return m_pNode; }
    void                        AddToRenderQueue(SRenderingSettings& settings);
    std::vector<CLuaAssetNode*> GetChildNodes();
    CClientMeshBuffer*          GetMeshBuffer(int idx);
    CMaterialItem*              GetTexture(int idx);
    size_t                      GetMeshNum();

private:
    uint               m_uiScriptID;
    CClientAssetModel* m_pAssetModel;
    const aiNode*      m_pNode;
};

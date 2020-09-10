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
    void                        GetMetaData(lua_State* luaVM);
    aiAABB                      GetBoundingBox();
    std::tuple<CVector, CVector> GetCVectorBoundingBox();

    const aiNode*               GetNode() const { return m_pNode; }
    CClientAssetModel*          GetAssetModel() const { return m_pAssetModel; }
    std::vector<std::shared_ptr<CLuaAssetMesh>> GetMeshes() const { return m_vecMeshes; }
    void                        AddToRenderQueue(std::unique_ptr<SRenderAssetItem> settings);
    std::vector<CLuaAssetNode*> GetChildNodes();
    CClientMeshBuffer*          GetMeshBuffer(int idx);
    CMaterialItem*              GetTexture(int idx);
    size_t                      GetMeshNum();

    const char* GetName() const { return m_pNode->mName.C_Str(); }
    CVector          GetPosition() const;
    CVector          GetRotation() const;
    CVector          GetScale() const;
    unsigned int     GetMeshesCount() const { return m_pNode->mNumMeshes; }
    unsigned int GetChildrenCount() const { return m_pNode->mNumChildren; }
    CLuaAssetNode*   GetParentNode() const { return (m_pNode->mParent == nullptr) ? nullptr : m_pAssetModel->GetNode(m_pNode->mParent); }

private:
    uint               m_uiScriptID;
    CClientAssetModel* m_pAssetModel;
    const aiNode*      m_pNode;
    std::vector<std::shared_ptr<CLuaAssetMesh>> m_vecMeshes;
};

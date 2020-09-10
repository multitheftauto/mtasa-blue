/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetMesh.h
 *  PURPOSE:     Lua asset mesh class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaAssetMesh;
class CClientAssetModel;
class aiMesh;

#pragma once
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaAssetMesh
{
public:
    CLuaAssetMesh(CClientAssetModel* pAssetModel, const aiMesh* pMesh, std::shared_ptr<CLuaAssetNode> pNode);
    ~CLuaAssetMesh();

    void RemoveScriptID();

    uint                  GetScriptID() const { return m_uiScriptID; }
    CClientMeshBuffer*    GetMeshBuffer() const { return m_pMeshBuffer.get(); }
    static CLuaAssetMesh* GetFromScriptID(unsigned int uiScriptID);
    std::shared_ptr<CLuaAssetNode> GetNode() const { return m_pNode; };

    unsigned int                 GetVerticesCount() const;
    unsigned int                 GetFacesCount() const;
    void                         GetUVComponentsCount(const unsigned int* pComponents) const;
    unsigned int                 GetUVChannels() const;
    unsigned int                 GetColorChannelsCount() const;
    unsigned int                 GetBonesCount() const;
    std::tuple<CVector, CVector> GetBoundingBox() const;
private:
    CClientAssetModel* m_pAssetModel;
    const aiMesh* m_pMesh;
    uint m_uiScriptID;
    std::shared_ptr<CLuaAssetNode>      m_pNode;
    std::unique_ptr<CClientMeshBuffer> m_pMeshBuffer;
};

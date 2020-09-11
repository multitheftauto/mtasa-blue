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
class aiNode;
class CClientAssetModel;
class CClientMultiMaterialMeshBuffer;

#pragma once
// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaAssetNode : public CLuaAssetNodeInterface
{
public:
    CLuaAssetNode(CClientAssetModel* pAssetModel, const aiNode* pNode);
    ~CLuaAssetNode();

    void RemoveScriptID();
    void CreateMeshBuffer();

    uint GetScriptID() const { return m_uiScriptID; }
    void                         Cache();
    static CLuaAssetNode*        GetFromScriptID(unsigned int uiScriptID);
    void                         CacheMetadata();
    aiAABB                       GetBoundingBox();
    std::tuple<CVector, CVector> GetCVectorBoundingBox();

    const aiNode*                               GetNode() const { return m_pNode; }
    CClientAssetModel*                          GetAssetModel() const { return m_pAssetModel; }
    std::vector<std::shared_ptr<CLuaAssetMesh>> GetMeshes() const { return m_vecMeshes; }
    void                                        AddToRenderQueue(std::unique_ptr<SRenderAssetItem> settings);
    std::vector<CLuaAssetNode*>                 GetChildNodes();
    CClientMeshBuffer*                          GetMeshBuffer(int idx);
    CMaterialItem*                              GetTexture(int idx);
    size_t                                      GetMeshNum();

    const char*    GetName() const { return m_pNode->mName.C_Str(); }
    CVector        GetPosition() const;
    CVector        GetRotation() const;
    CVector        GetScale() const;
    unsigned int   GetMeshesCount() const { return m_pNode->mNumMeshes; }
    unsigned int   GetChildrenCount() const { return m_pNode->mNumChildren; }
    CLuaAssetNode* GetParentNode() const { return (m_pNode->mParent == nullptr) ? nullptr : m_pAssetModel->GetNode(m_pNode->mParent); }

    std::unordered_map<std::string, bool>        GetBoolMetdata() const { return m_mapMetadataBool; }
    std::unordered_map<std::string, int>         GetIntMetdata() const { return m_mapMetadataInt; }
    std::unordered_map<std::string, uint64_t>    GetInt64Metdata() const { return m_mapMetadataInt64; }
    std::unordered_map<std::string, float>       GetFloatMetdata() const { return m_mapMetadataFloat; }
    std::unordered_map<std::string, double>      GetDoubleMetdata() const { return m_mapMetadataDouble; }
    std::unordered_map<std::string, std::string> GetStringMetdata() const { return m_mapMetadataString; }
    std::unordered_map<std::string, CVector>     GetVectorMetdata() const { return m_mapMetadataCVector; }

private:
    uint                                            m_uiScriptID;
    CClientAssetModel*                              m_pAssetModel;
    const aiNode*                                   m_pNode;
    std::unique_ptr<CClientMultiMaterialMeshBuffer> m_pMultimaterialMeshBuffer;
    std::vector<std::shared_ptr<CLuaAssetMesh>>     m_vecMeshes;

    std::unordered_map<std::string, bool>        m_mapMetadataBool;
    std::unordered_map<std::string, int>         m_mapMetadataInt;
    std::unordered_map<std::string, uint64_t>    m_mapMetadataInt64;
    std::unordered_map<std::string, float>       m_mapMetadataFloat;
    std::unordered_map<std::string, double>      m_mapMetadataDouble;
    std::unordered_map<std::string, std::string> m_mapMetadataString;
    std::unordered_map<std::string, CVector>     m_mapMetadataCVector;
};

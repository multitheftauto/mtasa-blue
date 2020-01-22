/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModel.h
 *  PURPOSE:     PointLights entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>
using namespace Assimp;

class CClientAssetModelManager;

class CClientAssetModel : public CClientEntity
{
    DECLARE_CLASS(CClientAssetModel, CClientEntity)

public:
    CClientAssetModel(class CClientManager* pManager, ElementID ID);
    ~CClientAssetModel();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTASSETMODEL; };

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    void DoPulse();

    bool LoadFromFile(std::string strPath);
    void GetProperties(lua_State* luaVM, eAssetProperty assetProperty);

    void GetLuaNode(lua_State* luaVM, const aiNode* pNode);

protected:
    void CacheNodes(const aiNode* pNode);

    CClientAssetModelManager* m_pAssetModelManager;

    bool                       bLoaded = false;
    Assimp::Importer importer;
    CVector        m_vecPosition;
    std::vector<const aiNode*> vecNodes;
    const aiScene* m_pScene;
};

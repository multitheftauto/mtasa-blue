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
#include <assimp/include/assimp/ProgressHandler.hpp>

using namespace Assimp;

class CClientAssetModelManager;
class CLuaAssetNode;

class AssetProgressHandler : public Assimp::ProgressHandler
{
public:
    std::atomic<float> fProgressProcentage = 0;
    bool Update(float percentage = -1.f)
    {
        fProgressProcentage = percentage;
        return true;
    }
};

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

    const char* LoadFromFile(std::string strPath);
    int GetProperties(lua_State* luaVM, eAssetProperty assetProperty);
    int GetLoadingProgress(lua_State* luaVM);

    CLuaAssetNode* GetNode(const aiNode* pNode = nullptr);

    Assimp::Importer& GetImporter() { return importer; }
    const aiScene*     GetScene() { return m_pScene; }
    bool              IsLoaded() { return m_bModelLoaded || !m_progressHandler; }

protected:
    void CacheNodes(const aiNode* pNode);

    CClientAssetModelManager* m_pAssetModelManager;

    Assimp::Importer importer;
    CVector        m_vecPosition;
    std::vector<const aiNode*> vecNodes;
    const aiScene*             m_pScene;
    std::vector<CLuaAssetNode*> m_vecAssetRootNode;
    AssetProgressHandler*        m_progressHandler;
    std::atomic<bool>                     m_bModelLoaded = false;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModel.h
 *  PURPOSE:     ASset model class header
 *
 *****************************************************************************/

#pragma once

#include "CClientMeshBuffer.h"
#include "CClientEntity.h"
#include "..\Client\mods\deathmatch\logic\lua\CLuaAssetMesh.h"
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>
#include "assimp/ProgressHandler.hpp"

using namespace Assimp;

class CClientAssetModelManager;
class CLuaAssetNode;
class CLuaAssetMesh;
class CClientMaterial;
class CClientMeshBuffer;

struct SRenderingSettings
{
    CMatrix                 matrix;
    CLuaAssetNodeInterface* assetNode;
};

struct SAssetTexture
{
    const char*     fileName;
    CClientTexture* pClientTexture;
    CClientMaterial* pMaterialElement = nullptr;
    SAssetTexture(const char* name, CClientTexture* pTexture)
    {
        fileName = name;
        pClientTexture = pTexture;
    }
};

class CAssetProgressHandler : public ProgressHandler
{
public:
    bool bLoad = true;
    CAssetProgressHandler() {}
    ~CAssetProgressHandler() {}

private:
    bool Update(float percentage = -1.f) { return bLoad; }
    void UpdateFileRead(int currentStep, int numberOfSteps) {}
    void UpdatePostProcess(int currentStep, int numberOfSteps) {}
    void UpdateFileWrite(int currentStep, int numberOfSteps) {}
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
    void GetMeshes(lua_State* luaVM);
    void GetTextures(lua_State* luaVM);

    const char* LoadFromFile(std::string strPath);
    const char* LoadFromRawData(const SString& strPath, const SString& strHint);
    int         GetProperties(lua_State* luaVM, eAssetProperty assetProperty);

    CLuaAssetNode* GetNode(const aiNode* pNode = nullptr);

    Assimp::Importer& GetImporter() { return importer; }
    const aiScene*    GetScene() { return m_pScene; }
    bool              IsLoaded() { return m_bModelLoaded; }

    void           CacheTextures(CResource* pParentResource);
    void           Cache();
    CClientMeshBuffer* GetMeshBuffer(int idx) { return m_vecAssetMeshes[idx]->GetMeshBuffer(); }
    size_t GetMeshNum() { return m_vecAssetMeshes.size(); }
    SAssetTexture& GetTexture(int idx) { return m_vecAssetTextures[idx]; }
    bool               SetTexture(int idx, CClientMaterial* pMaterial);

protected:
    void CacheNodes(const aiNode* pNode);
    void CacheMeshes();

    CClientAssetModelManager* m_pAssetModelManager;

    unsigned int                      m_uiImportFlags;
    CAssetProgressHandler*            m_pProgressHandler;
    Assimp::Importer                  importer;
    CVector                           m_vecPosition;
    std::vector<const aiNode*>        vecNodes;
    const aiScene*                    m_pScene;
    std::vector<CLuaAssetNode*>       m_vecAssetNodes;
    std::vector<CLuaAssetMesh*>       m_vecAssetMeshes;
    std::vector<SAssetTexture>        m_vecAssetTextures;
    std::atomic<bool>                 m_bModelLoaded = false;
};

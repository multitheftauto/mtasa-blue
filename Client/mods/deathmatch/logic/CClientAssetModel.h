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
#include "assimp/IOSystem.hpp"

using namespace Assimp;

class CClientAssetModelManager;
class CLuaAssetNode;
class CLuaAssetMesh;
class CClientMaterial;
class CClientMeshBuffer;

struct SRenderAssetItem
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

class CAssetIOHandler : public IOSystem
{
public:
    CAssetIOHandler() {}
    ~CAssetIOHandler() {}
    bool Exists(const std::string& pFile) const { return false; }
    bool Exists(const char* pFile) const { return false; }
    char getOsSeparator() const { return '.'; }
    IOStream*          Open(const char* pFile, const char* pMode = "rb") { return nullptr; }
    IOStream*          Open(const std::string& pFile, const std::string& pMode = std::string("rb")) { return nullptr; }
    void               Close(IOStream* pFile) {}
    bool               ComparePaths(const char* one, const char* second) const { return true; }
    bool               PushDirectory(const std::string& path) { return true; }
    bool               ComparePaths(const std::string& one, const std::string& second) const { return false; }
    const std::string& CurrentDirectory() const { return ""; }
    size_t             StackSize() const { return 0; }
    bool               PopDirectory() { return true; }
    bool               CreateDirectory(const std::string& path) { return true; }
    bool               ChangeDirectory(const std::string& path) { return true; }
    bool               DeleteFile(const std::string& file) { return true; }
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
    std::vector<std::shared_ptr<CLuaAssetNode>> GetNodes() const { return m_vecAssetNodes; }
    std::vector<std::shared_ptr<CLuaAssetMesh>> GetMeshes() const { return m_vecAssetMeshes; }
    std::vector<SAssetTexture> GetTextures() const { return m_vecAssetTextures; }

    //void GetTextures(lua_State* luaVM);

    const char* LoadFromFile(std::string strPath);
    const char* LoadFromRawData(const SString& strPath, const SString& strHint);

    CLuaAssetNode* GetNode(const aiNode* pNode = nullptr);

    Assimp::Importer& GetImporter() { return importer; }
    const aiScene*    GetScene() { return m_pScene; }
    bool              IsLoaded() { return m_bModelLoaded; }

    void           CacheTextures(CResource* pParentResource);
    void           Cache();
    void               GetMaterialProperties(lua_State* luaVM, int iMaterialIndex);
    void                                        CacheMetadata();
    CClientMeshBuffer* GetMeshBuffer(int idx) { return m_vecAssetMeshes[idx]->GetMeshBuffer(); }
    size_t GetMeshNum() { return m_vecAssetMeshes.size(); }
    SAssetTexture*     GetTexture(int idx);
    bool                                        SetTexture(int idx, CClientMaterial* pMaterial);
    std::vector<std::shared_ptr<CLuaAssetMesh>> GetMeshesOfNode(CLuaAssetNode* pNode);


    unsigned int GetAnimationsCount() const { return m_pScene->mNumAnimations; } 
    unsigned int GetCamerasCount() const { return m_pScene->mNumCameras; } 
    unsigned int GetLightsCount() const { return m_pScene->mNumLights; } 
    unsigned int GetMaterialsCount() const { return m_pScene->mNumMaterials; } 
    unsigned int GetMeshesCount() const { return m_pScene->mNumMeshes; } 
    unsigned int GetTexturesCount() const { return m_pScene->mNumTextures; } 
    unsigned int GetNodesCount() const { return vecNodes.size(); }

    std::unordered_map<std::string, bool>        GetBoolMetdata() const { return m_mapMetadataBool; }
    std::unordered_map<std::string, int>         GetIntMetdata() const { return m_mapMetadataInt; }
    std::unordered_map<std::string, uint64_t>    GetInt64Metdata() const { return m_mapMetadataInt64; }
    std::unordered_map<std::string, float>       GetFloatMetdata() const { return m_mapMetadataFloat; }
    std::unordered_map<std::string, double>      GetDoubleMetdata() const { return m_mapMetadataDouble; }
    std::unordered_map<std::string, std::string> GetStringMetdata() const { return m_mapMetadataString; }
    std::unordered_map<std::string, CVector>     GetVectorMetdata() const { return m_mapMetadataCVector; }

protected:
    void CacheNodesAndMeshes(const aiNode* pNode);

    CClientAssetModelManager* m_pAssetModelManager;

    unsigned int                      m_uiImportFlags;
    std::unique_ptr<CAssetProgressHandler>            m_pProgressHandler;
    std::unique_ptr<CAssetIOHandler>            m_pIOHandler;
    Assimp::Importer                  importer;
    CVector                           m_vecPosition;
    std::vector<const aiNode*>        vecNodes;
    const aiScene*                    m_pScene; // importer take care about freeing
    std::vector<std::shared_ptr<CLuaAssetNode>>       m_vecAssetNodes;
    std::vector<std::shared_ptr<CLuaAssetMesh>>       m_vecAssetMeshes;
    std::vector<SAssetTexture>        m_vecAssetTextures;
    std::atomic<bool>                 m_bModelLoaded = false;

    std::unordered_map<std::string, bool>        m_mapMetadataBool;
    std::unordered_map<std::string, int>         m_mapMetadataInt;
    std::unordered_map<std::string, uint64_t>    m_mapMetadataInt64;
    std::unordered_map<std::string, float>       m_mapMetadataFloat;
    std::unordered_map<std::string, double>      m_mapMetadataDouble;
    std::unordered_map<std::string, std::string> m_mapMetadataString;
    std::unordered_map<std::string, CVector>     m_mapMetadataCVector;
};

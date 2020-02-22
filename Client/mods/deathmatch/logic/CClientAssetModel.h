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
class CLuaAssetNode;
class CLuaAssetMesh;
class CClientMeshBuffer;

enum ePrimitiveData
{
    PRIMITIVE_DATA_XYZ = 1 << 0,
    PRIMITIVE_DATA_XY = 1 << 1,
    PRIMITIVE_DATA_UV = 1 << 2,
    PRIMITIVE_DATA_DIFFUSE = 1 << 3,
    PRIMITIVE_DATA_INDICES16 = 1 << 29,
    PRIMITIVE_DATA_INDICES32 = 1 << 30,
};

struct SRenderingSettings
{
    CMatrix                 matrix;
    CLuaAssetNodeInterface* assetNode;
};

struct SAssetTexture
{
    const char*     fileName;
    CClientTexture* pClientTexture;
    SAssetTexture(const char* name, CClientTexture* pTexture)
    {
        fileName = name;
        pClientTexture = pTexture;
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
    void           Render(SRenderingSettings* settings);
    CLuaAssetMesh* GetMesh(int idx) { return m_vecAssetMeshes[idx]; }

protected:
    void CacheNodes(const aiNode* pNode);
    void CacheMeshes();

    CClientAssetModelManager* m_pAssetModelManager;

    unsigned int                      m_uiImportFlags;
    Assimp::Importer                  importer;
    CVector                           m_vecPosition;
    std::vector<const aiNode*>        vecNodes;
    const aiScene*                    m_pScene;
    std::vector<CLuaAssetNode*>       m_vecAssetNodes;
    std::vector<CLuaAssetMesh*>       m_vecAssetMeshes;
    std::vector<SAssetTexture>        m_vecAssetTextures;
    std::atomic<bool>                 m_bModelLoaded = false;
};

class CClientMeshBuffer
{
public:
    CClientMeshBuffer() { m_pDevice = g_pCore->GetGraphics()->GetDevice(); }
    ~CClientMeshBuffer();
    // only int and unsigned short are allowed
    template <typename T>
    void CreateIndexBuffer(std::vector<T>& vecIndexList);

    template <typename T>
    void AddVertexBuffer(void* pData, size_t size, ePrimitiveData primitiveData);

public:
    IDirect3DDevice9*              m_pDevice;
    IDirect3DIndexBuffer9*         m_pIndexBuffer;
    IDirect3DVertexBuffer9*        m_arrayVertexBuffer[8] = {nullptr};
    int                            m_iStrideSize[8] = {0};
    LPDIRECT3DVERTEXDECLARATION9   m_pVertexDeclaration;
    D3DPRIMITIVETYPE               m_ePrimitiveType;
    std::vector<D3DVERTEXELEMENT9> m_vecVertexElements;
    int                            m_iFaceCount;
    int                            m_iIndicesCount;
    int                            m_iVertexCount;
    int                            m_FVF;
    float                          m_fBuffer[24] = {0};
    bool                           m_bUseIndexedPrimitives;
    bool                           m_bRequireMaterial;
    size_t                         m_szMemoryUsageInBytes;
};

template <typename T>
void CClientMeshBuffer::CreateIndexBuffer(std::vector<T>& vecIndexList)
{
    void* pVoid;

    if (std::is_same<T, int>::value)
        m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(T), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    else
        m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(T), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);

    m_pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(T));
    m_pIndexBuffer->Unlock();
    m_iIndicesCount = vecIndexList.size();
    m_bUseIndexedPrimitives = true;
    m_szMemoryUsageInBytes += vecIndexList.size() * sizeof(T);
}

template <typename T>
void CClientMeshBuffer::AddVertexBuffer(void* pData, size_t size, ePrimitiveData primitiveData)
{
    int index = 0;
    int FVF = 0;
    switch (primitiveData)
    {
        case PRIMITIVE_DATA_XYZ:
            index = 0;
            m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0});
            FVF = D3DFVF_XYZ;
            break;
        case PRIMITIVE_DATA_UV:
            index = 1;
            m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0});
            FVF = D3DFVF_TEX1;
            break;
        case PRIMITIVE_DATA_DIFFUSE:
            index = 2;
            m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0});
            FVF = D3DFVF_DIFFUSE;
            break;
    }

    void* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateVertexBuffer(size * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, pData, size * sizeof(T));
    m_arrayVertexBuffer[index]->Unlock();
    
    m_iStrideSize[index] = sizeof(T);
    m_szMemoryUsageInBytes += size * sizeof(T);
}

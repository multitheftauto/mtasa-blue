/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMeshBuffer.h
 *  PURPOSE:     Mesh buffer class header
 *
 *****************************************************************************/

#pragma once

enum ePrimitiveData
{
    PRIMITIVE_DATA_XYZ = 1 << 0,
    PRIMITIVE_DATA_XY = 1 << 1,
    PRIMITIVE_DATA_UV = 1 << 2,
    PRIMITIVE_DATA_DIFFUSE = 1 << 3,
    PRIMITIVE_DATA_INDICES16 = 1 << 29,
    PRIMITIVE_DATA_INDICES32 = 1 << 30,
};

#define D3DDECL_END() \
    { \
        0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 \
    }

class CClientMeshBuffer
{
public:
    CClientMeshBuffer() { m_pDevice = g_pCore->GetGraphics()->GetDevice(); }
    ~CClientMeshBuffer();
    // only int and unsigned short are allowed
    template <typename T>
    void CreateIndexBuffer(std::vector<T>& vecIndexList)
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
    void AddVertexBuffer(void* pData, size_t size, ePrimitiveData primitiveData)
    {
        int index;
        switch (primitiveData)
        {
            case PRIMITIVE_DATA_XYZ:
                index = 0;
                m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0});
                m_FVF += D3DFVF_XYZ;
                break;
            case PRIMITIVE_DATA_UV:
                index = 1;
                m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0});
                m_FVF += D3DFVF_TEX1;
                break;
            case PRIMITIVE_DATA_DIFFUSE:
                index = 2;
                m_vecVertexElements.push_back({(WORD)index, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0});
                m_FVF += D3DFVF_DIFFUSE;
                break;
        }

        void* pVoid;            // POINTER TO POINTER, remember forkerer
        m_pDevice->CreateVertexBuffer(size * sizeof(T), D3DUSAGE_WRITEONLY, m_FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
        m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid, pData, size * sizeof(T));
        m_arrayVertexBuffer[index]->Unlock();

        m_iStrideSize[index] = sizeof(T);
        m_szMemoryUsageInBytes += size * sizeof(T);
    }

    void Finalize()
    {
        m_vecVertexElements.push_back(D3DDECL_END());
        m_pDevice->CreateVertexDeclaration(&m_vecVertexElements[0], &m_pVertexDeclaration);
    }

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
    int                            m_FVF = 0;
    float                          m_fBuffer[24] = {0};
    bool                           m_bUseIndexedPrimitives;
    int                            m_uiMaterialIndex;
    std::vector<int>               m_vecMaterials;
    size_t                         m_szMemoryUsageInBytes;
};

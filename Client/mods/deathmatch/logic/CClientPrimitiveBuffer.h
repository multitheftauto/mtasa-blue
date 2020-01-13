/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientPrimitiveBuffer.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CClientPrimitiveBufferInterface.h"
#include "CClientEntity.h"

class CClientPrimitiveBuffer : public CClientPrimitiveBufferInterface, public CClientEntity
{
    DECLARE_CLASS(CClientPrimitiveBuffer, CClientEntity);

public:
    ~CClientPrimitiveBuffer();
    CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID);
    virtual eClientEntityType GetType() const { return CCLIENTPRIMITIVEBUFFER; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    // only int and unsigned short are allowed
    template <typename T>
    void CreateIndexBuffer(std::vector<T>& vecIndexList);

    template <typename T>
    void AddVertexBuffer(std::vector<T>& vecVertexList, ePrimitiveData primitiveData);


    void PreDraw();
    void Draw(PrimitiveBufferSettings& settings);
    void SetPrimitiveType(D3DPRIMITIVETYPE ePrimitiveType) { m_ePrimitiveType = ePrimitiveType; }
    void SetFVF(int FVF) { m_FVF = FVF; }
    bool IsRequireMaterial() const { return m_bRequireMaterial; }
    void SetRequireMaterial(bool bRequire) { m_bRequireMaterial = bRequire; }
    void Finalize();

private:
    IDirect3DDevice9*                                 m_pDevice;
    IDirect3DIndexBuffer9*                            m_pIndexBuffer;
    IDirect3DVertexBuffer9*                           m_arrayVertexBuffer[8] = {nullptr};
    int                                               m_iStrideSize[8] = {0};
    LPDIRECT3DVERTEXDECLARATION9                      m_pVertexDeclaration;
    D3DPRIMITIVETYPE                                  m_ePrimitiveType;
    std::vector<D3DVERTEXELEMENT9>                    m_vecVertexElements;
    int                                               m_iFaceCount;
    int                                               m_iIndicesCount;
    int                                               m_iVertexCount;
    int                                               m_FVF;
    float                                             m_fBuffer[24] = {0};
    bool                                              m_bUseIndexedPrimitives;
    bool                                              m_bRequireMaterial;
    size_t                                            m_szMemoryUsageInBytes;
};

template <typename T>
void CClientPrimitiveBuffer::CreateIndexBuffer(std::vector<T>& vecIndexList)
{
    void* pVoid;            // POINTER TO POINTER, remember forkerer

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
void CClientPrimitiveBuffer::AddVertexBuffer(std::vector<T>& vecVertexList, ePrimitiveData primitiveData)
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
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(T));
    m_arrayVertexBuffer[index]->Unlock();

    m_iFaceCount = vecVertexList.size() / 3;
    m_iStrideSize[index] = sizeof(T);
    m_szMemoryUsageInBytes += vecVertexList.size() * sizeof(T);
}

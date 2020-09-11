/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMultiMaterialMeshBuffer.h
 *  PURPOSE:     Multi materialesh buffer class header
 *
 *****************************************************************************/

#pragma once

class CClientMultiMaterialMeshBuffer
{
public:
    CClientMultiMaterialMeshBuffer();
    ~CClientMultiMaterialMeshBuffer();
    void CreateIndexBuffer(std::vector<int>& vecIndices, std::vector<int> vecTexturesOffsets);
    void AddVertexBuffer(void* pData, size_t size, size_t typeSize, ePrimitiveData primitiveData);
    void Finalize();

public:
    IDirect3DDevice9*              m_pDevice;
    IDirect3DIndexBuffer9*         m_pIndexBuffer;
    IDirect3DVertexBuffer9*        m_arrayVertexBuffer[8] = {nullptr};
    int                            m_iStrideSize[8] = {0};
    LPDIRECT3DVERTEXDECLARATION9   m_pVertexDeclaration;
    D3DPRIMITIVETYPE               m_ePrimitiveType;
    std::vector<D3DVERTEXELEMENT9> m_vecVertexElements;
    std::vector<int>               m_vecTexturesOffsets;
    int                            m_iFaceCount;
    int                            m_iIndicesCount;
    int                            m_iVertexCount;
    int                            m_FVF = 0;
    float                          m_fBuffer[24] = {0};
    bool                           m_bUseIndexedPrimitives;
    int                            m_uiMaterialIndex;
    size_t                         m_szMemoryUsageInBytes;
};

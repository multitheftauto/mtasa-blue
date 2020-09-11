/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMultiMaterialMeshBuffer.cpp
 *  PURPOSE:     Multi material mesh buffer class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientMultiMaterialMeshBuffer.h"

CClientMultiMaterialMeshBuffer::CClientMultiMaterialMeshBuffer()
{
    m_pDevice = g_pCore->GetGraphics()->GetDevice();
}

CClientMultiMaterialMeshBuffer::~CClientMultiMaterialMeshBuffer()
{

}

void CClientMultiMaterialMeshBuffer::CreateIndexBuffer(std::vector<int>& vecIndices, std::vector<int> vecTexturesOffsets)
{
    m_vecTexturesOffsets = vecTexturesOffsets;
    void* pVoid;

    m_pDevice->CreateIndexBuffer(vecIndices.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndices.data(), vecIndices.size() * sizeof(int));
    m_pIndexBuffer->Unlock();
    m_iIndicesCount = vecIndices.size();
    m_bUseIndexedPrimitives = true;
    m_szMemoryUsageInBytes += vecIndices.size() * sizeof(int);
}

void CClientMultiMaterialMeshBuffer::AddVertexBuffer(void* pData, size_t size, size_t typeSize, ePrimitiveData primitiveData)
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
    m_pDevice->CreateVertexBuffer(size * typeSize, D3DUSAGE_WRITEONLY, m_FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, pData, size * typeSize);
    m_arrayVertexBuffer[index]->Unlock();
    m_iStrideSize[index] = typeSize;
    m_szMemoryUsageInBytes += size * typeSize;
}

void CClientMultiMaterialMeshBuffer::Finalize()
{
    m_vecVertexElements.push_back(D3DDECL_END());
    m_pDevice->CreateVertexDeclaration(&m_vecVertexElements[0], &m_pVertexDeclaration);
}

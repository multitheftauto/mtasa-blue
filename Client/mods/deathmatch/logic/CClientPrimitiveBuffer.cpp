/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientPrimitiveBuffer.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

enum ePrimitiveData;

CClientPrimitiveBuffer::CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID) : CClientEntity(ID)
{
    m_pManager = pManager;
    SetTypeName("primitiveBuffer");
    m_pDevice = g_pCore->GetGraphics()->GetDevice();
    m_iIndicesCount = 0;
    m_bUseIndexedPrimitives = false;
    m_pIndexBuffer = nullptr;
    m_bRequireMaterial = false;
    std::fill(m_arrayVertexBuffer, m_arrayVertexBuffer + 8, nullptr);
    std::fill(m_iStrideSize, m_iStrideSize + 8, 0);
}

void CClientPrimitiveBuffer::Unlink()
{
}

void CClientPrimitiveBuffer::CreateIndexBuffer(std::vector<unsigned short>& vecIndexList)
{
    void* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(short), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(short));
    m_pIndexBuffer->Unlock();
    m_iIndicesCount = vecIndexList.size();
    m_bUseIndexedPrimitives = true;
}

void CClientPrimitiveBuffer::CreateIndexBuffer(std::vector<int>& vecIndexList)
{
    void* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    m_pIndexBuffer->Unlock();
    m_iIndicesCount = vecIndexList.size();
    m_bUseIndexedPrimitives = true;
    m_iFaceCount = m_iIndicesCount / 3;
}

void CClientPrimitiveBuffer::AddVertexBuffer(std::vector<CVector>& vecVertexList, ePrimitiveData primitiveData)
{
    int index = 0;
    int FVF = 0;
    switch (primitiveData)
    {
        case PRIMITIVE_DATA_XYZ:
            m_vecVertexElements.push_back({0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0});
            index = 0;
            FVF = D3DFVF_XYZ;
            break;
        case PRIMITIVE_DATA_UV:
            m_vecVertexElements.push_back({1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0});
            index = 1;
            FVF = D3DFVF_TEX1;
            break;
        case PRIMITIVE_DATA_DIFFUSE:
            m_vecVertexElements.push_back({2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0});
            index = 2;
            FVF = D3DFVF_DIFFUSE;
            break;
    }

    VOID* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(CVector), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(CVector));
    m_arrayVertexBuffer[index]->Unlock();

    m_iFaceCount = vecVertexList.size() / 3;
    m_iStrideSize[index] = sizeof(CVector);
}
void CClientPrimitiveBuffer::AddVertexBuffer(std::vector<CVector2D>& vecVertexList, ePrimitiveData primitiveData)
{
    int index = 0;
    int FVF = 0;
    switch (primitiveData)
    {
        case PRIMITIVE_DATA_XYZ:
            m_vecVertexElements.push_back({0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0});
            index = 0;
            FVF = D3DFVF_XYZ;
            break;
        case PRIMITIVE_DATA_UV:
            m_vecVertexElements.push_back({1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0});
            index = 1;
            FVF = D3DFVF_TEX1;
            break;
        case PRIMITIVE_DATA_DIFFUSE:
            m_vecVertexElements.push_back({2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0});
            index = 2;
            FVF = D3DFVF_DIFFUSE;
            break;
    }

    VOID* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(CVector2D), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(CVector2D));
    m_arrayVertexBuffer[index]->Unlock();

    m_iFaceCount = vecVertexList.size() / 3;
    m_iStrideSize[index] = sizeof(CVector2D);
    m_bUseIndexedPrimitives = true;
    m_bRequireMaterial = true;
}

void CClientPrimitiveBuffer::AddVertexBuffer(std::vector<D3DCOLOR>& vecVertexList, ePrimitiveData primitiveData)
{
    int index = 0;
    int FVF = 0;
    switch (primitiveData)
    {
        case PRIMITIVE_DATA_XYZ:
            m_vecVertexElements.push_back({0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0});
            index = 0;
            FVF = D3DFVF_XYZ;
            break;
        case PRIMITIVE_DATA_UV:
            m_vecVertexElements.push_back({1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0});
            index = 1;
            FVF = D3DFVF_TEX1;
            break;
        case PRIMITIVE_DATA_DIFFUSE:
            m_vecVertexElements.push_back({2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0});
            index = 2;
            FVF = D3DFVF_DIFFUSE;
            break;
    }

    VOID* pVoid;            // POINTER TO POINTER, remember forkerer
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(D3DCOLOR), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_arrayVertexBuffer[index], NULL);
    m_arrayVertexBuffer[index]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(D3DCOLOR));
    m_arrayVertexBuffer[index]->Unlock();

    m_iFaceCount = vecVertexList.size() / 3;
    m_iStrideSize[index] = sizeof(D3DCOLOR);
}

void CClientPrimitiveBuffer::Finalize()
{
    m_vecVertexElements.push_back(D3DDECL_END());
    m_pDevice->CreateVertexDeclaration(&m_vecVertexElements[0], &m_pVertexDeclaration);
}

void CClientPrimitiveBuffer::PreDraw()
{
    m_pDevice->SetFVF(m_FVF);
    m_pDevice->SetVertexDeclaration(m_pVertexDeclaration);
    if (m_bUseIndexedPrimitives)
        m_pDevice->SetIndices(m_pIndexBuffer);

    for (int i = 0; i < 8; i++)
        if (m_arrayVertexBuffer[i] != nullptr)
            m_pDevice->SetStreamSource(i, m_arrayVertexBuffer[i], 0, m_iStrideSize[i]);
}

void CClientPrimitiveBuffer::Draw(PrimitiveBufferSettings& settings)
{
    settings.matrix.GetBuffer(buffer);
    m_pDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)buffer);
    if (m_bUseIndexedPrimitives)
        m_pDevice->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iFaceCount, 0, m_iIndicesCount);
    else
        m_pDevice->DrawPrimitive(m_ePrimitiveType, 0, m_iFaceCount);
}

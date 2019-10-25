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

CClientPrimitiveBuffer::CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID) : CClientEntity(ID)
{
    m_pManager = pManager;
    SetTypeName("primitiveBuffer");
}

void CClientPrimitiveBuffer::Unlink()
{
}

void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZ>& vecVertexList, std::vector<int>& vecIndexList)
{
    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ);
}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZUV>& vecVertexList, std::vector<int>& vecIndexList)
{
    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_TEX1);

}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZDiffuse>& vecVertexList, std::vector<int>& vecIndexList)
{
    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_DIFFUSE);

}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZUVDiffuse>& vecVertexList, std::vector<int>& vecIndexList)
{
    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
}

template <typename T>
void CClientPrimitiveBuffer::CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid; // POINTER TO POINTER, remember forkerer
    VOID*             pIndexVoid;
    pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    m_pVertexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(T));
    m_pVertexBuffer->Unlock();

    pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pIndexVoid, 0);
    memcpy(pIndexVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    m_pIndexBuffer->Unlock();

    m_iIndicesCount = vecIndexList.size();
    m_iBufferSize = vecVertexList.size();
    m_FVF = FVF;
    m_iTypeSize = sizeof(T);
}

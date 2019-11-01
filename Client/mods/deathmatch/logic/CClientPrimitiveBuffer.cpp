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
    D3DVERTEXELEMENT9 dwVertexElement[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};

    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ, dwVertexElement);
}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZUV>& vecVertexList, std::vector<int>& vecIndexList)
{
    D3DVERTEXELEMENT9 dwVertexElement[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                           {0, 3 * sizeof(float), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                                   D3DDECL_END()};

    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_TEX1, dwVertexElement);
}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZDiffuse>& vecVertexList, std::vector<int>& vecIndexList)
{
    D3DVERTEXELEMENT9 dwVertexElement[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                           {0, 3 * sizeof(float), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
                                      D3DDECL_END()};

    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_DIFFUSE, dwVertexElement);
}
void CClientPrimitiveBuffer::CreateBuffer(std::vector<VertexXYZDiffuseUV>& vecVertexList, std::vector<int>& vecIndexList)
{
    D3DVERTEXELEMENT9 dwVertexElement[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                           {0, 3 * sizeof(float), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
                                           {0, 3 * sizeof(float) + sizeof(DWORD), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                                        D3DDECL_END()};

    CreateBuffer(vecVertexList, vecIndexList, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, dwVertexElement);
}

template <typename T>
void CClientPrimitiveBuffer::CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF, D3DVERTEXELEMENT9 dwVertexElement[])
{
    m_pDevice = g_pCore->GetGraphics()->GetDevice();
    m_pDevice->CreateVertexDeclaration(dwVertexElement, &m_pVertexDeclaration);
    VOID*             pVoid;            // POINTER TO POINTER, remember forkerer
    VOID*             pIndexVoid;
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    m_pVertexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(T));
    m_pVertexBuffer->Unlock();

    m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pIndexVoid, 0);
    memcpy(pIndexVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    m_pIndexBuffer->Unlock();

    m_iIndicesCount = vecIndexList.size();
    m_iFaceCount = vecVertexList.size()/3;
    m_FVF = FVF;
    m_iStrideSize = sizeof(T);
}

void CClientPrimitiveBuffer::PreDraw()
{
    m_pDevice->SetFVF(m_FVF);
    m_pDevice->SetVertexDeclaration(m_pVertexDeclaration);
    m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_iStrideSize);
    m_pDevice->SetIndices(m_pIndexBuffer);
}

void CClientPrimitiveBuffer::Draw(PrimitiveBufferSettings& settings)
{
    settings.matrix.GetBuffer(buffer);
    m_pDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)buffer);
    m_pDevice->DrawPrimitive(m_iPrimitiveType, 0, m_iFaceCount);
}

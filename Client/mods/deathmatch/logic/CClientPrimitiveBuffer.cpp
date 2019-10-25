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

template <typename T>
void CClientPrimitiveBuffer::CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF)
{
    //IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    //VOID*             pVoid; // POINTER TO POINTER, remember forkerer
    //VOID*             pIndexVoid;
    //pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_pBuffer, NULL);
    //m_pBuffer->Lock(0, 0, (void**)&pVoid, 0);
    //memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(T));
    //m_pBuffer->Unlock();

    //pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    //m_pIndexBuffer->Lock(0, 0, (void**)&pIndexVoid, 0);
    //memcpy(pIndexVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    //m_pIndexBuffer->Unlock();

    //m_iIndicesCount = vecIndexList.size();
    //m_iBufferSize = vector.size();
    //m_FVF = FVF;
    //m_iTypeSize = sizeof(T);
}

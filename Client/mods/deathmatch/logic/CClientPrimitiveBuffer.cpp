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

CClientPrimitiveBuffer::CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    m_pManager = pManager;
    SetTypeName("primitivebuffer");
    m_pDevice = g_pCore->GetGraphics()->GetDevice();
    m_iIndicesCount = 0;
    m_szMemoryUsageInBytes = 0;
    m_iFaceCount = 0;
    m_iVertexCount = 0;
    m_bUseIndexedPrimitives = false;
    m_pIndexBuffer = nullptr;
    m_bRequireMaterial = false;
    m_FVF = 0;
    m_ePrimitiveType = D3DPT_TRIANGLELIST;
}

void CClientPrimitiveBuffer::Unlink()
{
    g_pClientGame->GetPrimitiveBufferManager()->Delete(this);
}

CClientPrimitiveBuffer::~CClientPrimitiveBuffer()
{
    delete m_pIndexBuffer;
    for (int i = 0; i < 8; i++)
        if (m_arrayVertexBuffer[i] != nullptr)
        {
            m_arrayVertexBuffer[i]->Release();
            delete m_arrayVertexBuffer[i];
        }

    delete[] m_iStrideSize;
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
    settings.matrix.GetBuffer(m_fBuffer);
    m_pDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)m_fBuffer);
    if (m_bUseIndexedPrimitives)
        m_pDevice->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iFaceCount, 0, m_iIndicesCount);
    else
        m_pDevice->DrawPrimitive(m_ePrimitiveType, 0, m_iFaceCount);
}

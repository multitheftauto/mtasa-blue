/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBufferBatcher.cpp
 *  PURPOSE:
 *
 *
 *****************************************************************************/

#include <StdInc.h>
#include "../Client/mods/deathmatch/logic/CClientPrimitiveBufferInterface.h"
#include "CPrimitiveBufferBatcher.h"

////////////////////////////////////////////////////////////////
//
// CPrimitiveBufferBatcher::CPrimitiveBufferBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveBufferBatcher::CPrimitiveBufferBatcher(CGraphics* graphics)
{
    m_pGraphics = graphics;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBufferBatcher::~CPrimitiveBufferBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveBufferBatcher::~CPrimitiveBufferBatcher()
{
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBufferBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveBufferBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    // Cache matrices
    UpdateMatrices(fViewportSizeX, fViewportSizeY);
    D3DXMatrixIdentity(&m_MatWorld);
}

void CPrimitiveBufferBatcher::OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY)
{
    Flush();
    UpdateMatrices(uiNewViewportSizeX, uiNewViewportSizeY);
}

void CPrimitiveBufferBatcher::UpdateMatrices(float fViewportSizeX, float fViewportSizeY)
{
    m_fViewportSizeX = fViewportSizeX;
    m_fViewportSizeY = fViewportSizeY;
    D3DXMatrixIdentity(&m_MatView);
    D3DXMatrixIdentity(&m_MatProjection);
    // Make projection 3D friendly, so shaders can alter the z coord for fancy effects
    float fFarPlane = 10000;
    float fNearPlane = 100;
    float Q = fFarPlane / (fFarPlane - fNearPlane);
    float fAdjustZFactor = 1000.f;
    float rcpSizeX = 2.0f / m_fViewportSizeX;
    float rcpSizeY = -2.0f / m_fViewportSizeY;
    rcpSizeX *= fAdjustZFactor;
    rcpSizeY *= fAdjustZFactor;
    m_MatProjection.m[0][0] = rcpSizeX;
    m_MatProjection.m[1][1] = rcpSizeY;
    m_MatProjection.m[2][2] = Q;
    m_MatProjection.m[2][3] = 1;
    m_MatProjection.m[3][0] = (-m_fViewportSizeX / 2.0f - 0.5f) * rcpSizeX;
    m_MatProjection.m[3][1] = (-m_fViewportSizeY / 2.0f - 0.5f) * rcpSizeY;
    m_MatProjection.m[3][2] = -Q * fNearPlane;
    m_MatProjection.m[3][3] = 0;
    m_MatView.m[3][2] = fAdjustZFactor;

    D3DXMatrixOrthoOffCenterLH(&m_MatProjectionOrtho, 0, fViewportSizeX, fViewportSizeY, 0,
                               1.0f, 1000.0f);
}

void CPrimitiveBufferBatcher::SetDeviceStates()
{
    m_pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
    // Set states
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

void CPrimitiveBufferBatcher::Flush()
{
    if (m_primitiveBufferMap.empty())
        return;

    // Save render states
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

    // Set transformations
    m_pDevice->SetTransform(D3DTS_WORLD, &m_MatWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &m_MatView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);

    // Set device states
    SetDeviceStates();

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    for (auto& primitive : m_primitiveBufferMap)
    {
        if (primitive.first != nullptr && primitive.second.size() > 0)
        {
            primitive.first->PreDraw();
            for (auto& settings : primitive.second)
            {
                switch (settings.eView)
                {
                    case PRIMITIVE_VIEW_ORTHOGRAPHIC:
                        m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjectionOrtho);
                        break;
                    case PRIMITIVE_VIEW_PERSPECTIVE:
                        m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);
                        break;
                }
                primitive.first->Draw(settings);
            }
        }
    }

    // Clean up
    ClearQueue();

    // Restore render states
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

void CPrimitiveBufferBatcher::ClearQueue()
{
    for (auto& primitive : m_primitiveBufferMap)
    {
        primitive.second.clear();
    }
}

void CPrimitiveBufferBatcher::AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, PrimitiveBufferSettings& bufferSettings)
{
    if (m_primitiveBufferMap.find(pPrimitiveBuffer) == m_primitiveBufferMap.end())
        m_primitiveBufferMap[pPrimitiveBuffer] = std::vector<PrimitiveBufferSettings>();

    m_primitiveBufferMap[pPrimitiveBuffer].push_back(bufferSettings);
}

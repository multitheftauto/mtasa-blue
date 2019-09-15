/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CFBXBatcher.cpp
 *  PURPOSE:
 *
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CFBXBatcher.h"
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::CFBXBatcher
//
//
//
////////////////////////////////////////////////////////////////

CFBXBatcher::CFBXBatcher()
{
    D3DXMatrixIdentity(&m_MatWorld);
}
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::~CFBXBatcher
//
//
//
////////////////////////////////////////////////////////////////
CFBXBatcher::~CFBXBatcher()
{
}
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CFBXBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    // Cache matrices
    UpdateMatrices(fViewportSizeX, fViewportSizeY);

    g_pCore->GetFBX()->Initialize();
}
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CFBXBatcher::OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY)
{
    // Flush dx draws
    //Render();
    // Make new projection transform
    UpdateMatrices(uiNewViewportSizeX, uiNewViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CFBXBatcher::UpdateMatrices(float fViewportSizeX, float fViewportSizeY)
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
}
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::SetDeviceStates
//
//
//
////////////////////////////////////////////////////////////////
void CFBXBatcher::SetDeviceStates()
{
    // Set states
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
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
////////////////////////////////////////////////////////////////
//
// CFBXBatcher::Render
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////

void CFBXBatcher::Render()
{
    // Get scene matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    const D3DXMATRIX& matView = g_pDeviceState->TransformState.VIEW;
    const D3DXMATRIX& matProjection = g_pDeviceState->TransformState.PROJECTION;

    // Set transforms
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &matView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

    // fix alpha blending, func at address 0x005D6480 drawing models in gtasa

    // Set states
    if (g_pDeviceState->AdapterState.bRequiresClipping)
        m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
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
    m_pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    g_pCore->GetFBX()->UpdateFrustum(65535, matProjection, matView);
    g_pCore->GetFBX()->Render();


    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

void CFBXBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
{
    int iSize = 1;
    switch (eType)
    {
        case D3DPT_POINTLIST:
            iSize = iCollectionSize;
            break;
        case D3DPT_LINELIST:
            iSize = iCollectionSize / 2;
            break;
        case D3DPT_LINESTRIP:
            iSize = iCollectionSize - 1;
            break;
        case D3DPT_TRIANGLEFAN:
        case D3DPT_TRIANGLESTRIP:
            iSize = iCollectionSize - 2;
            break;
        case D3DPT_TRIANGLELIST:
            iSize = iCollectionSize / 3;
            break;
    }
    m_pDevice->DrawPrimitiveUP(eType, iSize, pDataAddr, uiVertexStride);
}

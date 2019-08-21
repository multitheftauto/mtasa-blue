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

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)            // | D3DFVF_TEX1;
CFBXBatcher::CFBXBatcher()
{
    D3DXMatrixIdentity(&m_MatWorld);
    vertices = {
        {0, 0, 3, D3DCOLOR_XRGB(255, 0, 0)}, {5, 0, 3, D3DCOLOR_XRGB(255, 0, 0)}, {5, 5, 3, D3DCOLOR_XRGB(255, 0, 0)},
        {3, 0, 5, D3DCOLOR_XRGB(255, 0, 0)}, {5, 0, 5, D3DCOLOR_XRGB(255, 0, 0)}, {5, 2, 5, D3DCOLOR_XRGB(255, 0, 0)},
    };
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(3);
    indices.emplace_back(4);
    indices.emplace_back(5);
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

    m_pDevice->CreateVertexBuffer(vertices.size() * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, CUSTOMFVF, D3DPOOL_MANAGED, &v_buffer, NULL);
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vertices.data(), vertices.size() * sizeof(CUSTOMVERTEX));
    v_buffer->Unlock();

    m_pDevice->CreateIndexBuffer(indices.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &i_buffer, NULL);
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indices.data(), indices.size() * sizeof(int));
    i_buffer->Unlock();
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

    // Pre-calc camera position
    D3DXMATRIX matViewInv;
    D3DXMatrixInverse(&matViewInv, NULL, &matView);
    const CVector vecCameraPos(matViewInv._41, matViewInv._42, matViewInv._43);

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

    // Set vertex stream
    // static const uint FNV = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    m_pDevice->SetFVF(D3DFVF_XYZ);

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    uint uiVertexStreamZeroStride = sizeof(PrimitiveMaterialVertice);

    // select which vertex format we are using
    m_pDevice->SetFVF(CUSTOMFVF);

    // select the vertex buffer to display
    m_pDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    m_pDevice->SetIndices(i_buffer);

    // copy the vertex buffer to the back buffer
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, indices.size(), 0, indices.size() / 3);

    //g_pCore->GetFBX()->Render();
    // CMaterialItem* pMaterial = primitive.pMaterial;
    // if (pMaterial != pLastMaterial)
    //{
    //    // Set texture addressing mode
    //    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pMaterial->m_TextureAddress);
    //    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pMaterial->m_TextureAddress);

    //    if (pMaterial->m_TextureAddress == TADDRESS_BORDER)
    //        m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pMaterial->m_uiBorderColor);
    //}

    // if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(pMaterial))
    //{
    //    m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
    //    DrawPrimitive(D3DPT_TRIANGLELIST, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
    //}
    // else if (CShaderInstance* pShaderInstance = DynamicCast<CShaderInstance>(pMaterial))
    //{
    //    // Draw using shader
    //    ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

    //    if (pMaterial != pLastMaterial)
    //    {
    //        // Apply custom parameters
    //        pShaderInstance->ApplyShaderParameters();
    //        // Apply common parameters
    //        pShaderInstance->m_pEffectWrap->ApplyCommonHandles();
    //        // Apply mapped parameters
    //        pShaderInstance->m_pEffectWrap->ApplyMappedHandles();
    //    }

    //    // Do shader passes
    //    DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;
    //    uint  uiNumPasses = 0;
    //    pShaderInstance->m_pEffectWrap->Begin(&uiNumPasses, dwFlags, false);

    //    for (uint uiPass = 0; uiPass < uiNumPasses; uiPass++)
    //    {
    //        pD3DEffect->BeginPass(uiPass);
    //        DrawPrimitive(primitive.eType, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
    //        pD3DEffect->EndPass();
    //    }
    //    pShaderInstance->m_pEffectWrap->End();

    //    // If we didn't get the effect to save the shader state, clear some things here
    //    if (dwFlags & D3DXFX_DONOTSAVESHADERSTATE)
    //    {
    //        m_pDevice->SetVertexShader(NULL);
    //        m_pDevice->SetPixelShader(NULL);
    //    }
    //}
    // pLastMaterial = pMaterial;
    // pMaterial->Release();

    // Clean up
    // ClearQueue();

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

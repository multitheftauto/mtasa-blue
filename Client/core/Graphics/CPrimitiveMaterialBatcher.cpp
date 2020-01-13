/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveMaterialBatcher.cpp
 *  PURPOSE:
 *
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CPrimitiveMaterialBatcher.h"
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::CPrimitiveMaterialBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveMaterialBatcher::CPrimitiveMaterialBatcher(CGraphics* graphics)
{
    m_pGraphics = graphics;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::~CPrimitiveMaterialBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveMaterialBatcher::~CPrimitiveMaterialBatcher()
{
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    // Cache matrices
    UpdateMatrices(fViewportSizeX, fViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY)
{
    // Flush dx draws
    Flush();
    // Make new projection transform
    UpdateMatrices(uiNewViewportSizeX, uiNewViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::UpdateMatrices(float fViewportSizeX, float fViewportSizeY)
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
// CPrimitiveMaterialBatcher::SetDeviceStates
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::SetDeviceStates()
{
    // Set states
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::Flush()
{
    if (m_primitiveList.empty())
        return;

    // Save render states
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);
    // Set transformations
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &m_MatView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);

    // Set vertex FVF
    m_pDevice->SetFVF(PrimitiveMaterialVertice::FNV);

    // Set device states
    SetDeviceStates();

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    for (auto& primitive : m_primitiveList)
    {
        const void* pVertexStreamZeroData = &primitive.pVecVertices->at(0);
        uint        uiVertexStreamZeroStride = sizeof(PrimitiveMaterialVertice);

        CMaterialItem* pMaterial = primitive.pMaterial;
        if (pMaterial != pLastMaterial)
        {
            // Set texture addressing mode
            m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pMaterial->m_TextureAddress);
            m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pMaterial->m_TextureAddress);

            if (pMaterial->m_TextureAddress == TADDRESS_BORDER)
                m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pMaterial->m_uiBorderColor);
        }

        if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(pMaterial))
        {
            // Draw using texture
            if (pMaterial != pLastMaterial)
            {
                m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
            }

            DrawPrimitive(primitive.eType, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
        }
        else if (CShaderInstance* pShaderInstance = DynamicCast<CShaderInstance>(pMaterial))
        {
            // Draw using shader
            ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

            if (pMaterial != pLastMaterial)
            {
                // Apply custom parameters
                pShaderInstance->ApplyShaderParameters();
                // Apply common parameters
                pShaderInstance->m_pEffectWrap->ApplyCommonHandles();
                // Apply mapped parameters
                pShaderInstance->m_pEffectWrap->ApplyMappedHandles();
            }

            // Do shader passes
            DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;
            uint  uiNumPasses = 0;
            pShaderInstance->m_pEffectWrap->Begin(&uiNumPasses, dwFlags, false);

            for (uint uiPass = 0; uiPass < uiNumPasses; uiPass++)
            {
                pD3DEffect->BeginPass(uiPass);
                DrawPrimitive(primitive.eType, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
                pD3DEffect->EndPass();
            }
            pShaderInstance->m_pEffectWrap->End();

            // If we didn't get the effect to save the shader state, clear some things here
            if (dwFlags & D3DXFX_DONOTSAVESHADERSTATE)
            {
                m_pDevice->SetVertexShader(NULL);
                m_pDevice->SetPixelShader(NULL);
            }
        }
        pLastMaterial = pMaterial;
        m_pGraphics->RemoveQueueRef(pMaterial);
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
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::DrawPrimitive
//
// Draws the primitives on render target
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
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
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::ClearQueue
//
// Clears all primitives in current queue
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::ClearQueue()
{
    // Clean up
    for (auto& primitive : m_primitiveList)
    {
        delete primitive.pVecVertices;
    }

    size_t prevSize = m_primitiveList.size();
    m_primitiveList.clear();
    m_primitiveList.reserve(prevSize);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::AddTriangle
//
// Add a new primitive to the list
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::AddPrimitive(D3DPRIMITIVETYPE eType, CMaterialItem* pMaterial, std::vector<PrimitiveMaterialVertice>* pVecVertices)
{
    m_primitiveList.push_back({eType, pMaterial, pVecVertices});
}

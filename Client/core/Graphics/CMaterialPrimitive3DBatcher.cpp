/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CMaterialPrimitive3DBatcher.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CMaterialPrimitive3DBatcher.h"
////////////////////////////////////////////////////////////////
//
// CMaterialPrimitive3DBatcher::CMaterialPrimitive3DBatcher
//
//
//
////////////////////////////////////////////////////////////////
CMaterialPrimitive3DBatcher::CMaterialPrimitive3DBatcher(bool bPreGUI, CGraphics* pGraphics) : m_bPreGUI(bPreGUI), m_pGraphics(pGraphics)
{
}
////////////////////////////////////////////////////////////////
//
// CMaterialPrimitive3DBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CMaterialPrimitive3DBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
}
////////////////////////////////////////////////////////////////
//
// CMaterialPrimitive3DBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CMaterialPrimitive3DBatcher::Flush()
{
    if (m_primitiveList.empty())
        return;

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
    m_pDevice->SetRenderState(D3DRS_ZENABLE, m_bPreGUI ? D3DZB_TRUE : D3DZB_FALSE);
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
    m_pDevice->SetFVF(PrimitiveMaterialVertice::FNV);

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    uint uiVertexStreamZeroStride = sizeof(PrimitiveMaterialVertice);

    for (auto& primitive : m_primitiveList)
    {
        const void* pVertexStreamZeroData = &primitive.pVecVertices->at(0);
        size_t      iCollectionSize = primitive.pVecVertices->size();

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
            m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
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
        pMaterial->Release();
    }

    // Clean up
    ClearQueue();

    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

void CMaterialPrimitive3DBatcher::ClearQueue()
{
    // Clean up
    for (auto& primitive : m_primitiveList)
    {
        delete primitive.pVecVertices;
    }

    m_primitiveList.clear();
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::DrawPrimitive
//
// Draws the primitives on render target
//
////////////////////////////////////////////////////////////////
void CMaterialPrimitive3DBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
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
// CMaterialPrimitive3DBatcher::AddPrimitive
//
// Add a new primitive to the list
//
////////////////////////////////////////////////////////////////
void CMaterialPrimitive3DBatcher::AddPrimitive(D3DPRIMITIVETYPE eType, CMaterialItem* pMaterial, std::vector<PrimitiveMaterialVertice>* pVecVertices)
{
    if (!pMaterial)
        return;

    pMaterial->AddRef();
    m_primitiveList.push_back({eType, pMaterial, pVecVertices});
}

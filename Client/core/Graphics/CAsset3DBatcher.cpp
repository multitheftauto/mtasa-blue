/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CAsset3DBatcher.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CAsset3DBatcher.h"
#include "core/CLuaAssetNodeInterface.h"
#include "../Client\mods\deathmatch\logic\CClientMeshBuffer.h"
#include "CAssetInstance.h"

CAsset3DBatcher::CAsset3DBatcher(CGraphics* pGraphics) : m_pGraphics(pGraphics)
{
}

void CAsset3DBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
}

void CAsset3DBatcher::Flush()
{
    if (m_vecRenderList.empty())
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

    m_pDevice->SetTexture(0, nullptr);

    if (g_pDeviceState->AdapterState.bRequiresClipping)
        m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CONSTANT);
    m_pDevice->SetTextureStageState(1, D3DTSS_CONSTANT, g_pCore->GetAssetsManager()->GetAmbientColor());

    float m_fBuffer[24] = {0};

    CClientMeshBuffer* pMeshBuffer;
    CMaterialItem*     pLastMaterial = nullptr;
    for (SRenderingSettings& renderSetting : m_vecRenderList)
    {
        //pGroup = g_pCore->GetAssetsManager()->GetRenderGroup(renderSetting.uiGroup);
        renderSetting.matrix.GetBuffer(m_fBuffer);
        m_pDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&m_fBuffer);
        for (int i = 0; i < renderSetting.assetNode->GetMeshNum(); i++)
        {
            pMeshBuffer = renderSetting.assetNode->GetMeshBuffer(i);

            for (int i = 0; i < 8; i++)
                if (pMeshBuffer->m_arrayVertexBuffer[i] != nullptr)
                    m_pDevice->SetStreamSource(i, pMeshBuffer->m_arrayVertexBuffer[i], 0, pMeshBuffer->m_iStrideSize[i]);
            m_pDevice->SetIndices(pMeshBuffer->m_pIndexBuffer);
            m_pDevice->SetFVF(pMeshBuffer->m_FVF);
            m_pDevice->SetVertexDeclaration(pMeshBuffer->m_pVertexDeclaration);

            if (pMeshBuffer->m_uiMaterialIndex >= 0)
            {
                CMaterialItem* pMaterial = renderSetting.assetNode->GetTexture(pMeshBuffer->m_uiMaterialIndex);
                if (pMaterial)
                {
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
                        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMeshBuffer->m_iIndicesCount, 0, pMeshBuffer->m_iFaceCount);
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
                            m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMeshBuffer->m_iIndicesCount, 0, pMeshBuffer->m_iFaceCount);
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
                    // pMaterial->Release();
                }
                else
                {
                    m_pDevice->SetTexture(0, nullptr);
                    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMeshBuffer->m_iIndicesCount, 0, pMeshBuffer->m_iFaceCount);
                }
            }
            else
            {
                m_pDevice->SetTexture(0, nullptr);
                m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pMeshBuffer->m_iIndicesCount, 0, pMeshBuffer->m_iFaceCount);
            }
        }
    }

    // Clean up
    ClearQueue();

    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

void CAsset3DBatcher::ClearQueue()
{
    m_vecRenderList.clear();
}

void CAsset3DBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
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

void CAsset3DBatcher::AddAsset(SRenderingSettings& settings)
{
    m_vecRenderList.push_back(std::move(settings));
}

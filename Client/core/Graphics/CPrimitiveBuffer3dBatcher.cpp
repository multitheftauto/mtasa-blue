/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBuffer3DBatcher.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CPrimitiveBuffer3DBatcher.h"
#include "../Client/mods/deathmatch/logic/CClientPrimitiveBufferInterface.h"

CPrimitiveBuffer3DBatcher::CPrimitiveBuffer3DBatcher(CGraphics* graphics, bool bPreGUI) : m_bPreGUI(bPreGUI)
{
    m_pGraphics = graphics;
}
void CPrimitiveBuffer3DBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
}

void CPrimitiveBuffer3DBatcher::Flush()
{
    if (m_primitiveBufferMap.empty())
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

    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

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
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Set transforms
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &matView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

    // Draw
    m_pDevice->SetTexture(0, nullptr);

    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    for (auto& primitive : m_primitiveBufferMap)
    {
        if (primitive.first != nullptr && primitive.second.size() > 0)
        {
            primitive.first->PreDraw();
            for (auto& settings : primitive.second)
            {
                CMaterialItem* pMaterial = settings.pMaterial;
                if (pMaterial != nullptr)
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
                        primitive.first->Draw(settings);
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
                            primitive.first->Draw(settings);
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
                else
                {
                    m_pDevice->SetTexture(0, nullptr);
                    primitive.first->Draw(settings);
                }
            }
        }
    }

    ClearQueue();

    if (g_pDeviceState->AdapterState.bRequiresClipping)
        m_pDevice->SetRenderState(D3DRS_CLIPPING, FALSE);

    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

void CPrimitiveBuffer3DBatcher::AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, PrimitiveBufferSettings& bufferSettings)
{
    if (m_primitiveBufferMap.find(pPrimitiveBuffer) == m_primitiveBufferMap.end())
        m_primitiveBufferMap[pPrimitiveBuffer] = std::vector<PrimitiveBufferSettings>();

    m_primitiveBufferMap[pPrimitiveBuffer].push_back(bufferSettings);
}

void CPrimitiveBuffer3DBatcher::ClearQueue()
{
    for (auto& primitive : m_primitiveBufferMap)
    {
        primitive.second.clear();
    }
}

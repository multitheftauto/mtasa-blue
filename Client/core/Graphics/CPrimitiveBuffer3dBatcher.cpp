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

    // Set transforms
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &matView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

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
    m_pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Draw
    m_pDevice->SetTexture(0, nullptr);

    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    for (auto& primitive : m_primitiveBufferMap)
    {
        if (primitive.first == nullptr || primitive.second.size() == 0)
            continue;

        primitive.first->PreDraw();
        for (auto& settings : primitive.second)
        {

            if (settings.pMaterial != nullptr)
            {
                // Change texture addressing mode if required
                if (m_CurrentTextureAddress != settings.pMaterial->m_TextureAddress)
                {
                    m_CurrentTextureAddress = settings.pMaterial->m_TextureAddress;
                    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, m_CurrentTextureAddress);
                    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, m_CurrentTextureAddress);
                }

                if (m_CurrentTextureAddress == TADDRESS_BORDER)
                    m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, settings.pMaterial->m_uiBorderColor);

                // Draw
                if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(settings.pMaterial))
                {
                    // Draw using texture
                    m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
                    primitive.first->Draw(settings);
                }
                else if (CShaderInstance* pShaderInstance = DynamicCast<CShaderInstance>(settings.pMaterial))
                {
                    // Draw using shader
                    ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

                    // Apply custom parameters
                    pShaderInstance->ApplyShaderParameters();
                    // Apply common parameters
                    pShaderInstance->m_pEffectWrap->ApplyCommonHandles();
                    // Apply mapped parameters
                    pShaderInstance->m_pEffectWrap->ApplyMappedHandles();

                    // Do shader passes
                    DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;            // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
                    uint  uiNumPasses = 0;
                    pShaderInstance->m_pEffectWrap->Begin(&uiNumPasses, dwFlags);

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
                settings.pMaterial->Release();
            }
            else
            {
                m_pDevice->SetTexture(0, nullptr);
                primitive.first->Draw(settings);
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

    if (bufferSettings.pMaterial)
        bufferSettings.pMaterial->AddRef();

    m_primitiveBufferMap[pPrimitiveBuffer].push_back(bufferSettings);
}

void CPrimitiveBuffer3DBatcher::ClearQueue()
{
    for (auto& primitive : m_primitiveBufferMap)
    {
        primitive.second.clear();
    }
}

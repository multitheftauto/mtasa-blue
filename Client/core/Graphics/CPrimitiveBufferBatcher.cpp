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

CPrimitiveBufferBatcher::CPrimitiveBufferBatcher(CGraphics* graphics)
{
    m_pGraphics = graphics;
}

CPrimitiveBufferBatcher::~CPrimitiveBufferBatcher()
{
}

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

    D3DXMatrixOrthoOffCenterLH(&m_MatProjectionOrtho, 0, fViewportSizeX, fViewportSizeY, 0, 1.0f, 1000.0f);
}

void CPrimitiveBufferBatcher::SetDeviceStates()
{
    m_pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);

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

void CPrimitiveBufferBatcher::SetDeviceMaterialStates()
{
    m_pDevice->Clear(0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);

    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

void CPrimitiveBufferBatcher::FlushMaterialPrimitives()
{
    SetDeviceMaterialStates();

    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    for (auto& primitive : m_materialPrimitiveBufferMap)
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
        }
    }
}
void CPrimitiveBufferBatcher::FlushPrimitives()
{
    SetDeviceStates();

    // Draw
    m_pDevice->SetTexture(0, nullptr);

    for (auto& primitive : m_primitiveBufferMap)
    {
        if (primitive.first == nullptr || primitive.second.size() == 0)
            continue;

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

    FlushPrimitives();
    FlushMaterialPrimitives();

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

    for (auto& primitive : m_materialPrimitiveBufferMap)
    {
        primitive.second.clear();
    }
}

void CPrimitiveBufferBatcher::AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, PrimitiveBufferSettings& bufferSettings)
{
    if (bufferSettings.pMaterial)
    {
        if (m_materialPrimitiveBufferMap.find(pPrimitiveBuffer) == m_materialPrimitiveBufferMap.end())
            m_materialPrimitiveBufferMap[pPrimitiveBuffer] = std::vector<PrimitiveBufferSettings>();

        bufferSettings.pMaterial->AddRef();

        m_materialPrimitiveBufferMap[pPrimitiveBuffer].push_back(bufferSettings);
    }
    else
    {
        if (m_primitiveBufferMap.find(pPrimitiveBuffer) == m_primitiveBufferMap.end())
            m_primitiveBufferMap[pPrimitiveBuffer] = std::vector<PrimitiveBufferSettings>();

        m_primitiveBufferMap[pPrimitiveBuffer].push_back(bufferSettings);
    }
}

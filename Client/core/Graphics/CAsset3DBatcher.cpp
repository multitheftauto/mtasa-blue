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

CAsset3DBatcher::~CAsset3DBatcher()
{
    // Make missing texture even more missing
    SAFE_DELETE(m_pMissingTexture);
}

void CAsset3DBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
}

void CAsset3DBatcher::Flush()
{
    if (m_mapRenderList.empty())
        return;

    if (m_pMissingTexture == nullptr)
        m_pMissingTexture = m_pGraphics->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\missing.png"));

    auto pAssetManager = g_pCore->GetAssetsManager();
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
    m_pDevice->SetTextureStageState(1, D3DTSS_CONSTANT, pAssetManager->GetAmbientColor());

    
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    float m_fBuffer[24] = {0};

    std::unordered_map<const CMaterialItem*, std::unordered_map<CClientMeshBuffer*, std::vector<std::pair<CMatrix,CMatrix>>>> mapTextured;
    std::unordered_map<CClientMeshBuffer*, std::vector<std::pair<CMatrix, CMatrix>>>                                           mapFlatColor;
    std::unordered_map<CClientMeshBuffer*, std::vector<std::pair<CMatrix, CMatrix>>>                                          mapMissingTexture;

    CClientMeshBuffer* pMeshBuffer;
    CMaterialItem*     pLastMaterial = nullptr;
    float             floats[24] = {0};
    for (auto const& renderItem : m_mapRenderList)
        for (int i = 0; i < renderItem.first->GetMeshNum(); i++)
        {
            pMeshBuffer = renderItem.first->GetMeshBuffer(i);

            if (pMeshBuffer->m_uiMaterialIndex >= 0)
            {
                const CMaterialItem* pMaterial = renderItem.first->GetTexture(pMeshBuffer->m_uiMaterialIndex + 1);
                if (pMaterial != nullptr)
                {
                    if (mapTextured.find(pMaterial) == mapTextured.end())
                    {
                        mapTextured[pMaterial] = std::unordered_map<CClientMeshBuffer*, std::vector<std::pair<CMatrix, CMatrix>>>();
                    }
                    if (mapTextured[pMaterial].find(pMeshBuffer) == mapFlatColor.end())
                    {
                        mapTextured[pMaterial][pMeshBuffer] = std::vector<std::pair<CMatrix, CMatrix>>();
                        mapTextured[pMaterial][pMeshBuffer].resize(mapTextured[pMaterial][pMeshBuffer].size() + renderItem.second.size());
                    }

                    for (auto const& matrix : renderItem.second)
                    {
                        mapTextured[pMaterial][pMeshBuffer].push_back(matrix);
                    }
                }
                else
                {
                    if (mapMissingTexture.find(pMeshBuffer) == mapFlatColor.end())
                    {
                        mapMissingTexture[pMeshBuffer] = std::vector<std::pair<CMatrix, CMatrix>>();
                    }

                    for (auto const& matrix : renderItem.second)
                    {
                        mapMissingTexture[pMeshBuffer].push_back(matrix);
                    }
                }
            }
            else
            {
                if (mapFlatColor.find(pMeshBuffer) == mapFlatColor.end())
                {
                    mapFlatColor[pMeshBuffer] = std::vector<std::pair<CMatrix, CMatrix>>();
                }

                for (auto const& matrix : renderItem.second)
                {
                    mapFlatColor[pMeshBuffer].push_back(matrix);
                }
            }
        }

    for (auto const& renderItem : mapTextured)
    {
        // Set texture addressing mode
        m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, renderItem.first->m_TextureAddress);
        m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, renderItem.first->m_TextureAddress);

        if (renderItem.first->m_TextureAddress == TADDRESS_BORDER)
            m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, renderItem.first->m_uiBorderColor);
        if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>((CMaterialItem*)renderItem.first))
        {
            m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
            for (auto const& meshPair : renderItem.second)
            {
                for (int i = 0; i < 8; i++)
                    if (meshPair.first->m_arrayVertexBuffer[i] != nullptr)
                        m_pDevice->SetStreamSource(i, meshPair.first->m_arrayVertexBuffer[i], 0, meshPair.first->m_iStrideSize[i]);
                m_pDevice->SetIndices(meshPair.first->m_pIndexBuffer);
                m_pDevice->SetFVF(meshPair.first->m_FVF);
                m_pDevice->SetVertexDeclaration(meshPair.first->m_pVertexDeclaration);

                /*std::sort(meshPair.second.begin(), meshPair.second.end(), [vecCameraPos](const CMatrix a, const CMatrix& b) -> bool {
                    return (vecCameraPos - a.GetPosition()).LengthSquared() > (vecCameraPos - b.GetPosition()).LengthSquared();
                });*/

                for (auto const& matrix : meshPair.second)
                {
                    matrix.first.GetBuffer(&floats[0]);
                    D3DMATRIX d3dModelMatrix;
                    int       i = 0;
                    for (int i1 = 0; i1 < 4; i1++)
                        for (int i2 = 0; i2 < 4; i2++)
                            d3dModelMatrix.m[i1][i2] = floats[i++];
                    m_pDevice->SetTransform(D3DTS_WORLD, &d3dModelMatrix);

                    matrix.second.GetBuffer(&floats[0]);
                    D3DMATRIX d3dTextureMatrix;
                           i = 0;
                    for (int i1 = 0; i1 < 4; i1++)
                        for (int i2 = 0; i2 < 4; i2++)
                            d3dTextureMatrix.m[i1][i2] = floats[i++];
                    m_pDevice->SetTransform(D3DTS_TEXTURE0, &d3dTextureMatrix);

                    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshPair.first->m_iIndicesCount, 0, meshPair.first->m_iFaceCount);
                }
            }
        }
        else if (CShaderInstance* pShaderInstance = DynamicCast<CShaderInstance>((CMaterialItem*)renderItem.first))
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
    }

    m_pDevice->SetTexture(0, nullptr);

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
    m_mapRenderList.clear();
}

void CAsset3DBatcher::AddAsset(std::unique_ptr<SRenderAssetItem> assetRenderItem)
{
    auto const& renderItem = m_mapRenderList.find(assetRenderItem->assetNode);

    if (renderItem == m_mapRenderList.end())
    {
        m_mapRenderList[assetRenderItem->assetNode] = std::vector<std::pair<CMatrix, CMatrix>>();
    }
    if (assetRenderItem->preserveUV)
    {
        CMatrix matrixTexture;
        matrixTexture.SetScale(-assetRenderItem->matrix.GetScale());
        m_mapRenderList[assetRenderItem->assetNode].push_back(std::pair{assetRenderItem->matrix, matrixTexture});
    }
    else
    {
        m_mapRenderList[assetRenderItem->assetNode].push_back(std::pair{assetRenderItem->matrix, CMatrix()});
    }
}

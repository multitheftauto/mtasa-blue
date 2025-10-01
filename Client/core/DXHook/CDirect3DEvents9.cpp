/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirect3DEvents9.cpp
 *  PURPOSE:     Handler implementations for Direct3D 9 events
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CRenderWare.h>
#define DECLARE_PROFILER_SECTION_CDirect3DEvents9
#include "profiler/SharedUtil.Profiler.h"
#include "CProxyDirect3DVertexBuffer.h"
#include "CProxyDirect3DIndexBuffer.h"
#include "CProxyDirect3DTexture.h"
#include "CAdditionalVertexStreamManager.h"
#include "CVertexStreamBoundingBoxManager.h"
#include "CProxyDirect3DVertexDeclaration.h"
#include "Graphics/CRenderItem.EffectTemplate.h"


std::atomic<bool>   g_bInMTAScene{false};
extern std::atomic<bool> g_bInGTAScene;
void ResetGTASceneState();

// Other variables
static uint                 ms_RequiredAnisotropicLevel = 1;
static EDiagnosticDebugType ms_DiagnosticDebug = EDiagnosticDebug::NONE;

// To reuse shader setups between calls to DrawIndexedPrimitive
CShaderItem* g_pActiveShader = NULL;

namespace
{
struct SResolvedShaderState
{
    CShaderInstance* pInstance = nullptr;
    CEffectWrap*     pEffectWrap = nullptr;
    ID3DXEffect*     pEffect = nullptr;
};

bool TryResolveShaderState(CShaderItem* pShaderItem, SResolvedShaderState& outState)
{
    if (!pShaderItem)
        return false;

    CShaderInstance* pInstance = pShaderItem->m_pShaderInstance;
    if (!pInstance)
        return false;

    CEffectWrap* pEffectWrap = pInstance->m_pEffectWrap;
    if (!pEffectWrap)
        return false;

    outState.pInstance = pInstance;
    outState.pEffectWrap = pEffectWrap;
    outState.pEffect = pEffectWrap->m_pD3DEffect;
    return true;
}
}

bool CDirect3DEvents9::IsDeviceOperational(IDirect3DDevice9* pDevice, bool* pbTemporarilyLost, HRESULT* pHrCooperativeLevel)
{
    if (pHrCooperativeLevel)
        *pHrCooperativeLevel = D3DERR_INVALIDCALL;
    if (pbTemporarilyLost)
        *pbTemporarilyLost = false;

    if (!pDevice)
    {
        if (pHrCooperativeLevel)
            *pHrCooperativeLevel = D3DERR_INVALIDCALL;
        return false;
    }

    const HRESULT hr = pDevice->TestCooperativeLevel();
    if (pHrCooperativeLevel)
        *pHrCooperativeLevel = hr;
    if (hr == D3D_OK)
        return true;

    if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
    {
        if (pbTemporarilyLost)
            *pbTemporarilyLost = true;
        return false;
    }

    const bool bSceneActive = g_bInMTAScene.load(std::memory_order_acquire) || g_bInGTAScene.load(std::memory_order_acquire);
    if (hr == D3DERR_INVALIDCALL && bSceneActive)
    {
        if (pHrCooperativeLevel)
            *pHrCooperativeLevel = D3D_OK;
        return true;
    }

    return false;
}

void CDirect3DEvents9::OnDirect3DDeviceCreate(IDirect3DDevice9* pDevice)
{
    WriteDebugEvent("CDirect3DEvents9::OnDirect3DDeviceCreate");

    // Create the GUI manager
    CCore::GetSingleton().InitGUI(pDevice);

    CAdditionalVertexStreamManager::GetSingleton()->OnDeviceCreate(pDevice);
    CVertexStreamBoundingBoxManager::GetSingleton()->OnDeviceCreate(pDevice);

    // Create all our fonts n stuff
    CGraphics::GetSingleton().OnDeviceCreate(pDevice);

    // Create the GUI elements
    CLocalGUI::GetSingleton().CreateObjects(pDevice);
}

void CDirect3DEvents9::OnDirect3DDeviceDestroy(IDirect3DDevice9* pDevice)
{
    WriteDebugEvent("CDirect3DEvents9::OnDirect3DDeviceDestroy");

    // Unload the current mod
    CModManager::GetSingleton().Unload();

    // Destroy the GUI elements
    CLocalGUI::GetSingleton().DestroyObjects();

    CAdditionalVertexStreamManager::DestroySingleton();
    CVertexStreamBoundingBoxManager::DestroySingleton();

    // De-initialize the GUI manager (destroying is done on Exit)
    CCore::GetSingleton().DeinitGUI();
}

void CDirect3DEvents9::OnBeginScene(IDirect3DDevice9* pDevice)
{
    // Notify core
    CCore::GetSingleton().DoPreFramePulse();
}

bool CDirect3DEvents9::OnEndScene(IDirect3DDevice9* pDevice)
{
    CloseActiveShader();
    return true;
}

void CDirect3DEvents9::OnInvalidate(IDirect3DDevice9* pDevice)
{
    WriteDebugEvent("CDirect3DEvents9::OnInvalidate");

    const HRESULT hrCooperativeLevel = pDevice->TestCooperativeLevel();
    const bool    bDeviceOperational = (hrCooperativeLevel == D3D_OK);
    const bool    bDeviceTemporarilyLost =
        (hrCooperativeLevel == D3DERR_DEVICELOST || hrCooperativeLevel == D3DERR_DEVICENOTRESET);

    if (!bDeviceOperational && !bDeviceTemporarilyLost)
        WriteDebugEvent(SString("OnInvalidate: unexpected cooperative level %08x", hrCooperativeLevel));

    const bool bInMTAScene = g_bInMTAScene.load(std::memory_order_acquire);
    const bool bInGTAScene = g_bInGTAScene.load(std::memory_order_acquire);
    const bool bInAnyScene = bInMTAScene || bInGTAScene;

    if (bDeviceOperational)
    {
        // Flush any pending operations before invalidation while the device still accepts work
        g_pCore->GetGraphics()->GetRenderItemManager()->SaveReadableDepthBuffer();
        g_pCore->GetGraphics()->GetRenderItemManager()->FlushNonAARenderTarget();

        // Ensure any in-progress effect passes are wrapped up before ending the scene
        CloseActiveShader();

        if (bInAnyScene)
        {
            const HRESULT hrEndScene = pDevice->EndScene();
            if (FAILED(hrEndScene))
                WriteDebugEvent(SString("OnInvalidate: EndScene failed: %08x", hrEndScene));
        }
    }
    else
    {
        CloseActiveShader(false);

        if (bInAnyScene)
        {
            WriteDebugEvent("OnInvalidate: device lost, skipping EndScene and pending GPU work");
        }
    }

    g_bInMTAScene.store(false, std::memory_order_release);
    ResetGTASceneState();

    // Invalidate the VMR9 Manager
    // CVideoManager::GetSingleton ().OnLostDevice ();

    // Notify gui
    CLocalGUI::GetSingleton().Invalidate();

    CGraphics::GetSingleton().OnDeviceInvalidate(pDevice);
}

void CDirect3DEvents9::OnRestore(IDirect3DDevice9* pDevice)
{
    WriteDebugEvent("CDirect3DEvents9::OnRestore");

    // Restore the VMR9 manager
    // CVideoManager::GetSingleton ().OnResetDevice ( pDevice );

    // Restore the GUI
    CLocalGUI::GetSingleton().Restore();

    // Restore the graphics manager
    CGraphics::GetSingleton().OnDeviceRestore(pDevice);

    CCore::GetSingleton().OnDeviceRestore();
}

void CDirect3DEvents9::OnPresent(IDirect3DDevice9* pDevice)
{
    TIMING_CHECKPOINT("+OnPresent1");
    // Start a new scene. This isn't ideal and is not really recommended by MSDN.
    // I tried disabling EndScene from GTA and just end it after this code ourselves
    // before present, but that caused graphical issues randomly with the sky.
    const HRESULT hrBeginScene = pDevice->BeginScene();
    if (FAILED(hrBeginScene))
    {
        WriteDebugEvent(SString("OnPresent: BeginScene failed: %08x", hrBeginScene));
    g_bInMTAScene.store(false, std::memory_order_release);
        return;
    }

    g_bInMTAScene.store(true, std::memory_order_release);

    // Reset samplers on first call
    static bool bDoneReset = false;
    if (!bDoneReset)
    {
        bDoneReset = true;
        for (uint i = 0; i < 16; i++)
        {
            pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        }
    }

    CGraphics::GetSingleton().EnteringMTARenderZone();

    // Tell everyone that the zbuffer will need clearing before use
    CGraphics::GetSingleton().OnZBufferModified();

    TIMING_CHECKPOINT("-OnPresent1");
    // Make a screenshot if needed (before GUI)
    CScreenShot::CheckForScreenShot(true);

    // Notify core
    CCore::GetSingleton().DoPostFramePulse();
    TIMING_CHECKPOINT("+OnPresent2");

    // Restore in case script forgets
    CGraphics::GetSingleton().GetRenderItemManager()->RestoreDefaultRenderTarget();

    // Must do this before GUI draw to prevent NVidia performance problems
    CGraphics::GetSingleton().GetRenderItemManager()->FlushNonAARenderTarget();

    bool bTookScreenShot = false;
    if (!CGraphics::GetSingleton().GetScreenGrabber()->IsQueueEmpty())
    {
        if (g_pCore->IsWebCoreLoaded())
            g_pCore->GetWebCore()->OnPreScreenshot();

        bTookScreenShot = true;
    }

    // Draw pre-GUI primitives
    CGraphics::GetSingleton().DrawPreGUIQueue();

    // Maybe grab screen for upload
    CGraphics::GetSingleton().GetScreenGrabber()->DoPulse();

    if (bTookScreenShot && g_pCore->IsWebCoreLoaded())
        g_pCore->GetWebCore()->OnPostScreenshot();

    // Draw the GUI
    CLocalGUI::GetSingleton().Draw();

    // Draw post-GUI primitives
    CGraphics::GetSingleton().DrawPostGUIQueue();

    // Redraw the mouse cursor so it will always be over other elements
    CLocalGUI::GetSingleton().DrawMouseCursor();

    CGraphics::GetSingleton().DidRenderScene();

    CGraphics::GetSingleton().LeavingMTARenderZone();

    // Finalize any lingering shader passes before wrapping the scene
    CloseActiveShader();

    // End the scene that we started.
    if (g_bInMTAScene.load(std::memory_order_acquire))
    {
        pDevice->EndScene();
    g_bInMTAScene.store(false, std::memory_order_release);
    }

    // Update incase settings changed
    int iAnisotropic;
    CVARS_GET("anisotropic", iAnisotropic);
    ms_RequiredAnisotropicLevel = 1 << iAnisotropic;
    ms_DiagnosticDebug = CCore::GetSingleton().GetDiagnosticDebug();
    CCore::GetSingleton().GetGUI()->SetBidiEnabled(ms_DiagnosticDebug != EDiagnosticDebug::BIDI_6778);

    // Make a screenshot if needed (after GUI)
    CScreenShot::CheckForScreenShot(false);

    TIMING_CHECKPOINT("-OnPresent2");

    TIMING_CHECKPOINT("");            // End of frame for profiler
}

#define SAVE_RENDERSTATE_AND_SET( reg, value ) \
    const DWORD dwSaved_##reg = g_pDeviceState->RenderState.reg; \
    const bool bSet_##reg = ( dwSaved_##reg != value ); \
    if ( bSet_##reg ) \
        pDevice->SetRenderState ( D3DRS_##reg, value )

#define RESTORE_RENDERSTATE( reg ) \
    if ( bSet_##reg ) \
        pDevice->SetRenderState ( D3DRS_##reg, dwSaved_##reg )

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::OnDrawPrimitive
//
// May change render states for custom renderings
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::OnDrawPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::GRAPHICS_6734)
        return pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);

    CloseActiveShader();

    // Any shader for this texture ?
    SShaderItemLayers* pLayers =
        CGraphics::GetSingleton().GetRenderItemManager()->GetAppliedShaderForD3DData((CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture);

    // Skip draw if there is a vertex shader conflict
    if (pLayers && pLayers->bUsesVertexShader && g_pDeviceState->VertexShader)
        return D3D_OK;

    if (!pLayers)
    {
        // No shaders for this texture
        return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
    }
    else
    {
        // Yes base shader
        DrawPrimitiveShader(pDevice, PrimitiveType, StartVertex, PrimitiveCount, pLayers->pBase, false);

        // Draw each layer
        if (!pLayers->layerList.empty())
        {
            float fSlopeDepthBias = -0.02f;
            float fDepthBias = -0.00002f;
            SAVE_RENDERSTATE_AND_SET(SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeDepthBias);
            SAVE_RENDERSTATE_AND_SET(DEPTHBIAS, *(DWORD*)&fDepthBias);
            SAVE_RENDERSTATE_AND_SET(ALPHABLENDENABLE, TRUE);
            SAVE_RENDERSTATE_AND_SET(SRCBLEND, D3DBLEND_SRCALPHA);
            SAVE_RENDERSTATE_AND_SET(DESTBLEND, D3DBLEND_INVSRCALPHA);
            SAVE_RENDERSTATE_AND_SET(ALPHATESTENABLE, TRUE);
            SAVE_RENDERSTATE_AND_SET(ALPHAREF, 0x01);
            SAVE_RENDERSTATE_AND_SET(ALPHAFUNC, D3DCMP_GREATER);
            SAVE_RENDERSTATE_AND_SET(ZWRITEENABLE, FALSE);
            SAVE_RENDERSTATE_AND_SET(ZFUNC, D3DCMP_LESSEQUAL);

            for (uint i = 0; i < pLayers->layerList.size(); i++)
            {
                DrawPrimitiveShader(pDevice, PrimitiveType, StartVertex, PrimitiveCount, pLayers->layerList[i], true);
            }

            RESTORE_RENDERSTATE(SLOPESCALEDEPTHBIAS);
            RESTORE_RENDERSTATE(DEPTHBIAS);
            RESTORE_RENDERSTATE(ALPHABLENDENABLE);
            RESTORE_RENDERSTATE(SRCBLEND);
            RESTORE_RENDERSTATE(DESTBLEND);
            RESTORE_RENDERSTATE(ALPHATESTENABLE);
            RESTORE_RENDERSTATE(ALPHAREF);
            RESTORE_RENDERSTATE(ALPHAFUNC);
            RESTORE_RENDERSTATE(ZWRITEENABLE);
            RESTORE_RENDERSTATE(ZFUNC);
        }

        return D3D_OK;
    }
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::DrawPrimitiveShader
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawPrimitiveShader(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount,
                                              CShaderItem* pShaderItem, bool bIsLayer)
{
    if (!pShaderItem)
    {
        // No shader for this texture
        if (!bIsLayer)
            return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
    }
    else
    {
        // Yes shader for this texture
        SResolvedShaderState shaderState;
        if (!TryResolveShaderState(pShaderItem, shaderState) || !shaderState.pEffect)
        {
            if (!bIsLayer)
                return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
            return D3D_OK;
        }

        CShaderInstance* pShaderInstance = shaderState.pInstance;
        CEffectWrap*     pEffectWrap = shaderState.pEffectWrap;
        ID3DXEffect*     pD3DEffect = shaderState.pEffect;

        // Apply custom parameters
        pShaderInstance->ApplyShaderParameters();
        // Apply common parameters
        pEffectWrap->ApplyCommonHandles();
        // Apply mapped parameters
        pEffectWrap->ApplyMappedHandles();

        // Remember vertex shader if original draw was using it
        IDirect3DVertexShader9* pOriginalVertexShader = NULL;
        pDevice->GetVertexShader(&pOriginalVertexShader);

        // Do shader passes
        bool bEffectDeviceTemporarilyLost = false;
        bool bEffectDeviceOperational = true;
        IDirect3DDevice9* pEffectDevice = nullptr;
        if (SUCCEEDED(pD3DEffect->GetDevice(&pEffectDevice)) && pEffectDevice)
        {
            bEffectDeviceOperational = IsDeviceOperational(pEffectDevice, &bEffectDeviceTemporarilyLost);
            SAFE_RELEASE(pEffectDevice);
        }

        if (!bEffectDeviceOperational)
        {
            SAFE_RELEASE(pOriginalVertexShader);
            if (!bIsLayer && !bEffectDeviceTemporarilyLost)
                return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
            return D3D_OK;
        }

        DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;            // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint  uiNumPasses = 0;
    HRESULT hrBegin = pEffectWrap->Begin(&uiNumPasses, dwFlags);
        if (FAILED(hrBegin) || uiNumPasses == 0)
        {
            if (FAILED(hrBegin) && hrBegin != D3DERR_DEVICELOST && hrBegin != D3DERR_DEVICENOTRESET)
                WriteDebugEvent(SString("DrawPrimitiveShader: Begin failed %08x", hrBegin));

            SAFE_RELEASE(pOriginalVertexShader);
            if (!bIsLayer && hrBegin != D3DERR_DEVICELOST && hrBegin != D3DERR_DEVICENOTRESET)
                return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
            return D3D_OK;
        }

        bool bCompletedAnyPass = false;
        bool bEncounteredDeviceLoss = false;
        for (uint uiPass = 0; uiPass < uiNumPasses; uiPass++)
        {
            HRESULT hrBeginPass = pD3DEffect->BeginPass(uiPass);
            if (FAILED(hrBeginPass))
            {
                if (hrBeginPass != D3DERR_DEVICELOST && hrBeginPass != D3DERR_DEVICENOTRESET)
                    WriteDebugEvent(SString("DrawPrimitiveShader: BeginPass %u failed %08x", uiPass, hrBeginPass));
                else
                    bEncounteredDeviceLoss = true;
                break;
            }

            // Apply original vertex shader if original draw was using it (i.e. for ped animation)
            if (pOriginalVertexShader)
                pDevice->SetVertexShader(pOriginalVertexShader);

            HRESULT hrDraw = DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
            if (hrDraw == D3DERR_DEVICELOST || hrDraw == D3DERR_DEVICENOTRESET)
                bEncounteredDeviceLoss = true;

            HRESULT hrEndPass = pD3DEffect->EndPass();
            if (FAILED(hrEndPass))
            {
                if (hrEndPass != D3DERR_DEVICELOST && hrEndPass != D3DERR_DEVICENOTRESET)
                    WriteDebugEvent(SString("DrawPrimitiveShader: EndPass %u failed %08x", uiPass, hrEndPass));
                else
                    bEncounteredDeviceLoss = true;
                break;
            }

            if (SUCCEEDED(hrDraw))
                bCompletedAnyPass = true;
        }

    HRESULT hrEnd = pEffectWrap->End(bEffectDeviceOperational && !bEncounteredDeviceLoss);
        if (FAILED(hrEnd) && hrEnd != D3DERR_DEVICELOST && hrEnd != D3DERR_DEVICENOTRESET)
            WriteDebugEvent(SString("DrawPrimitiveShader: End failed %08x", hrEnd));

        // If we didn't get the effect to save the shader state, clear some things here
        if (dwFlags & D3DXFX_DONOTSAVESHADERSTATE)
        {
            pDevice->SetVertexShader(pOriginalVertexShader);
            pDevice->SetPixelShader(NULL);
        }

        SAFE_RELEASE(pOriginalVertexShader);

        if (!bCompletedAnyPass && !bIsLayer && !bEncounteredDeviceLoss)
            return DrawPrimitiveGuarded(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
    }

    return D3D_OK;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::OnDrawIndexedPrimitive
//
// May change render states for custom renderings
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::OnDrawIndexedPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex,
                                                 UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::GRAPHICS_6734)
        return pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

    // Apply anisotropic filtering if required
    if (ms_RequiredAnisotropicLevel > 1)
    {
        pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
        pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        if (ms_RequiredAnisotropicLevel > g_pDeviceState->SamplerState[0].MAXANISOTROPY)
            pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, ms_RequiredAnisotropicLevel);
    }

    // See if we can continue using the active shader
    if (g_pActiveShader)
    {
        return DrawIndexedPrimitiveShader(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, g_pActiveShader, false,
                                          false);
    }

    // Any shader for this texture ?
    SShaderItemLayers* pLayers =
        CGraphics::GetSingleton().GetRenderItemManager()->GetAppliedShaderForD3DData((CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture);

    // Skip draw if there is a vertex shader conflict
    if (pLayers && pLayers->bUsesVertexShader && g_pDeviceState->VertexShader)
        return D3D_OK;

    if (!pLayers)
    {
        // No shaders for this texture
        return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }
    else
    {
        // Draw base shader
        DrawIndexedPrimitiveShader(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, pLayers->pBase, false,
                                   pLayers->layerList.empty());

        // Draw each layer
        if (!pLayers->layerList.empty())
        {
            float fSlopeDepthBias = -0.02f;
            float fDepthBias = -0.00002f;
            SAVE_RENDERSTATE_AND_SET(SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeDepthBias);
            SAVE_RENDERSTATE_AND_SET(DEPTHBIAS, *(DWORD*)&fDepthBias);
            SAVE_RENDERSTATE_AND_SET(ALPHABLENDENABLE, TRUE);
            SAVE_RENDERSTATE_AND_SET(SRCBLEND, D3DBLEND_SRCALPHA);
            SAVE_RENDERSTATE_AND_SET(DESTBLEND, D3DBLEND_INVSRCALPHA);
            SAVE_RENDERSTATE_AND_SET(ALPHATESTENABLE, TRUE);
            SAVE_RENDERSTATE_AND_SET(ALPHAREF, 0x01);
            SAVE_RENDERSTATE_AND_SET(ALPHAFUNC, D3DCMP_GREATER);
            SAVE_RENDERSTATE_AND_SET(ZWRITEENABLE, FALSE);
            SAVE_RENDERSTATE_AND_SET(ZFUNC, D3DCMP_LESSEQUAL);

            for (uint i = 0; i < pLayers->layerList.size(); i++)
            {
                DrawIndexedPrimitiveShader(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, pLayers->layerList[i],
                                           true, false);
            }

            RESTORE_RENDERSTATE(SLOPESCALEDEPTHBIAS);
            RESTORE_RENDERSTATE(DEPTHBIAS);
            RESTORE_RENDERSTATE(ALPHABLENDENABLE);
            RESTORE_RENDERSTATE(SRCBLEND);
            RESTORE_RENDERSTATE(DESTBLEND);
            RESTORE_RENDERSTATE(ALPHATESTENABLE);
            RESTORE_RENDERSTATE(ALPHAREF);
            RESTORE_RENDERSTATE(ALPHAFUNC);
            RESTORE_RENDERSTATE(ZWRITEENABLE);
            RESTORE_RENDERSTATE(ZFUNC);
        }

        return D3D_OK;
    }
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::DrawIndexedPrimitiveShader
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawIndexedPrimitiveShader(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex,
                                                     UINT NumVertices, UINT startIndex, UINT primCount, CShaderItem* pShaderItem, bool bIsLayer,
                                                     bool bCanBecomeActiveShader)
{
    if (pShaderItem && pShaderItem->m_fMaxDistanceSq > 0)
    {
        // If shader has a max distance, check this vertex range bounding box
        float fDistanceSq = CVertexStreamBoundingBoxManager::GetSingleton()->GetDistanceSqToGeometry(PrimitiveType, BaseVertexIndex, MinVertexIndex,
                                                                                                     NumVertices, startIndex, primCount);

        // If distance was obtained and vertices are too far away, don't apply the shader
        if (fDistanceSq > 0 && fDistanceSq > pShaderItem->m_fMaxDistanceSq)
            pShaderItem = NULL;
    }

    if (!pShaderItem)
    {
        CloseActiveShader();

        // No shader for this texture
        if (!bIsLayer)
            return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }
    else
    {
        // See if we should use the previously setup shader
        if (g_pActiveShader)
        {
            SResolvedShaderState activeState;
            if (!TryResolveShaderState(g_pActiveShader, activeState) || !activeState.pEffect)
            {
                CloseActiveShader(false);
                if (!bIsLayer)
                    return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
                return D3D_OK;
            }

            dassert(pShaderItem == g_pActiveShader);
            g_pDeviceState->FrameStats.iNumShadersReuseSetup++;

            // Transfer any state changes to the active shader, but ensure the device still accepts work
            CShaderInstance* pShaderInstance = activeState.pInstance;
            CEffectWrap*     pActiveEffectWrap = activeState.pEffectWrap;
            ID3DXEffect*     pActiveEffect = activeState.pEffect;

            bool bDeviceTemporarilyLost = false;
            bool bDeviceOperational = true;
            IDirect3DDevice9* pEffectDevice = nullptr;
            if (SUCCEEDED(pActiveEffect->GetDevice(&pEffectDevice)) && pEffectDevice)
            {
                bDeviceOperational = IsDeviceOperational(pEffectDevice, &bDeviceTemporarilyLost);
                SAFE_RELEASE(pEffectDevice);
            }

            if (!bDeviceOperational)
            {
                CloseActiveShader(false);
                return D3D_OK;
            }

            bool bChanged = pActiveEffectWrap->ApplyCommonHandles();
            bChanged |= pActiveEffectWrap->ApplyMappedHandles();
            if (bChanged)
            {
                HRESULT hrCommit = pActiveEffect->CommitChanges();
                if (FAILED(hrCommit))
                {
                    if (hrCommit != D3DERR_DEVICELOST && hrCommit != D3DERR_DEVICENOTRESET)
                        WriteDebugEvent(SString("DrawIndexedPrimitiveShader: CommitChanges failed %08x", hrCommit));
                    CloseActiveShader(false);
                    return D3D_OK;
                }
            }

            return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
        }
        g_pDeviceState->FrameStats.iNumShadersFullSetup++;

        // Yes shader for this texture
        SResolvedShaderState shaderState;
        if (!TryResolveShaderState(pShaderItem, shaderState) || !shaderState.pEffect)
        {
            if (!bIsLayer)
                return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
            return D3D_OK;
        }

        CShaderInstance* pShaderInstance = shaderState.pInstance;
        CEffectWrap*     pEffectWrap = shaderState.pEffectWrap;
        ID3DXEffect*     pD3DEffect = shaderState.pEffect;

        // Add normal stream if shader wants it
        CAdditionalVertexStreamManager* pAdditionalStreamManager = CAdditionalVertexStreamManager::GetExistingSingleton();
        if (pEffectWrap->m_pEffectTemplate->m_bRequiresNormals && pAdditionalStreamManager)
        {
            // Find/create/set additional vertex stream
            pAdditionalStreamManager->MaybeSetAdditionalVertexStream(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
        }

        // Apply custom parameters
        pShaderInstance->ApplyShaderParameters();
        // Apply common parameters
        pEffectWrap->ApplyCommonHandles();
        // Apply mapped parameters
        pEffectWrap->ApplyMappedHandles();

        // Remember vertex shader if original draw was using it
        IDirect3DVertexShader9* pOriginalVertexShader = NULL;
        pDevice->GetVertexShader(&pOriginalVertexShader);

        // Do shader passes
        bool bEffectDeviceTemporarilyLost = false;
        bool bEffectDeviceOperational = true;
        IDirect3DDevice9* pEffectDevice = nullptr;
        if (SUCCEEDED(pD3DEffect->GetDevice(&pEffectDevice)) && pEffectDevice)
        {
            bEffectDeviceOperational = IsDeviceOperational(pEffectDevice, &bEffectDeviceTemporarilyLost);
            SAFE_RELEASE(pEffectDevice);
        }

        if (!bEffectDeviceOperational)
        {
            SAFE_RELEASE(pOriginalVertexShader);
            if (pAdditionalStreamManager)
                pAdditionalStreamManager->MaybeUnsetAdditionalVertexStream();
            if (!bEffectDeviceTemporarilyLost && !bIsLayer)
                return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
            return D3D_OK;
        }

        DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;            // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint  uiNumPasses = 0;
    HRESULT hrBegin = pEffectWrap->Begin(&uiNumPasses, dwFlags);
        if (FAILED(hrBegin) || uiNumPasses == 0)
        {
            if (FAILED(hrBegin) && hrBegin != D3DERR_DEVICELOST && hrBegin != D3DERR_DEVICENOTRESET)
                WriteDebugEvent(SString("DrawIndexedPrimitiveShader: Begin failed %08x", hrBegin));

            SAFE_RELEASE(pOriginalVertexShader);
            if (pAdditionalStreamManager)
                pAdditionalStreamManager->MaybeUnsetAdditionalVertexStream();

            if (hrBegin != D3DERR_DEVICELOST && hrBegin != D3DERR_DEVICENOTRESET && !bIsLayer)
                return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
            return D3D_OK;
        }

        bool bCompletedAnyPass = false;
        bool bEncounteredDeviceLoss = false;
        for (uint uiPass = 0; uiPass < uiNumPasses; uiPass++)
        {
            HRESULT hrBeginPass = pD3DEffect->BeginPass(uiPass);
            if (FAILED(hrBeginPass))
            {
                if (hrBeginPass != D3DERR_DEVICELOST && hrBeginPass != D3DERR_DEVICENOTRESET)
                    WriteDebugEvent(SString("DrawIndexedPrimitiveShader: BeginPass %u failed %08x", uiPass, hrBeginPass));
                else
                    bEncounteredDeviceLoss = true;
                break;
            }

            // Apply original vertex shader if original draw was using it (i.e. for ped animation)
            if (pOriginalVertexShader)
                pDevice->SetVertexShader(pOriginalVertexShader);

            HRESULT hrDraw = DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
            if (hrDraw == D3DERR_DEVICELOST || hrDraw == D3DERR_DEVICENOTRESET)
                bEncounteredDeviceLoss = true;

            if (uiNumPasses == 1 && bCanBecomeActiveShader && pOriginalVertexShader == NULL && g_pCore->IsRenderingGrass() && SUCCEEDED(hrDraw))
            {
                // Make this the active shader for possible reuse
                dassert(dwFlags == D3DXFX_DONOTSAVESHADERSTATE);
                pShaderItem->AddRef();
                g_pActiveShader = pShaderItem;
                SAFE_RELEASE(pOriginalVertexShader);
                return D3D_OK;
            }

            HRESULT hrEndPass = pD3DEffect->EndPass();
            if (FAILED(hrEndPass))
            {
                if (hrEndPass != D3DERR_DEVICELOST && hrEndPass != D3DERR_DEVICENOTRESET)
                    WriteDebugEvent(SString("DrawIndexedPrimitiveShader: EndPass %u failed %08x", uiPass, hrEndPass));
                else
                    bEncounteredDeviceLoss = true;
                break;
            }

            if (SUCCEEDED(hrDraw))
                bCompletedAnyPass = true;
        }

    HRESULT hrEnd = pEffectWrap->End(bEffectDeviceOperational && !bEncounteredDeviceLoss);
        if (FAILED(hrEnd) && hrEnd != D3DERR_DEVICELOST && hrEnd != D3DERR_DEVICENOTRESET)
            WriteDebugEvent(SString("DrawIndexedPrimitiveShader: End failed %08x", hrEnd));

        // If we didn't get the effect to save the shader state, clear some things here
        if (dwFlags & D3DXFX_DONOTSAVESHADERSTATE)
        {
            pDevice->SetVertexShader(pOriginalVertexShader);
            pDevice->SetPixelShader(NULL);
        }

        // Unset additional vertex stream
        if (pAdditionalStreamManager)
            pAdditionalStreamManager->MaybeUnsetAdditionalVertexStream();

        SAFE_RELEASE(pOriginalVertexShader);

        if (!bCompletedAnyPass && !bEncounteredDeviceLoss && !bIsLayer)
            return DrawIndexedPrimitiveGuarded(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }

    return D3D_OK;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CloseActiveShader
//
// Finish the active shader if there is one
//
/////////////////////////////////////////////////////////////
void CDirect3DEvents9::CloseActiveShader(bool bDeviceOperational)
{
    CShaderItem* pShaderItem = g_pActiveShader;
    g_pActiveShader = nullptr;
    if (!pShaderItem)
        return;

    SResolvedShaderState shaderState;
    bool                 bHasShaderState = TryResolveShaderState(pShaderItem, shaderState);

    ID3DXEffect* pD3DEffect = bHasShaderState ? shaderState.pEffect : nullptr;
    IDirect3DDevice9* pDevice = g_pGraphics ? g_pGraphics->GetDevice() : nullptr;

    bool bAllowDeviceWork = bDeviceOperational;
    if (pDevice)
    {
        bool bDeviceTemporarilyLost = false;
        if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost))
            bAllowDeviceWork = !bDeviceTemporarilyLost && bDeviceOperational;
    }

    if (pD3DEffect)
    {
        HRESULT hrEndPass = pD3DEffect->EndPass();
        if (FAILED(hrEndPass) && hrEndPass != D3DERR_DEVICELOST && hrEndPass != D3DERR_DEVICENOTRESET)
            WriteDebugEvent(SString("CloseActiveShader: EndPass failed: %08x", hrEndPass));
    }

    // When the device is lost we intentionally skip touching the GPU beyond the required End call; the effect will be reset later.
    if (bHasShaderState)
        shaderState.pEffectWrap->End(bAllowDeviceWork);

    if (CAdditionalVertexStreamManager* pAdditionalStreamManager = CAdditionalVertexStreamManager::GetExistingSingleton())
        pAdditionalStreamManager->MaybeUnsetAdditionalVertexStream();

    if (bAllowDeviceWork && pDevice)
    {
        // We didn't get the effect to save the shader state, clear some things here
        pDevice->SetVertexShader(nullptr);
        pDevice->SetPixelShader(nullptr);
    }

    pShaderItem->Release();
}

/////////////////////////////////////////////////////////////
//
// AreVertexStreamsBigEnough
//
// Occasionally, GTA tries to draw water/clouds/something with a vertex buffer that is
// too small (which causes problems for some graphics drivers).
//
// This function checks the sizes are valid
//
/////////////////////////////////////////////////////////////
bool AreVertexStreamsBigEnough(IDirect3DDevice9* pDevice, uint viMinBased, uint viMaxBased)
{
    // Check each stream used
    for (uint i = 0; i < NUMELMS(g_pDeviceState->VertexDeclState.bUsesStreamAtIndex); i++)
    {
        if (g_pDeviceState->VertexDeclState.bUsesStreamAtIndex[i])
        {
            IDirect3DVertexBuffer9* pStreamData = NULL;
            UINT                    StreamOffset;
            UINT                    StreamStride;
            pDevice->GetStreamSource(i, &pStreamData, &StreamOffset, &StreamStride);

            if (pStreamData)
            {
                D3DVERTEXBUFFER_DESC VertexBufferDesc;
                pStreamData->GetDesc(&VertexBufferDesc);
                SAFE_RELEASE(pStreamData);

                uint ReadOffsetStart = viMinBased * StreamStride + StreamOffset;
                uint ReadOffsetSize = (viMaxBased - viMinBased) * StreamStride;

                uint MinSizeVertexBufferShouldBe = ReadOffsetStart + ReadOffsetSize;

                // Check vertex buffer is big enough to do the draw
                if (VertexBufferDesc.Size < MinSizeVertexBufferShouldBe)
                    return false;
            }
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////
//
// FilterException
//
// Check if exception should be handled by us
//
/////////////////////////////////////////////////////////////
thread_local uint uiLastExceptionCode = 0;
int             FilterException(uint exceptionCode)
{
    uiLastExceptionCode = exceptionCode;
    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_EXECUTE_HANDLER;
    if (exceptionCode == 0xE06D7363)
    {
        WriteDebugEvent("FilterException: caught Microsoft C++ exception");
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

namespace
{
void WriteDebugEventFormatted(const char* format, HRESULT value)
{
    char buffer[160];
    _snprintf_s(buffer, _countof(buffer), _TRUNCATE, format, value);
    WriteDebugEvent(buffer);
}

HRESULT CallSetRenderTargetWithGuard(IDirect3DDevice9* pDevice, DWORD renderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    HRESULT hr = D3D_OK;
    __try
    {
        hr = pDevice->SetRenderTarget(renderTargetIndex, pRenderTarget);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 15 * 1000000);
    }
    return hr;
}

HRESULT CallSetDepthStencilSurfaceWithGuard(IDirect3DDevice9* pDevice, IDirect3DSurface9* pNewZStencil)
{
    HRESULT hr = D3D_OK;
    __try
    {
        hr = pDevice->SetDepthStencilSurface(pNewZStencil);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 16 * 1000000);
    }
    return hr;
}

HRESULT CallCreateAdditionalSwapChainWithGuard(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
    HRESULT hr = D3D_OK;
    __try
    {
        hr = pDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 17 * 1000000);
    }
    return hr;
}
}

/////////////////////////////////////////////////////////////
//
// DrawPrimitiveGuarded
//
// Catch access violations
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawPrimitiveGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);

    bool     bDeviceTemporarilyLost = false;
    HRESULT  hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    if (PrimitiveType == D3DPT_TRIANGLELIST || PrimitiveType == D3DPT_TRIANGLESTRIP)
    {
        UINT NumVertices = PrimitiveCount + 2;
        if (PrimitiveType == D3DPT_TRIANGLELIST)
            NumVertices = PrimitiveCount * 3;

        uint viMinBased = StartVertex;
        uint viMaxBased = NumVertices + StartVertex;

    if (!AreVertexStreamsBigEnough(pDevice, viMinBased, viMaxBased))
            return hr;
    }

    __try
    {
        hr = pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 1 * 1000000);
    }
    return hr;
}

/////////////////////////////////////////////////////////////
//
// DrawIndexedPrimitiveGuarded
//
// Catch access violations
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawIndexedPrimitiveGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex,
                                                      UINT NumVertices, UINT startIndex, UINT primCount)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

    bool     bDeviceTemporarilyLost = false;
    HRESULT  hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    uint viMinBased = MinVertexIndex + BaseVertexIndex;
    uint viMaxBased = MinVertexIndex + NumVertices + BaseVertexIndex;

    if (!AreVertexStreamsBigEnough(pDevice, viMinBased, viMaxBased))
        return hr;

    __try
    {
        hr = pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 2 * 1000000);
    }
    return hr;
}

/////////////////////////////////////////////////////////////
//
// DrawPrimitiveUPGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawPrimitiveUPGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
                                                 CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 3 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// DrawIndexedPrimitiveUPGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawIndexedPrimitiveUPGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices,
                                                        UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat,
                                                        CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat,
                                               pVertexStreamZeroData, VertexStreamZeroStride);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat,
                                             pVertexStreamZeroData, VertexStreamZeroStride);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 4 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// DrawRectPatchGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawRectPatchGuarded(IDirect3DDevice9* pDevice, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 6 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// DrawTriPatchGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawTriPatchGuarded(IDirect3DDevice9* pDevice, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 7 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// ProcessVerticesGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::ProcessVerticesGuarded(IDirect3DDevice9* pDevice, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount,
                                                 IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 8 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// ClearGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::ClearGuarded(IDirect3DDevice9* pDevice, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 9 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// ColorFillGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::ColorFillGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->ColorFill(pSurface, pRect, color);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->ColorFill(pSurface, pRect, color);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 10 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// UpdateSurfaceGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::UpdateSurfaceGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect,
                                               IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 11 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// UpdateTextureGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::UpdateTextureGuarded(IDirect3DDevice9* pDevice, IDirect3DBaseTexture9* pSourceTexture,
                                               IDirect3DBaseTexture9* pDestinationTexture)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->UpdateTexture(pSourceTexture, pDestinationTexture);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->UpdateTexture(pSourceTexture, pDestinationTexture);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 12 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// GetRenderTargetDataGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::GetRenderTargetDataGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pRenderTarget,
                                                     IDirect3DSurface9* pDestSurface)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->GetRenderTargetData(pRenderTarget, pDestSurface);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->GetRenderTargetData(pRenderTarget, pDestSurface);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 13 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// GetFrontBufferDataGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::GetFrontBufferDataGuarded(IDirect3DDevice9* pDevice, UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->GetFrontBufferData(iSwapChain, pDestSurface);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->GetFrontBufferData(iSwapChain, pDestSurface);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 14 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// SetRenderTargetGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::SetRenderTargetGuarded(IDirect3DDevice9* pDevice, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            WriteDebugEventFormatted("CDirect3DEvents9::SetRenderTargetGuarded skipped due to device state: %08x", hrCooperativeLevel);
        else if (hrCooperativeLevel != D3D_OK)
            WriteDebugEventFormatted("CDirect3DEvents9::SetRenderTargetGuarded unexpected cooperative level: %08x", hrCooperativeLevel);
        else
            WriteDebugEvent("CDirect3DEvents9::SetRenderTargetGuarded invalid device state");

        return (hrCooperativeLevel != D3D_OK) ? hrCooperativeLevel : D3DERR_INVALIDCALL;
    }

    return CallSetRenderTargetWithGuard(pDevice, RenderTargetIndex, pRenderTarget);
}

/////////////////////////////////////////////////////////////
//
// SetDepthStencilSurfaceGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::SetDepthStencilSurfaceGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pNewZStencil)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->SetDepthStencilSurface(pNewZStencil);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            WriteDebugEventFormatted("CDirect3DEvents9::SetDepthStencilSurfaceGuarded skipped due to device state: %08x", hrCooperativeLevel);
        else if (hrCooperativeLevel != D3D_OK)
            WriteDebugEventFormatted("CDirect3DEvents9::SetDepthStencilSurfaceGuarded unexpected cooperative level: %08x", hrCooperativeLevel);
        else
            WriteDebugEvent("CDirect3DEvents9::SetDepthStencilSurfaceGuarded invalid device state");

        return (hrCooperativeLevel != D3D_OK) ? hrCooperativeLevel : D3DERR_INVALIDCALL;
    }

    return CallSetDepthStencilSurfaceWithGuard(pDevice, pNewZStencil);
}

/////////////////////////////////////////////////////////////
//
// CreateAdditionalSwapChainGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateAdditionalSwapChainGuarded(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                                           IDirect3DSwapChain9** pSwapChain)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            WriteDebugEventFormatted("CDirect3DEvents9::CreateAdditionalSwapChainGuarded skipped due to device state: %08x", hrCooperativeLevel);
        else if (hrCooperativeLevel != D3D_OK)
            WriteDebugEventFormatted("CDirect3DEvents9::CreateAdditionalSwapChainGuarded unexpected cooperative level: %08x", hrCooperativeLevel);
        else
            WriteDebugEvent("CDirect3DEvents9::CreateAdditionalSwapChainGuarded invalid device state");

        return (hrCooperativeLevel != D3D_OK) ? hrCooperativeLevel : D3DERR_INVALIDCALL;
    }

    return CallCreateAdditionalSwapChainWithGuard(pDevice, pPresentationParameters, pSwapChain);
}

/////////////////////////////////////////////////////////////
//
// CreateVolumeTextureGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateVolumeTextureGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage,
                                                     D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture,
                                                     HANDLE* pSharedHandle)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 18 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// CreateCubeTextureGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateCubeTextureGuarded(IDirect3DDevice9* pDevice, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format,
                                                   D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 19 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// CreateRenderTargetGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateRenderTargetGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format,
                                                    D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable,
                                                    IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 20 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// CreateDepthStencilSurfaceGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateDepthStencilSurfaceGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format,
                                                           D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard,
                                                           IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 21 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// CreateOffscreenPlainSurfaceGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateOffscreenPlainSurfaceGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool,
                                                             IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    if (ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732)
        return pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);

    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 22 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// PresentGuarded
//
// Catch access violations and device-loss states
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::PresentGuarded(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
                                         CONST RGNDATA* pDirtyRegion)
{
    bool    bDeviceTemporarilyLost = false;
    HRESULT hrCooperativeLevel = D3DERR_INVALIDCALL;
    if (!IsDeviceOperational(pDevice, &bDeviceTemporarilyLost, &hrCooperativeLevel))
    {
        if (bDeviceTemporarilyLost)
            return hrCooperativeLevel;

        if (hrCooperativeLevel != D3D_OK)
            return hrCooperativeLevel;

        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;

    __try
    {
        hr = pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    }
    __except (FilterException(GetExceptionCode()))
    {
        CCore::GetSingleton().OnCrashAverted((uiLastExceptionCode & 0xFFFF) + 5 * 1000000);
    }

    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateVertexBuffer
//
// Creates a proxy object for the new vertex buffer
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateVertexBuffer(IDirect3DDevice9* pDevice, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
                                             IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
    HRESULT hr;

    // We may have to look at the contents to generate normals (Not needed for dynamic buffers)
    if ((Usage & D3DUSAGE_DYNAMIC) == 0)
        Usage &= -1 - D3DUSAGE_WRITEONLY;

    for (uint i = 0; i < 2; i++)
    {
        hr = pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
        if (SUCCEEDED(hr))
            break;

        if (hr != D3DERR_OUTOFVIDEOMEMORY || i > 0)
        {
            SString strMessage("CreateVertexBuffer fail: hr:%x Length:%x Usage:%x FVF:%x Pool:%x", hr, Length, Usage, FVF, Pool);
            WriteDebugEvent(strMessage);
            AddReportLog(8610, strMessage);
            CCore::GetSingleton().LogEvent(610, "CreateVertexBuffer", "", strMessage);
            return hr;
        }

        pDevice->EvictManagedResources();
    }

    // Create proxy
    *ppVertexBuffer = new CProxyDirect3DVertexBuffer(pDevice, *ppVertexBuffer, Length, Usage, FVF, Pool);
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateIndexBuffer
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateIndexBuffer(IDirect3DDevice9* pDevice, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                            IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
    HRESULT hr;

    for (uint i = 0; i < 2; i++)
    {
        hr = pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
        if (SUCCEEDED(hr))
            break;

        if (hr != D3DERR_OUTOFVIDEOMEMORY || i > 0)
        {
            SString strMessage("CreateIndexBuffer fail: hr:%x Length:%x Usage:%x Format:%x Pool:%x", hr, Length, Usage, Format, Pool);
            WriteDebugEvent(strMessage);
            AddReportLog(8611, strMessage);
            CCore::GetSingleton().LogEvent(611, "CreateIndexBuffer", "", strMessage);
            return hr;
        }

        pDevice->EvictManagedResources();
    }

    // Create proxy so we can track when it's finished with
    *ppIndexBuffer = new CProxyDirect3DIndexBuffer(pDevice, *ppIndexBuffer, Length, Usage, Format, Pool);
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateTexture
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateTexture(IDirect3DDevice9* pDevice, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                        IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
    HRESULT hr;

    for (uint i = 0; i < 2; i++)
    {
        hr = pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
        if (SUCCEEDED(hr))
            break;

        if (hr != D3DERR_OUTOFVIDEOMEMORY || i > 0)
        {
            SString strMessage("CreateTexture fail: hr:%x W:%x H:%x L:%x Usage:%x Format:%x Pool:%x", hr, Width, Height, Levels, Usage, Format, Pool);
            WriteDebugEvent(strMessage);
            AddReportLog(8612, strMessage);
            CCore::GetSingleton().LogEvent(612, "CreateTexture", "", strMessage);
            return hr;
        }

        pDevice->EvictManagedResources();
    }

    // Create proxy so we can track when it's finished with
    *ppTexture = new CProxyDirect3DTexture(pDevice, *ppTexture, Width, Height, Levels, Usage, Format, Pool);
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetStreamSource
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
IDirect3DVertexBuffer9* CDirect3DEvents9::GetRealVertexBuffer(IDirect3DVertexBuffer9* pStreamData)
{
    if (pStreamData)
    {
        // See if it's a proxy
        CProxyDirect3DVertexBuffer* pProxy = NULL;
        pStreamData->QueryInterface(CProxyDirect3DVertexBuffer_GUID, (void**)&pProxy);

        // If so, use the original vertex buffer
        if (pProxy)
        {
            pStreamData = pProxy->GetOriginal();
            SAFE_RELEASE(pProxy);
        }
    }

    return pStreamData;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetIndices
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
IDirect3DIndexBuffer9* CDirect3DEvents9::GetRealIndexBuffer(IDirect3DIndexBuffer9* pIndexData)
{
    if (pIndexData)
    {
        // See if it's a proxy
        CProxyDirect3DIndexBuffer* pProxy = NULL;
        pIndexData->QueryInterface(CProxyDirect3DIndexBuffer_GUID, (void**)&pProxy);

        // If so, use the original index buffer
        if (pProxy)
        {
            pIndexData = pProxy->GetOriginal();
            SAFE_RELEASE(pProxy);
        }
    }
    return pIndexData;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetTexture
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
IDirect3DBaseTexture9* CDirect3DEvents9::GetRealTexture(IDirect3DBaseTexture9* pTexture)
{
    if (pTexture)
    {
        // See if it's a proxy
        CProxyDirect3DTexture* pProxy = NULL;
        pTexture->QueryInterface(CProxyDirect3DTexture_GUID, (void**)&pProxy);

        // If so, use the original texture
        if (pProxy)
        {
            pTexture = pProxy->GetOriginal();
            SAFE_RELEASE(pProxy);
        }
    }
    return pTexture;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateVertexDeclaration
//
// Creates a proxy object for the new vertex declaration
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateVertexDeclaration(IDirect3DDevice9* pDevice, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
    if (!pDevice)
    {
        WriteDebugEvent("CreateVertexDeclaration: pDevice is null");
        return D3DERR_INVALIDCALL;
    }

    if (!pVertexElements)
    {
        WriteDebugEvent("CreateVertexDeclaration: pVertexElements is null");
        return D3DERR_INVALIDCALL;
    }

    if (!ppDecl)
    {
        WriteDebugEvent("CreateVertexDeclaration: ppDecl is null");
        return D3DERR_INVALIDCALL;
    }

    *ppDecl = nullptr;

    HRESULT                          hr = D3D_OK;
    IDirect3DVertexDeclaration9*     pOriginalDecl = nullptr;

    hr = pDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
    if (FAILED(hr))
    {
        SString strStatus;
        // Make a string with decl info
        for (uint i = 0; i < MAXD3DDECLLENGTH; i++)
        {
            const D3DVERTEXELEMENT9& element = pVertexElements[i];
            if (element.Stream == 0xFF)
                break;

            strStatus += SString("[%d,%d,%d,%d,%d,%d]", element.Stream, element.Offset, element.Type, element.Method, element.Usage, element.UsageIndex);
        }

        SString strMessage("CreateVertexDecl fail: hr:%x %s", hr, *strStatus);
        WriteDebugEvent(strMessage);
        AddReportLog(8613, strMessage);
        CCore::GetSingleton().LogEvent(613, "CreateVertexDecl", "", strMessage);
        return hr;
    }

    pOriginalDecl = *ppDecl;
    if (!pOriginalDecl)
    {
        WriteDebugEvent("CreateVertexDeclaration: driver returned a null declaration");
        return D3DERR_INVALIDCALL;
    }

    // Create proxy
    *ppDecl = new CProxyDirect3DVertexDeclaration(pDevice, pOriginalDecl, pVertexElements);
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetVertexDeclaration
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::SetVertexDeclaration(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9* pDecl)
{
    if (pDecl)
    {
        // See if it's a proxy
        CProxyDirect3DVertexDeclaration* pProxy = NULL;
        pDecl->QueryInterface(CProxyDirect3DVertexDeclaration_GUID, (void**)&pProxy);

        // If so, use the original vertex declaration
        if (pProxy)
        {
            pDecl = pProxy->GetOriginal();

            // Update state info
            CScopedActiveProxyDevice proxyDevice;
            if (proxyDevice)
            {
                CProxyDirect3DDevice9::SD3DVertexDeclState* pInfo = MapFind(proxyDevice->m_VertexDeclMap, pProxy);
                if (pInfo)
                    g_pDeviceState->VertexDeclState = *pInfo;
            }

            SAFE_RELEASE(pProxy);
        }
    }

    return pDevice->SetVertexDeclaration(pDecl);
}

////////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::DiscoverReadableDepthFormat
//
// Check the best available readable depth buffer format
//
////////////////////////////////////////////////////////////////
ERenderFormat CDirect3DEvents9::DiscoverReadableDepthFormat(IDirect3DDevice9* pDevice, D3DMULTISAMPLE_TYPE multisampleType, bool bWindowed)
{
    IDirect3D9* pD3D = NULL;
    pDevice->GetDirect3D(&pD3D);

    ERenderFormat discoveredFormat = RFORMAT_UNKNOWN;

    if (pD3D)
    {
        // Formats to check for
        ERenderFormat checkList[] = {RFORMAT_INTZ, RFORMAT_DF24, RFORMAT_DF16, RFORMAT_RAWZ};

        D3DDISPLAYMODE displayMode;
        if (pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode) == D3D_OK)
        {
            for (uint i = 0; i < NUMELMS(checkList); i++)
            {
                D3DFORMAT DepthFormat = (D3DFORMAT)checkList[i];

                // Can use this format?
                if (D3D_OK != pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DepthFormat))
                    continue;

                // Don't check for compatibility with multisampling, as we turn AA off when using readable depth buffer

                // Found a working format
                discoveredFormat = checkList[i];
                break;
            }
        }
    }

    SAFE_RELEASE(pD3D);
    return discoveredFormat;
}

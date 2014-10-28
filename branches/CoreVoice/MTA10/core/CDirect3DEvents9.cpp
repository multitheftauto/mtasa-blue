/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DEvents9.cpp
*  PURPOSE:     Handler implementations for Direct3D 9 events
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CDirect3DEvents9
#include "profiler/SharedUtil.Profiler.h"
#include "CProxyDirect3DVertexBuffer.h"
#include "CProxyDirect3DIndexBuffer.h"
#include "CProxyDirect3DTexture.h"
#include "CAdditionalVertexStreamManager.h"
#include "CVertexStreamBoundingBoxManager.h"
#include "CProxyDirect3DVertexDeclaration.h"

bool g_bInMTAScene = false;

// Variables used for screen shot saving
static CBuffer             ms_ScreenShotBuffer;
static long long           ms_LastSaveTime      = 0;
// Other variables
static uint                ms_RequiredAnisotropicLevel = 1;
static EDiagnosticDebugType ms_DiagnosticDebug = EDiagnosticDebug::NONE;

// To reuse shader setups between calls to DrawIndexedPrimitive
CShaderItem* g_pActiveShader = NULL;

void CDirect3DEvents9::OnDirect3DDeviceCreate  ( IDirect3DDevice9 *pDevice )
{
    WriteDebugEvent ( "CDirect3DEvents9::OnDirect3DDeviceCreate" );

    // Create the GUI manager
    CCore::GetSingleton ( ).InitGUI ( pDevice );

    CAdditionalVertexStreamManager::GetSingleton ()->OnDeviceCreate ( pDevice );
    CVertexStreamBoundingBoxManager::GetSingleton ()->OnDeviceCreate ( pDevice );

    // Create all our fonts n stuff
    CGraphics::GetSingleton ().OnDeviceCreate ( pDevice );

    // Create the GUI elements
    CLocalGUI::GetSingleton().CreateObjects ( pDevice );    
}

void CDirect3DEvents9::OnDirect3DDeviceDestroy ( IDirect3DDevice9 *pDevice )
{
    WriteDebugEvent ( "CDirect3DEvents9::OnDirect3DDeviceDestroy" );

    // Unload the current mod
    CModManager::GetSingleton ().Unload ();

    // Destroy the GUI elements
    CLocalGUI::GetSingleton().DestroyObjects ();

    // De-initialize the GUI manager (destroying is done on Exit)
    CCore::GetSingleton ( ).DeinitGUI ();
}

void CDirect3DEvents9::OnBeginScene ( IDirect3DDevice9 *pDevice )
{
    // Notify core
    CCore::GetSingleton ().DoPreFramePulse ();
}

bool CDirect3DEvents9::OnEndScene ( IDirect3DDevice9 *pDevice )
{
    return true;
}

void CDirect3DEvents9::OnInvalidate ( IDirect3DDevice9 *pDevice )
{
    WriteDebugEvent ( "CDirect3DEvents9::OnInvalidate" );

    // Invalidate the VMR9 Manager
    //CVideoManager::GetSingleton ().OnLostDevice ();

    // Notify gui
    CLocalGUI::GetSingleton().Invalidate ();

    CGraphics::GetSingleton ().OnDeviceInvalidate ( pDevice );
}

void CDirect3DEvents9::OnRestore ( IDirect3DDevice9 *pDevice )
{
    WriteDebugEvent ( "CDirect3DEvents9::OnRestore" );

    // Restore the VMR9 manager
    //CVideoManager::GetSingleton ().OnResetDevice ( pDevice );

    // Restore the GUI
    CLocalGUI::GetSingleton().Restore ();

    // Restore the graphics manager
    CGraphics::GetSingleton ().OnDeviceRestore ( pDevice );

    CCore::GetSingleton ().OnDeviceRestore ();
}

void CDirect3DEvents9::OnPresent ( IDirect3DDevice9 *pDevice )
{
    TIMING_CHECKPOINT( "+OnPresent1" );
    // Start a new scene. This isn't ideal and is not really recommended by MSDN.
    // I tried disabling EndScene from GTA and just end it after this code ourselves
    // before present, but that caused graphical issues randomly with the sky.
    if ( pDevice->BeginScene() == D3D_OK )
        g_bInMTAScene = true;

    // Reset samplers on first call
    static bool bDoneReset = false;
    if ( !bDoneReset )
    {
        bDoneReset = true;
        for ( uint i = 0 ; i < 16 ; i++ )
        {
            pDevice->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            pDevice->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
            pDevice->SetSamplerState ( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        }
    }

    CGraphics::GetSingleton ().EnteringMTARenderZone();

    // Tell everyone that the zbuffer will need clearing before use
    CGraphics::GetSingleton ().OnZBufferModified ();

    TIMING_CHECKPOINT( "-OnPresent1" );
    // Notify core
    CCore::GetSingleton ().DoPostFramePulse ();
    TIMING_CHECKPOINT( "+OnPresent2" );

    // Restore in case script forgets
    CGraphics::GetSingleton ().GetRenderItemManager ()->RestoreDefaultRenderTarget ();

    // Draw pre-GUI primitives
    CGraphics::GetSingleton ().DrawPreGUIQueue ();

    // Maybe grab screen for upload
    CGraphics::GetSingleton ().GetScreenGrabber ()->DoPulse ();

    // Draw the GUI
    CLocalGUI::GetSingleton().Draw ();

    // Draw post-GUI primitives
    CGraphics::GetSingleton ().DrawPostGUIQueue ();

    // Redraw the mouse cursor so it will always be over other elements
    CLocalGUI::GetSingleton().DrawMouseCursor();

    CGraphics::GetSingleton().DidRenderScene();

    CGraphics::GetSingleton ().LeavingMTARenderZone();

    // End the scene that we started.
    pDevice->EndScene ();
    g_bInMTAScene = false;

    // Update incase settings changed
    int iAnisotropic;
    CVARS_GET ( "anisotropic", iAnisotropic );
    ms_RequiredAnisotropicLevel = 1 << iAnisotropic;
    ms_DiagnosticDebug = CCore::GetSingleton ().GetDiagnosticDebug ();
    CCore::GetSingleton().GetGUI ()->SetBidiEnabled ( ms_DiagnosticDebug != EDiagnosticDebug::BIDI_6778 );
    
    // Make a screenshot if needed
    CheckForScreenShot ();

    TIMING_CHECKPOINT( "-OnPresent2" );

    CGraphics::GetSingleton ().GetRenderItemManager ()->FlushNonAARenderTarget();
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CheckForScreenShot
//
// Take a screenshot if required and able
//
/////////////////////////////////////////////////////////////
void CDirect3DEvents9::CheckForScreenShot ( void )
{
    // Make a screenshot if needed
    if ( CCore::GetSingleton().bScreenShot && !CScreenShot::IsSaving () )
    {
        // Max one screenshot per second
        if ( GetTickCount64_ () - ms_LastSaveTime < 1000 )
            return;
        ms_LastSaveTime = GetTickCount64_ ();

        uint uiWidth = CDirect3DData::GetSingleton ().GetViewportWidth ();
        uint uiHeight = CDirect3DData::GetSingleton ().GetViewportHeight ();

        // Call the pre-screenshot function 
        SString strFileName = CScreenShot::PreScreenShot ();

        // Try to get the screen data
        SString strError;
        if ( CGraphics::GetSingleton ().GetScreenGrabber ()->GetBackBufferPixels ( uiWidth, uiHeight, ms_ScreenShotBuffer, strError ) )
        {
            // Validate data size
            uint uiDataSize = ms_ScreenShotBuffer.GetSize ();
            uint uiReqDataSize = uiWidth * uiHeight * 4;

            if ( uiDataSize == uiReqDataSize )
            {
                // Start the save thread
                CScreenShot::BeginSave ( strFileName, ms_ScreenShotBuffer.GetData (), uiDataSize, uiWidth, uiHeight );
            }
            else
            {
                g_pCore->GetConsole()->Printf ( _("Screenshot got %d bytes, but expected %d"), uiDataSize, uiReqDataSize );
                strFileName = "";
            }
        }
        else
        {
            g_pCore->GetConsole()->Print ( _("Screenshot failed") + SString( " (%s)", *strError ) );
            strFileName = "";
        }

        // Call the post-screenshot function
        CScreenShot::PostScreenShot ( strFileName );

        CCore::GetSingleton().bScreenShot = false;
    }
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
HRESULT CDirect3DEvents9::OnDrawPrimitive ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount )
{
    if ( ms_DiagnosticDebug == EDiagnosticDebug::GRAPHICS_6734 )
        return pDevice->DrawPrimitive ( PrimitiveType, StartVertex, PrimitiveCount );

    CloseActiveShader();

    // Any shader for this texture ?
    SShaderItemLayers* pLayers = CGraphics::GetSingleton ().GetRenderItemManager ()->GetAppliedShaderForD3DData ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );

    // Skip draw if there is a vertex shader conflict
    if ( pLayers && pLayers->bUsesVertexShader && g_pDeviceState->VertexShader )
        return D3D_OK;

    if ( !pLayers )
    {
        // No shaders for this texture
        return DrawPrimitiveGuarded ( pDevice, PrimitiveType, StartVertex, PrimitiveCount );
    }
    else
    {
        // Yes base shader
        DrawPrimitiveShader ( pDevice, PrimitiveType, StartVertex, PrimitiveCount, pLayers->pBase, false );

        // Draw each layer
        if ( !pLayers->layerList.empty () )
        {
            float fSlopeDepthBias = -0.02f;
            float fDepthBias = -0.00002f;
            SAVE_RENDERSTATE_AND_SET( SLOPESCALEDEPTHBIAS,  *(DWORD*)&fSlopeDepthBias );
            SAVE_RENDERSTATE_AND_SET( DEPTHBIAS,            *(DWORD*)&fDepthBias );
            SAVE_RENDERSTATE_AND_SET( ALPHABLENDENABLE, TRUE );
            SAVE_RENDERSTATE_AND_SET( SRCBLEND,         D3DBLEND_SRCALPHA );
            SAVE_RENDERSTATE_AND_SET( DESTBLEND,        D3DBLEND_INVSRCALPHA );
            SAVE_RENDERSTATE_AND_SET( ALPHATESTENABLE,  TRUE );
            SAVE_RENDERSTATE_AND_SET( ALPHAREF,         0x01 );
            SAVE_RENDERSTATE_AND_SET( ALPHAFUNC,        D3DCMP_GREATER );
            SAVE_RENDERSTATE_AND_SET( ZWRITEENABLE,     FALSE );
            SAVE_RENDERSTATE_AND_SET( ZFUNC,            D3DCMP_LESSEQUAL );

            for ( uint i = 0 ; i < pLayers->layerList.size () ; i++ )
            {
                DrawPrimitiveShader ( pDevice, PrimitiveType, StartVertex, PrimitiveCount, pLayers->layerList[i], true );
            }

            RESTORE_RENDERSTATE( SLOPESCALEDEPTHBIAS );
            RESTORE_RENDERSTATE( DEPTHBIAS );
            RESTORE_RENDERSTATE( ALPHABLENDENABLE );
            RESTORE_RENDERSTATE( SRCBLEND );
            RESTORE_RENDERSTATE( DESTBLEND );
            RESTORE_RENDERSTATE( ALPHATESTENABLE );
            RESTORE_RENDERSTATE( ALPHAREF );
            RESTORE_RENDERSTATE( ALPHAFUNC );
            RESTORE_RENDERSTATE( ZWRITEENABLE );
            RESTORE_RENDERSTATE( ZFUNC );
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
HRESULT CDirect3DEvents9::DrawPrimitiveShader ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount, CShaderItem* pShaderItem, bool bIsLayer )
{
    if ( !pShaderItem )
    {
        // No shader for this texture
        if ( !bIsLayer )
            return DrawPrimitiveGuarded ( pDevice, PrimitiveType, StartVertex, PrimitiveCount );
    }
    else
    {
        // Yes shader for this texture
        CShaderInstance* pShaderInstance = pShaderItem->m_pShaderInstance;

        // Save some debugging info
        g_pDeviceState->CallState.strShaderName = pShaderInstance->m_pEffectWrap->m_strName;
        g_pDeviceState->CallState.bShaderRequiresNormals = pShaderInstance->m_pEffectWrap->m_bRequiresNormals;

        // Apply custom parameters
        pShaderInstance->ApplyShaderParameters ();
        // Apply common parameters
        pShaderInstance->m_pEffectWrap->ApplyCommonHandles ();
        // Apply mapped parameters
        pShaderInstance->m_pEffectWrap->ApplyMappedHandles ();

        // Remember vertex shader if original draw was using it
        IDirect3DVertexShader9* pOriginalVertexShader = NULL;
        pDevice->GetVertexShader ( &pOriginalVertexShader );

        // Do shader passes
        ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

        DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint uiNumPasses = 0;
        pD3DEffect->Begin ( &uiNumPasses, dwFlags );

        for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
        {
            pD3DEffect->BeginPass ( uiPass );

            // Apply original vertex shader if original draw was using it (i.e. for ped animation)
            if ( pOriginalVertexShader )
                pDevice->SetVertexShader( pOriginalVertexShader );

            DrawPrimitiveGuarded ( pDevice, PrimitiveType, StartVertex, PrimitiveCount );
            pD3DEffect->EndPass ();
        }
        pD3DEffect->End ();

        // If we didn't get the effect to save the shader state, clear some things here
        if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
        {
            pDevice->SetVertexShader( pOriginalVertexShader );
            pDevice->SetPixelShader( NULL );
        }

        g_pDeviceState->CallState.strShaderName = "";
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
HRESULT CDirect3DEvents9::OnDrawIndexedPrimitive ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount )
{
    if ( ms_DiagnosticDebug == EDiagnosticDebug::GRAPHICS_6734 )
        return pDevice->DrawIndexedPrimitive ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

    // Apply anisotropic filtering if required
    if ( ms_RequiredAnisotropicLevel > 1 )
    {
        pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
        pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        if ( ms_RequiredAnisotropicLevel > g_pDeviceState->SamplerState[0].MAXANISOTROPY )
            pDevice->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, ms_RequiredAnisotropicLevel );
    }

    // See if we can continue using the active shader
    if ( g_pActiveShader )
    {
        return DrawIndexedPrimitiveShader ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, g_pActiveShader, false, false );
    }

    // Any shader for this texture ?
    SShaderItemLayers* pLayers = CGraphics::GetSingleton ().GetRenderItemManager ()->GetAppliedShaderForD3DData ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );

    // Skip draw if there is a vertex shader conflict
    if ( pLayers && pLayers->bUsesVertexShader && g_pDeviceState->VertexShader )
        return D3D_OK;

    if ( !pLayers )
    {
        // No shaders for this texture
        return DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
    }
    else
    {
        // Draw base shader
        DrawIndexedPrimitiveShader ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, pLayers->pBase, false, pLayers->layerList.empty() );

        // Draw each layer
        if ( !pLayers->layerList.empty () )
        {
            float fSlopeDepthBias = -0.02f;
            float fDepthBias = -0.00002f;
            SAVE_RENDERSTATE_AND_SET( SLOPESCALEDEPTHBIAS,  *(DWORD*)&fSlopeDepthBias );
            SAVE_RENDERSTATE_AND_SET( DEPTHBIAS,            *(DWORD*)&fDepthBias );
            SAVE_RENDERSTATE_AND_SET( ALPHABLENDENABLE, TRUE );
            SAVE_RENDERSTATE_AND_SET( SRCBLEND,         D3DBLEND_SRCALPHA );
            SAVE_RENDERSTATE_AND_SET( DESTBLEND,        D3DBLEND_INVSRCALPHA );
            SAVE_RENDERSTATE_AND_SET( ALPHATESTENABLE,  TRUE );
            SAVE_RENDERSTATE_AND_SET( ALPHAREF,         0x01 );
            SAVE_RENDERSTATE_AND_SET( ALPHAFUNC,        D3DCMP_GREATER );
            SAVE_RENDERSTATE_AND_SET( ZWRITEENABLE,     FALSE );
            SAVE_RENDERSTATE_AND_SET( ZFUNC,            D3DCMP_LESSEQUAL );

            for ( uint i = 0 ; i < pLayers->layerList.size () ; i++ )
            {
                DrawIndexedPrimitiveShader ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount, pLayers->layerList[i], true, false );
            }

            RESTORE_RENDERSTATE( SLOPESCALEDEPTHBIAS );
            RESTORE_RENDERSTATE( DEPTHBIAS );
            RESTORE_RENDERSTATE( ALPHABLENDENABLE );
            RESTORE_RENDERSTATE( SRCBLEND );
            RESTORE_RENDERSTATE( DESTBLEND );
            RESTORE_RENDERSTATE( ALPHATESTENABLE );
            RESTORE_RENDERSTATE( ALPHAREF );
            RESTORE_RENDERSTATE( ALPHAFUNC );
            RESTORE_RENDERSTATE( ZWRITEENABLE );
            RESTORE_RENDERSTATE( ZFUNC );
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
HRESULT CDirect3DEvents9::DrawIndexedPrimitiveShader ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, CShaderItem* pShaderItem, bool bIsLayer, bool bCanBecomeActiveShader )
{
    if ( pShaderItem && pShaderItem->m_fMaxDistanceSq > 0 )
    {
        // If shader has a max distance, check this vertex range bounding box
        float fDistanceSq = CVertexStreamBoundingBoxManager::GetSingleton ()->GetDistanceSqToGeometry ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

        // If distance was obtained and vertices are too far away, don't apply the shader
        if ( fDistanceSq > 0 && fDistanceSq > pShaderItem->m_fMaxDistanceSq )
            pShaderItem = NULL;
    }

    if ( !pShaderItem )
    {
        CloseActiveShader();

        // No shader for this texture
        if ( !bIsLayer )
            return DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
    }
    else
    {
        // See if we should use the previously setup shader
        if ( g_pActiveShader )
        {
            dassert( pShaderItem == g_pActiveShader );
            g_pDeviceState->FrameStats.iNumShadersReuseSetup++;

            // Transfer any state changes to the active shader
            CShaderInstance* pShaderInstance = g_pActiveShader->m_pShaderInstance;
            bool bChanged = pShaderInstance->m_pEffectWrap->ApplyCommonHandles ();
            bChanged |= pShaderInstance->m_pEffectWrap->ApplyMappedHandles ();
            if ( bChanged )
                pShaderInstance->m_pEffectWrap->m_pD3DEffect->CommitChanges();

            return DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
        }
        g_pDeviceState->FrameStats.iNumShadersFullSetup++;

        // Yes shader for this texture
        CShaderInstance* pShaderInstance = pShaderItem->m_pShaderInstance;

        // Save some debugging info
        g_pDeviceState->CallState.strShaderName = pShaderInstance->m_pEffectWrap->m_strName;
        g_pDeviceState->CallState.bShaderRequiresNormals = pShaderInstance->m_pEffectWrap->m_bRequiresNormals;

        // Add normal stream if shader wants it
        if ( pShaderInstance->m_pEffectWrap->m_bRequiresNormals )
        {
            // Find/create/set additional vertex stream
            CAdditionalVertexStreamManager::GetSingleton ()->MaybeSetAdditionalVertexStream ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
        }

        // Apply custom parameters
        pShaderInstance->ApplyShaderParameters ();
        // Apply common parameters
        pShaderInstance->m_pEffectWrap->ApplyCommonHandles ();
        // Apply mapped parameters
        pShaderInstance->m_pEffectWrap->ApplyMappedHandles ();

        // Remember vertex shader if original draw was using it
        IDirect3DVertexShader9* pOriginalVertexShader = NULL;
        pDevice->GetVertexShader ( &pOriginalVertexShader );

        // Do shader passes
        ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

        DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint uiNumPasses = 0;
        pD3DEffect->Begin ( &uiNumPasses, dwFlags );

        for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
        {
            pD3DEffect->BeginPass ( uiPass );

            // Apply original vertex shader if original draw was using it (i.e. for ped animation)
            if ( pOriginalVertexShader )
                pDevice->SetVertexShader( pOriginalVertexShader );

            DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

            if ( uiNumPasses == 1 && bCanBecomeActiveShader && pOriginalVertexShader == NULL && g_pCore->IsRenderingGrass() )
            {
                // Make this the active shader for possible reuse
                dassert( dwFlags == D3DXFX_DONOTSAVESHADERSTATE );
                g_pActiveShader = pShaderItem;
                return D3D_OK;
            }

            pD3DEffect->EndPass ();
        }
        pD3DEffect->End ();

        // If we didn't get the effect to save the shader state, clear some things here
        if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
        {
            pDevice->SetVertexShader( pOriginalVertexShader );
            pDevice->SetPixelShader( NULL );
        }

        // Unset additional vertex stream
        CAdditionalVertexStreamManager::GetSingleton ()->MaybeUnsetAdditionalVertexStream ();

        g_pDeviceState->CallState.strShaderName = "";
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
void CDirect3DEvents9::CloseActiveShader( void )
{
    if ( !g_pActiveShader )
        return;

    ID3DXEffect* pD3DEffect = g_pActiveShader->m_pShaderInstance->m_pEffectWrap->m_pD3DEffect;
    g_pActiveShader = NULL;

    pD3DEffect->EndPass ();

    pD3DEffect->End ();

    // We didn't get the effect to save the shader state, clear some things here
    IDirect3DDevice9* pDevice = g_pGraphics->GetDevice();
    pDevice->SetVertexShader( NULL );
    pDevice->SetPixelShader( NULL );

    // Unset additional vertex stream
    CAdditionalVertexStreamManager::GetSingleton ()->MaybeUnsetAdditionalVertexStream ();

    g_pDeviceState->CallState.strShaderName = "";
}


/////////////////////////////////////////////////////////////
//
// AreVertexStreamsAreBigEnough
//
// Occasionally, GTA tries to draw water/clouds/something with a vertex buffer that is
// too small (which causes problems for some graphics drivers).
//
// This function checks the sizes are valid
//
/////////////////////////////////////////////////////////////
bool AreVertexStreamsAreBigEnough ( IDirect3DDevice9* pDevice, uint viMinBased, uint viMaxBased )
{
    // Check each stream used
    for ( uint i = 0 ; i < NUMELMS( g_pDeviceState->VertexDeclState.bUsesStreamAtIndex ) ; i++ )
    {
        if ( g_pDeviceState->VertexDeclState.bUsesStreamAtIndex[i] )
        {
            IDirect3DVertexBuffer9* pStreamData = NULL;
            UINT StreamOffset;
            UINT StreamStride;
            pDevice->GetStreamSource ( i, &pStreamData, &StreamOffset, &StreamStride );

            if ( pStreamData )
            {
                D3DVERTEXBUFFER_DESC VertexBufferDesc;
                pStreamData->GetDesc ( &VertexBufferDesc );
                SAFE_RELEASE( pStreamData );

                uint ReadOffsetStart = viMinBased * StreamStride + StreamOffset;
                uint ReadOffsetSize = ( viMaxBased - viMinBased ) * StreamStride;

                uint MinSizeVertexBufferShouldBe = ReadOffsetStart + ReadOffsetSize;

                // Check vertex buffer is big enough to do the draw
                if ( VertexBufferDesc.Size < MinSizeVertexBufferShouldBe )
                    return false;
            }
        }
    }
    return true;
}


/////////////////////////////////////////////////////////////
//
// FilerException
//
// Check if exception should be handled by us
//
/////////////////////////////////////////////////////////////
uint uiLastExceptionCode = 0;
int FilerException( uint ExceptionCode )
{
    uiLastExceptionCode = ExceptionCode;
    if ( ExceptionCode == EXCEPTION_ACCESS_VIOLATION )
        return EXCEPTION_EXECUTE_HANDLER;
    if ( ExceptionCode == 0xE06D7363 )
        return EXCEPTION_EXECUTE_HANDLER;
    return EXCEPTION_CONTINUE_SEARCH;
}


/////////////////////////////////////////////////////////////
//
// DrawPrimitiveGuarded
//
// Catch access violations
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawPrimitiveGuarded ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount )
{
    if ( ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732 )
        return pDevice->DrawPrimitive ( PrimitiveType, StartVertex, PrimitiveCount );

    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    if ( PrimitiveType == D3DPT_TRIANGLELIST || PrimitiveType == D3DPT_TRIANGLESTRIP )
    {
        UINT NumVertices = PrimitiveCount + 2;
        if ( PrimitiveType == D3DPT_TRIANGLELIST )
            NumVertices = PrimitiveCount * 3;

        uint viMinBased = StartVertex;
        uint viMaxBased = NumVertices + StartVertex;

        if ( !AreVertexStreamsAreBigEnough ( pDevice, viMinBased, viMaxBased ) )
            return hr;
    }

    __try
    {
        hr = pDevice->DrawPrimitive ( PrimitiveType, StartVertex, PrimitiveCount );
    }
    __except( FilerException( GetExceptionCode() ) )
    {
        CCore::GetSingleton ().OnCrashAverted ( ( uiLastExceptionCode & 0xFFFF ) + 1 * 1000000 );
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
HRESULT CDirect3DEvents9::DrawIndexedPrimitiveGuarded ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount )
{
    if ( ms_DiagnosticDebug == EDiagnosticDebug::D3D_6732 )
        return pDevice->DrawIndexedPrimitive ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    uint viMinBased = MinVertexIndex + BaseVertexIndex;
    uint viMaxBased = MinVertexIndex + NumVertices + BaseVertexIndex;

    if ( !AreVertexStreamsAreBigEnough ( pDevice, viMinBased, viMaxBased ) )
        return hr;

    __try
    {
        hr = pDevice->DrawIndexedPrimitive ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
    }
    __except( FilerException( GetExceptionCode() ) )
    {
        CCore::GetSingleton ().OnCrashAverted ( ( uiLastExceptionCode & 0xFFFF ) + 2 * 1000000 );
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
HRESULT CDirect3DEvents9::CreateVertexBuffer ( IDirect3DDevice9 *pDevice, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle )
{
    HRESULT hr;

    // We may have to look at the contents to generate normals (Not needed for dynamic buffers)
    if ( ( Usage & D3DUSAGE_DYNAMIC ) == 0 )
        Usage &= -1 - D3DUSAGE_WRITEONLY;

    for ( uint i = 0 ; i < 2 ; i++ )
    {
        hr = pDevice->CreateVertexBuffer ( Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle );
        if( SUCCEEDED( hr ) )
            break;

        if ( hr != D3DERR_OUTOFVIDEOMEMORY || i > 0 )
        {
            SString strMessage( "CreateVertexBuffer fail: hr:%x Length:%x Usage:%x FVF:%x Pool:%x", hr, Length, Usage, FVF, Pool );
            WriteDebugEvent( strMessage );
            AddReportLog( 8610, strMessage );
            CCore::GetSingleton ().LogEvent ( 610, "CreateVertexBuffer", "", strMessage );
            return hr;
        }

        pDevice->EvictManagedResources ();
    }

    // Create proxy
    *ppVertexBuffer = new CProxyDirect3DVertexBuffer ( pDevice, *ppVertexBuffer, Length, Usage, FVF, Pool );
    return hr;
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateIndexBuffer
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateIndexBuffer ( IDirect3DDevice9 *pDevice, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle )
{
    HRESULT hr;

    for ( uint i = 0 ; i < 2 ; i++ )
    {
        hr = pDevice->CreateIndexBuffer ( Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle );
        if( SUCCEEDED( hr ) )
            break;

        if ( hr != D3DERR_OUTOFVIDEOMEMORY || i > 0 )
        {
            SString strMessage( "CreateIndexBuffer fail: hr:%x Length:%x Usage:%x Format:%x Pool:%x", hr, Length, Usage, Format, Pool );
            WriteDebugEvent( strMessage );
            AddReportLog( 8611, strMessage );
            CCore::GetSingleton ().LogEvent ( 611, "CreateIndexBuffer", "", strMessage );
            return hr;
        }

        pDevice->EvictManagedResources ();
    }

    // Create proxy so we can track when it's finished with
    *ppIndexBuffer = new CProxyDirect3DIndexBuffer ( pDevice, *ppIndexBuffer, Length, Usage, Format, Pool );
    return hr;
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::CreateTexture
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::CreateTexture ( IDirect3DDevice9 *pDevice, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle )
{
    HRESULT hr;

    for ( uint i = 0 ; i < 2 ; i++ )
    {
        hr = pDevice->CreateTexture ( Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle );
        if( SUCCEEDED( hr ) )
            break;

        if ( hr != D3DERR_OUTOFVIDEOMEMORY || i > 0 )
        {
            SString strMessage( "CreateTexture fail: hr:%x W:%x H:%x L:%x Usage:%x Format:%x Pool:%x", hr, Width, Height, Levels, Usage, Format, Pool );
            WriteDebugEvent( strMessage );
            AddReportLog( 8612, strMessage );
            CCore::GetSingleton ().LogEvent ( 612, "CreateTexture", "", strMessage );
            return hr;
        }

        pDevice->EvictManagedResources ();
    }

    // Create proxy so we can track when it's finished with
    *ppTexture = new CProxyDirect3DTexture ( pDevice, *ppTexture, Width, Height, Levels, Usage, Format, Pool );
    return hr;
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetStreamSource
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
IDirect3DVertexBuffer9* CDirect3DEvents9::GetRealVertexBuffer ( IDirect3DVertexBuffer9* pStreamData )
{
	if( pStreamData )
	{
        // See if it's a proxy
	    CProxyDirect3DVertexBuffer* pProxy = NULL;
        pStreamData->QueryInterface ( CProxyDirect3DVertexBuffer_GUID, (void**)&pProxy );

        // If so, use the original vertex buffer
        if ( pProxy )
            pStreamData = pProxy->GetOriginal ();
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
IDirect3DIndexBuffer9* CDirect3DEvents9::GetRealIndexBuffer  ( IDirect3DIndexBuffer9* pIndexData )
{
	if( pIndexData )
	{
        // See if it's a proxy
	    CProxyDirect3DIndexBuffer* pProxy = NULL;
        pIndexData->QueryInterface ( CProxyDirect3DIndexBuffer_GUID, (void**)&pProxy );

        // If so, use the original index buffer
        if ( pProxy )
            pIndexData = pProxy->GetOriginal ();
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
IDirect3DBaseTexture9* CDirect3DEvents9::GetRealTexture ( IDirect3DBaseTexture9* pTexture )
{
	if( pTexture )
	{
        // See if it's a proxy
	    CProxyDirect3DTexture* pProxy = NULL;
        pTexture->QueryInterface ( CProxyDirect3DTexture_GUID, (void**)&pProxy );

        // If so, use the original texture
        if ( pProxy )
            pTexture = pProxy->GetOriginal ();
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
HRESULT CDirect3DEvents9::CreateVertexDeclaration ( IDirect3DDevice9 *pDevice, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl )
{
    HRESULT hr;

    hr = pDevice->CreateVertexDeclaration ( pVertexElements, ppDecl );
    if ( FAILED(hr) )
    {
        SString strStatus;
        // Make a string with decl info
        for ( uint i = 0 ; i < MAXD3DDECLLENGTH ; i++ )
        {
            const D3DVERTEXELEMENT9& element = pVertexElements[ i ];
            if ( element.Stream == 0xFF )
                break;

            strStatus += SString( "[%d,%d,%d,%d,%d,%d]"
                                    ,element.Stream
                                    ,element.Offset
                                    ,element.Type
                                    ,element.Method
                                    ,element.Usage
                                    ,element.UsageIndex
                                    );
        }

        SString strMessage( "CreateVertexDecl fail: hr:%x %s", hr, *strStatus );
        WriteDebugEvent( strMessage );
        AddReportLog( 8613, strMessage );
        CCore::GetSingleton ().LogEvent ( 613, "CreateVertexDecl", "", strMessage );
        return hr;
    }

    // Create proxy
	*ppDecl = new CProxyDirect3DVertexDeclaration ( pDevice, *ppDecl, pVertexElements );
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetVertexDeclaration
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::SetVertexDeclaration ( IDirect3DDevice9 *pDevice, IDirect3DVertexDeclaration9* pDecl )
{
	if( pDecl )
	{
        // See if it's a proxy
	    CProxyDirect3DVertexDeclaration* pProxy = NULL;
        pDecl->QueryInterface ( CProxyDirect3DVertexDeclaration_GUID, (void**)&pProxy );

        // If so, use the original vertex declaration
        if ( pProxy )
        {
            pDecl = pProxy->GetOriginal ();

            // Update state info
            CProxyDirect3DDevice9::SD3DVertexDeclState* pInfo = MapFind ( g_pProxyDevice->m_VertexDeclMap, pProxy );
            if ( pInfo )
                g_pDeviceState->VertexDeclState = *pInfo;
        }
    }

    return pDevice->SetVertexDeclaration ( pDecl );
}


////////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::DiscoverReadableDepthFormat
//
// Check the best available readable depth buffer format
//
////////////////////////////////////////////////////////////////
ERenderFormat CDirect3DEvents9::DiscoverReadableDepthFormat ( IDirect3DDevice9 *pDevice, D3DMULTISAMPLE_TYPE multisampleType, bool bWindowed )
{
    IDirect3D9* pD3D = NULL;
    pDevice->GetDirect3D ( &pD3D );

    // Formats to check for
    ERenderFormat checkList[] = { RFORMAT_INTZ, RFORMAT_DF24, RFORMAT_DF16, RFORMAT_RAWZ };

    D3DDISPLAYMODE displayMode;
    if ( pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode ) == D3D_OK )
    {
        for ( uint i = 0; i < NUMELMS( checkList ) ; i++ )
        {
            D3DFORMAT DepthFormat = (D3DFORMAT)checkList[i];

            // Can use this format?
            if ( D3D_OK != pD3D->CheckDeviceFormat ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DepthFormat ) )
                continue;

            // Don't check for compatibility with multisampling, as we turn AA off when using readable depth buffer

            // Found a working format
            return checkList[i];
        }
    }

    return RFORMAT_UNKNOWN;
}

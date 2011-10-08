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
#include "CAdditionalVertexStreamManager.h"
#include "CVertexStreamBoundingBoxManager.h"
#include "CProxyDirect3DVertexDeclaration.h"

#include <stdexcept>

// Variables used for screen shot saving
static IDirect3DSurface9*  ms_pSaveLockSurface  = NULL;
static int                 ms_bSaveStarted      = 0;
static long long           ms_LastSaveTime      = 0;
static uint                ms_RequiredAnisotropicLevel = 1;


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
    // Start a new scene. This isn't ideal and is not really recommended by MSDN.
    // I tried disabling EndScene from GTA and just end it after this code ourselves
    // before present, but that caused graphical issues randomly with the sky.
    pDevice->BeginScene ();

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

    // Create a state block.
    IDirect3DStateBlock9 * pDeviceState = NULL;
    pDevice->CreateStateBlock ( D3DSBT_ALL, &pDeviceState );

    // Make sure linear sampling is enabled
    pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    // Make sure stencil is off to avoid problems with flame effects
    pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );

    // Notify core
    CCore::GetSingleton ().DoPostFramePulse ();

    // Draw pre-GUI primitives
    CGraphics::GetSingleton ().DrawPreGUIQueue ();

    // Draw the GUI
    CLocalGUI::GetSingleton().Draw ();

    // Draw post-GUI primitives
    CGraphics::GetSingleton ().DrawPostGUIQueue ();

    // Redraw the mouse cursor so it will always be over other elements
    CLocalGUI::GetSingleton().DrawMouseCursor();

    // Restore the render states
    if ( pDeviceState )
    {
        pDeviceState->Apply ( );
        pDeviceState->Release ( );
    }
    
    // End the scene that we started.
    pDevice->EndScene ();

    // Update incase settings changed
    int iAnisotropic;
    CVARS_GET ( "anisotropic", iAnisotropic );
    ms_RequiredAnisotropicLevel = 1 << iAnisotropic;
    
    // Tidyup after last screenshot
    if ( ms_bSaveStarted )
    {
        // Done yet?
        if ( !CScreenShot::IsSaving () )
        {
            ms_pSaveLockSurface->UnlockRect ();
            ms_pSaveLockSurface->Release ();
            ms_pSaveLockSurface = NULL;
            ms_bSaveStarted = false;
        }
    }

    // Make a screenshot if needed
    if ( CCore::GetSingleton().bScreenShot && ms_bSaveStarted == false ) {

        // Max one screenshot per second
        if ( GetTickCount64_ () - ms_LastSaveTime < 1000 )
            return;
        ms_LastSaveTime = GetTickCount64_ ();

        RECT ScreenSize;

        IDirect3DSurface9 *pSurface = NULL;

        // Define a screen rectangle
        ScreenSize.top = ScreenSize.left = 0;
        ScreenSize.right = CDirect3DData::GetSingleton ().GetViewportWidth ();
        ScreenSize.bottom = CDirect3DData::GetSingleton ().GetViewportHeight ();        
        pDevice->GetRenderTarget ( 0, &pSurface );

        // Create a new render target
        if ( !pSurface || pDevice->CreateRenderTarget ( ScreenSize.right, ScreenSize.bottom, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &ms_pSaveLockSurface, NULL ) != D3D_OK ) {
            CCore::GetSingleton ().GetConsole ()->Printf("Couldn't create a new render target.");
        } else {
            unsigned long ulBeginTime = GetTickCount32 ();

            // Copy data from surface to surface
            if ( pDevice->StretchRect ( pSurface, &ScreenSize, ms_pSaveLockSurface, &ScreenSize, D3DTEXF_NONE ) != D3D_OK ) {
                CCore::GetSingleton ().GetConsole ()->Printf("Couldn't copy the surface.");
            }

            // Lock the surface
            D3DLOCKED_RECT LockedRect;
            if ( ms_pSaveLockSurface->LockRect ( &LockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK ) != D3D_OK ) {
                CCore::GetSingleton ().GetConsole ()->Printf("Couldn't lock the surface.");
            }

            // Call the pre-screenshot function 
            SString strFileName = CScreenShot::PreScreenShot ();

            // Start the save thread
            CScreenShot::BeginSave ( strFileName, LockedRect.pBits, LockedRect.Pitch, ScreenSize );
            ms_bSaveStarted = true;

            // Call the post-screenshot function
            CScreenShot::PostScreenShot ( strFileName );

            CCore::GetSingleton ().GetConsole ()->Printf ( "Screenshot capture took %.2f seconds.", (float)(GetTickCount32 () - ulBeginTime) / 1000.0f );
        }

        CCore::GetSingleton().bScreenShot = false;

        if ( pSurface )
        {
            if ( FAILED ( pSurface->Release () ) )
                std::runtime_error ( "Failed to release the ScreenShot rendertaget surface" );
            pSurface = NULL;
        }
    }
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::OnDrawPrimitive
//
// May change render states for custom renderings
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::OnDrawPrimitive ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount )
{
    // Any shader for this texture ?
    CShaderItem* pShaderItem = CGraphics::GetSingleton ().GetRenderItemManager ()->GetAppliedShaderForD3DData ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );

    // Don't apply if a vertex shader is already being used
    if ( g_pDeviceState->VertexShader )
        pShaderItem = NULL;

    if ( !pShaderItem )
    {
        // No shader for this texture
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

        // Do shader passes
        ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

        DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint uiNumPasses = 0;
        pD3DEffect->Begin ( &uiNumPasses, dwFlags );

        for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
        {
            pD3DEffect->BeginPass ( uiPass );
            DrawPrimitiveGuarded ( pDevice, PrimitiveType, StartVertex, PrimitiveCount );
            pD3DEffect->EndPass ();
        }
        pD3DEffect->End ();

        // If we didn't get the effect to save the shader state, clear some things here
        if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
        {
            pDevice->SetVertexShader( NULL );
            pDevice->SetPixelShader( NULL );
        }

        g_pDeviceState->CallState.strShaderName = "";
        return D3D_OK;
    }
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
    // Apply anisotropic filtering if required
    if ( ms_RequiredAnisotropicLevel > 1 )
    {
        pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
        pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        if ( ms_RequiredAnisotropicLevel > g_pDeviceState->SamplerState[0].MAXANISOTROPY )
            pDevice->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, ms_RequiredAnisotropicLevel );
    }

    // Any shader for this texture ?
    CShaderItem* pShaderItem = CGraphics::GetSingleton ().GetRenderItemManager ()->GetAppliedShaderForD3DData ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );

    // Don't apply if a vertex shader is already being used
    if ( g_pDeviceState->VertexShader )
        pShaderItem = NULL;

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
        // No shader for this texture
        return DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
    }
    else
    {
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

        // Do shader passes
        ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

        DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
        uint uiNumPasses = 0;
        pD3DEffect->Begin ( &uiNumPasses, dwFlags );

        for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
        {
            pD3DEffect->BeginPass ( uiPass );
            DrawIndexedPrimitiveGuarded ( pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
            pD3DEffect->EndPass ();
        }
        pD3DEffect->End ();

        // If we didn't get the effect to save the shader state, clear some things here
        if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
        {
            pDevice->SetVertexShader( NULL );
            pDevice->SetPixelShader( NULL );
        }

        // Unset additional vertex stream
        CAdditionalVertexStreamManager::GetSingleton ()->MaybeUnsetAdditionalVertexStream ();

        g_pDeviceState->CallState.strShaderName = "";
        return D3D_OK;
    }
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
bool AreVertexStreamsAreBigEnough ( IDirect3DDevice9* pDevice, WORD viMinBased, WORD viMaxBased )
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
// DrawPrimitiveGuarded
//
// Catch access violations
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::DrawPrimitiveGuarded ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount )
{
    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    if ( PrimitiveType == D3DPT_TRIANGLELIST || PrimitiveType == D3DPT_TRIANGLESTRIP )
    {
        UINT NumVertices = PrimitiveCount + 2;
        if ( PrimitiveType == D3DPT_TRIANGLELIST )
            NumVertices = PrimitiveCount * 3;

        WORD viMinBased = StartVertex;
        WORD viMaxBased = NumVertices + StartVertex;

        if ( !AreVertexStreamsAreBigEnough ( pDevice, viMinBased, viMaxBased ) )
            return hr;
    }

    __try
    {
        hr = pDevice->DrawPrimitive ( PrimitiveType, StartVertex, PrimitiveCount );
    }
    __except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
    {
        CCore::GetSingleton ().OnCrashAverted ( 100 );
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
    HRESULT hr = D3D_OK;

    // Check vertices used will be within the supplied vertex buffer bounds
    WORD viMinBased = MinVertexIndex + BaseVertexIndex;
    WORD viMaxBased = MinVertexIndex + NumVertices + BaseVertexIndex;

    if ( !AreVertexStreamsAreBigEnough ( pDevice, viMinBased, viMaxBased ) )
        return hr;

    __try
    {
        hr = pDevice->DrawIndexedPrimitive ( PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
    }
    __except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
    {
        CCore::GetSingleton ().OnCrashAverted ( 101 );
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

    hr = pDevice->CreateVertexBuffer ( Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle );
    if ( FAILED(hr) )
        return hr;

    // Create proxy
	*ppVertexBuffer = new CProxyDirect3DVertexBuffer ( pDevice, *ppVertexBuffer, Length, Usage, FVF, Pool );
    return hr;
}


/////////////////////////////////////////////////////////////
//
// CDirect3DEvents9::SetStreamSource
//
// Ensures the correct object gets sent to D3D
//
/////////////////////////////////////////////////////////////
HRESULT CDirect3DEvents9::SetStreamSource(IDirect3DDevice9 *pDevice, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
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

	return pDevice->SetStreamSource( StreamNumber, pStreamData, OffsetInBytes, Stride );
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
        return hr;

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

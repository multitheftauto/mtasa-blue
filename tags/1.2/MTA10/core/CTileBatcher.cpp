/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTileBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*
*****************************************************************************/

#include <StdInc.h>
#include "CTileBatcher.h"

////////////////////////////////////////////////////////////////
//
// CTileBatcher::CTileBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTileBatcher::CTileBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::~CTileBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTileBatcher::~CTileBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;

    // Cache matrices
    UpdateMatrices ( fViewportSizeX, fViewportSizeY );
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    Flush ();

    // Make new projection transform
    UpdateMatrices ( uiNewViewportSizeX, uiNewViewportSizeY );
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::UpdateMatrices ( float fViewportSizeX, float fViewportSizeY)
{
    D3DXMatrixIdentity ( &m_MatView );
    D3DXMatrixIdentity ( &m_MatProjection );

    // Make projection 3D friendly, so shaders can alter the z coord for fancy effects
    float fFarPlane = 1000;
    float fNearPlane = 10;
    float Q = fFarPlane / ( fFarPlane - fNearPlane );
    float fAdjustZFactor = 20.f;
    float rcpSizeX = 2.0f / fViewportSizeX;
    float rcpSizeY = -2.0f / fViewportSizeY;
    rcpSizeX *= fAdjustZFactor;
    rcpSizeY *= fAdjustZFactor;

    m_MatProjection.m[0][0] = rcpSizeX;
    m_MatProjection.m[1][1] = rcpSizeY;
    m_MatProjection.m[2][2] = Q;
    m_MatProjection.m[2][3] = 1;
    m_MatProjection.m[3][0] = ( -fViewportSizeX / 2.0f - 0.5f ) * rcpSizeX;
    m_MatProjection.m[3][1] = ( -fViewportSizeY / 2.0f - 0.5f ) * rcpSizeY;
    m_MatProjection.m[3][2] = -Q * fNearPlane;
    m_MatProjection.m[3][3] = 0;

    m_MatView.m[3][2] = fAdjustZFactor;
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CTileBatcher::Flush ( void )
{
    if( !m_Vertices.empty() )
    {
        //
        // TODO - Optimize all this
        //

        // Save state
        LPDIRECT3DSTATEBLOCK9   m_pDeviceState = NULL;
#if 0   // Creating a state block here may not be necessary
        m_pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pDeviceState );
#endif

        // Set states
        m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE );
        m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
        m_pDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
        m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pDevice->SetRenderState( D3DRS_ALPHAREF,         0x01 );
        m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );


        // Set transforms
        D3DXMATRIX matWorld;
        if ( m_fCurrentRotation == 0.0f )
        {
            // No rotation
            D3DXMatrixIdentity ( &matWorld );
        }
        else
        {
            // Yes rotation
            D3DXMATRIX MatToCOR, MatRot, MatFromCOR;
            D3DXMatrixTranslation ( &MatToCOR, -m_fCurrentRotCenX, -m_fCurrentRotCenY, 0 );
            D3DXMatrixRotationZ ( &MatRot, m_fCurrentRotation * (6.2832f/360.f) );
            D3DXMatrixTranslation ( &MatFromCOR, m_fCurrentRotCenX, m_fCurrentRotCenY, 0 );

            // Mout = Mrc-1 * Mr * Mrc
            matWorld = MatToCOR * MatRot * MatFromCOR;
        }

        m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );
        m_pDevice->SetTransform ( D3DTS_VIEW, &m_MatView );
        m_pDevice->SetTransform ( D3DTS_PROJECTION, &m_MatProjection );

        // Set vertex stream
        uint PrimitiveCount                 = m_Indices.size () / 3;
        const void* pIndexData              = &m_Indices[0];
        uint NumVertices                    = m_Indices.size ();
        const void* pVertexStreamZeroData   = &m_Vertices[0];
        uint VertexStreamZeroStride         = sizeof(SPDTVertex);
        m_pDevice->SetFVF ( SPDTVertex::FVF );

        // Draw
        if ( CTextureItem* pTextureItem = DynamicCast < CTextureItem > ( m_pCurrentMaterial ) )
        {
            // Draw using texture
            m_pDevice->SetTexture( 0, pTextureItem->m_pD3DTexture );
            m_pDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride );
        }
        else
        if ( CShaderInstance* pShaderInstance = DynamicCast < CShaderInstance > ( m_pCurrentMaterial ) )
        {
            // Draw using shader
            ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

            // Apply custom parameters
            pShaderInstance->ApplyShaderParameters ();
            // Apply common parameters
            pShaderInstance->m_pEffectWrap->ApplyCommonHandles ();
            // Apply mapped parameters
            pShaderInstance->m_pEffectWrap->ApplyMappedHandles ();

            // Do shader passes
            DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
            uint uiNumPasses = 0;
            pD3DEffect->Begin ( &uiNumPasses, dwFlags );

            for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
            {
                pD3DEffect->BeginPass ( uiPass );
                m_pDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride );
                pD3DEffect->EndPass ();
            }
            pD3DEffect->End ();

            // If we didn't get the effect to save the shader state, clear some things here
            if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
            {
                m_pDevice->SetVertexShader( NULL );
                m_pDevice->SetPixelShader( NULL );
            }
        }

        // Clean up
        m_Indices.clear ();
        m_Vertices.clear ();
        SetCurrentMaterial ( NULL );
        m_fCurrentRotation = 0;
        m_fCurrentRotCenX = 0;
        m_fCurrentRotCenY = 0;

        // Restore state
        if ( m_pDeviceState )
        {
            m_pDeviceState->Apply ( );
            m_pDeviceState->Release ( );
            m_pDeviceState = NULL;
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::SetCurrentMaterial
//
// Safely change the current rendering material
//
////////////////////////////////////////////////////////////////
void CTileBatcher::SetCurrentMaterial ( CMaterialItem* pMaterial )
{
    if ( m_pCurrentMaterial )
        m_pCurrentMaterial->Release ();

    m_pCurrentMaterial = pMaterial;

    if ( m_pCurrentMaterial )
        m_pCurrentMaterial->AddRef ();
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::AddTile
//
// Add a new tile to the buffer.
// Will flush previous tiles if the material is changing and such
//
////////////////////////////////////////////////////////////////
void CTileBatcher::AddTile ( float fX1, float fY1,
                              float fX2, float fY2,
                              float fU1, float fV1,
                              float fU2, float fV2, 
                              CMaterialItem* pMaterial,
                              float fRotation,
                              float fRotCenOffX,
                              float fRotCenOffY,
                              unsigned long ulColor )
{
    // Get tessellation setting from shader
    uint uiTessellationX = 1;
    uint uiTessellationY = 1;
    if ( CShaderInstance* pShaderInstance = DynamicCast < CShaderInstance > ( pMaterial ) )
    {
        // Validate tessellation range (ensure x*y < 65536)
        uiTessellationX = Clamp < uint > ( 1, pShaderInstance->m_uiTessellationX, 4000 );
        uiTessellationY = Clamp < uint > ( 1, pShaderInstance->m_uiTessellationY, 4000 );

        uint uiNumVertices = ( uiTessellationX + 1 ) * ( uiTessellationY + 1 );

        if ( uiNumVertices > 65535 )
        {
            uint div = uiNumVertices / 6553 + 1;

            if ( uiTessellationX > uiTessellationY )
                uiTessellationX = uiTessellationX / div * 10;
            else
                uiTessellationY = uiTessellationY / div * 10;
        }
    }

    // Calc number of vertices to add
    uint uiNumVertices = ( uiTessellationX + 1 ) * ( uiTessellationY + 1 );

    // Calc position of rotation center
    float fRotCenX = fRotCenOffX + ( fX1 + fX2 ) * 0.5f;
    float fRotCenY = fRotCenOffY + ( fY1 + fY2 ) * 0.5f;

    // Check if we need to flush what has been done so far
    if ( pMaterial != m_pCurrentMaterial
            || fRotation != m_fCurrentRotation
            || fRotCenX != m_fCurrentRotCenX
            || fRotCenY != m_fCurrentRotCenY
            || uiNumVertices + m_Vertices.size () > 65535
            )
    {
        Flush ();
        SetCurrentMaterial ( pMaterial );
        m_fCurrentRotation = fRotation;
        m_fCurrentRotCenX = fRotCenX;
        m_fCurrentRotCenY = fRotCenY;
    }

    // Do quicker things if tessellation is not required
    if ( uiTessellationX == 1 && uiTessellationY == 1 )
    {
        // Make room for 4 more vertices
        m_Vertices.resize ( m_Vertices.size () + 4 );
        SPDTVertex* pVBuffer = &m_Vertices[m_Vertices.size () - 4];
    
        // Fill vertex data
        WRITE_PDT_VERTEX( pVBuffer, fX1, fY1, 0, ulColor, fU1 , fV1 );
        WRITE_PDT_VERTEX( pVBuffer, fX2, fY1, 0, ulColor, fU2 , fV1 );
        WRITE_PDT_VERTEX( pVBuffer, fX1, fY2, 0, ulColor, fU1 , fV2 );
        WRITE_PDT_VERTEX( pVBuffer, fX2, fY2, 0, ulColor, fU2 , fV2 );

        // Make room for 6 more indices
        uint uiBaseIndex = m_Indices.size ();
        m_Indices.resize ( m_Indices.size () + 6 );
        WORD* pIBuffer = &m_Indices[m_Indices.size () - 6];

        WRITE_QUAD_INDICES( pIBuffer, uiBaseIndex, uiBaseIndex + 2 );
        return;
    }

    // Write out the vertices
    {
        // Make room for vertices
        m_Vertices.resize ( m_Vertices.size () + uiNumVertices );
        SPDTVertex* pVBuffer = &m_Vertices[m_Vertices.size () - uiNumVertices];

        float fStepX = ( fX2 - fX1 ) / (float)uiTessellationX;
        float fStepY = ( fY2 - fY1 ) / (float)uiTessellationY;

        float fStepU = ( fU2 - fU1 ) / (float)uiTessellationX;
        float fStepV = ( fV2 - fV1 ) / (float)uiTessellationY;

        for ( uint y = 0 ; y < uiTessellationY + 1 ; y++ )
        {
            float fY = fY1 + fStepY * y;
            float fV = fV1 + fStepV * y;

            for ( uint x = 0 ; x < uiTessellationX + 1 ; x++ )
            {
                float fX = fX1 + fStepX * x;
                float fU = fU1 + fStepU * x;

                WRITE_PDT_VERTEX( pVBuffer, fX, fY, 0, ulColor, fU, fV );
            }
        }
    }

    // Write out the indices
    {
        uint uiNumIndices = uiTessellationX * uiTessellationY * 6;

        // Make room for indices
        m_Indices.resize ( m_Indices.size () + uiNumIndices );
        WORD* pIBuffer = &m_Indices[m_Indices.size () - uiNumIndices];

        for ( uint y = 0 ; y < uiTessellationY ; y++ )
        {
            uint uiRow0Base = y * ( uiTessellationX + 1 );
            uint uiRow1Base = ( y + 1 ) * ( uiTessellationX + 1 );

            for ( uint x = 0 ; x < uiTessellationX ; x++ )
            {
                WRITE_QUAD_INDICES( pIBuffer, uiRow0Base + x, uiRow1Base + x );
            }
        }
    }
}

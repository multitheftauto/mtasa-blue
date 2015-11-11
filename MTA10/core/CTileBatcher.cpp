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
#define DEG2RAD(deg) ( (deg) * (6.2832f/360.f) )

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
    m_fViewportSizeX = fViewportSizeX;
    m_fViewportSizeY = fViewportSizeY;

    D3DXMatrixIdentity ( &m_MatView );
    D3DXMatrixIdentity ( &m_MatProjection );

    // Make projection 3D friendly, so shaders can alter the z coord for fancy effects
    float fFarPlane = 10000;
    float fNearPlane = 100;
    float Q = fFarPlane / ( fFarPlane - fNearPlane );
    float fAdjustZFactor = 1000.f;
    float rcpSizeX = 2.0f / m_fViewportSizeX;
    float rcpSizeY = -2.0f / m_fViewportSizeY;
    rcpSizeX *= fAdjustZFactor;
    rcpSizeY *= fAdjustZFactor;

    m_MatProjection.m[0][0] = rcpSizeX;
    m_MatProjection.m[1][1] = rcpSizeY;
    m_MatProjection.m[2][2] = Q;
    m_MatProjection.m[2][3] = 1;
    m_MatProjection.m[3][0] = ( -m_fViewportSizeX / 2.0f - 0.5f ) * rcpSizeX;
    m_MatProjection.m[3][1] = ( -m_fViewportSizeY / 2.0f - 0.5f ) * rcpSizeY;
    m_MatProjection.m[3][2] = -Q * fNearPlane;
    m_MatProjection.m[3][3] = 0;

    m_MatView.m[3][2] = fAdjustZFactor;
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::OnZBufferModified
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::OnZBufferModified ( void )
{
    m_bZBufferDirty = true;
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

        if ( m_bUseCustomMatrices && m_bZBufferDirty )
        {
            // Shaders with transforms will probably need a clear zbuffer
            m_pDevice->Clear ( 0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1, 0 );
            m_bZBufferDirty = false;
        }

        // Set states
        if ( g_pDeviceState->AdapterState.bRequiresClipping )
            m_pDevice->SetRenderState ( D3DRS_CLIPPING, TRUE );
        m_pDevice->SetRenderState ( D3DRS_ZENABLE, m_bUseCustomMatrices ? D3DZB_TRUE : D3DZB_FALSE );
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


        if ( m_bUseCustomMatrices )
        {
            m_pDevice->SetTransform ( D3DTS_WORLD, &m_MatCustomWorld );
            m_pDevice->SetTransform ( D3DTS_VIEW, &m_MatView );
            m_pDevice->SetTransform ( D3DTS_PROJECTION, &m_MatCustomProjection );
        }
        else
        {
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
        }


        // Set vertex stream
        uint PrimitiveCount                 = m_Indices.size () / 3;
        const void* pIndexData              = &m_Indices[0];
        uint NumVertices                    = m_Vertices.size ();
        const void* pVertexStreamZeroData   = &m_Vertices[0];
        uint VertexStreamZeroStride         = sizeof(SPDTVertex);
        m_pDevice->SetFVF ( SPDTVertex::FVF );

        // Set texture addressing mode
        m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, m_pCurrentMaterial->m_TextureAddress );
        m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, m_pCurrentMaterial->m_TextureAddress );

        if ( m_pCurrentMaterial->m_TextureAddress == TADDRESS_BORDER )
            m_pDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, m_pCurrentMaterial->m_uiBorderColor );

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
            pShaderInstance->m_pEffectWrap->Begin ( &uiNumPasses, dwFlags, false );

            for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
            {
                pD3DEffect->BeginPass ( uiPass );
                m_pDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride );
                pD3DEffect->EndPass ();
            }
            pShaderInstance->m_pEffectWrap->End ();

            // If we didn't get the effect to save the shader state, clear some things here
            if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
            {
                m_pDevice->SetVertexShader( NULL );
                m_pDevice->SetPixelShader( NULL );
            }
        }

        // Clean up
        ListClearAndReserve ( m_Indices );
        ListClearAndReserve ( m_Vertices );

        SetCurrentMaterial ( NULL );
        m_fCurrentRotation = 0;
        m_fCurrentRotCenX = 0;
        m_fCurrentRotCenY = 0;

        if ( g_pDeviceState->AdapterState.bRequiresClipping )
            m_pDevice->SetRenderState ( D3DRS_CLIPPING, FALSE );
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
    // Flush previous if it was using a shader transform
    if ( m_bUseCustomMatrices )
        Flush ();

    m_bUseCustomMatrices = false;
    uint uiTessellationX = 1;
    uint uiTessellationY = 1;

    // Get settings from shader
    if ( CShaderInstance* pShaderInstance = DynamicCast < CShaderInstance > ( pMaterial ) )
    {
        // Get transform setting 
        if ( pShaderInstance && pShaderInstance->m_bHasModifiedTransform )
        {
            MakeCustomMatrices ( pShaderInstance->m_Transform, fX1, fY1, fX2, fY2, m_MatCustomWorld, m_MatCustomProjection );
            m_bUseCustomMatrices = true;
        }

        // Get tessellation setting
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
    float fRotCenX = 0;
    float fRotCenY = 0;
    if ( fRotation != 0.0f )
    {
        fRotCenX = fRotCenOffX + ( fX1 + fX2 ) * 0.5f;
        fRotCenY = fRotCenOffY + ( fY1 + fY2 ) * 0.5f;
    }

    // Check if we need to flush what has been done so far
    if ( pMaterial != m_pCurrentMaterial
            || fRotation != m_fCurrentRotation
            || fRotCenX != m_fCurrentRotCenX
            || fRotCenY != m_fCurrentRotCenY
            || uiNumVertices + m_Vertices.size () > 65535
            || m_bUseCustomMatrices
            )
    {
        Flush ();
        SetCurrentMaterial ( pMaterial );
        m_fCurrentRotation = fRotation;
        m_fCurrentRotCenX = fRotCenX;
        m_fCurrentRotCenY = fRotCenY;
    }

    uint uiBaseIndex = m_Vertices.size ();

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
                WRITE_QUAD_INDICES( pIBuffer, uiBaseIndex + uiRow0Base + x, uiBaseIndex + uiRow1Base + x );
            }
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::MakeCustomMatrices
//
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::MakeCustomMatrices ( const SShaderTransform& t
                                        ,float fX1, float fY1
                                        ,float fX2, float fY2
                                        ,D3DXMATRIX& matOutWorld
                                        ,D3DXMATRIX& matOutProjection )
{
    CVector vecCenterOfTile = ( CVector( fX1, fY1, 0 ) + CVector( fX2, fY2, 0 ) ) * 0.5f; 
    CVector vecCenterOfScreen = CVector( m_fViewportSizeX, m_fViewportSizeY, 0 ) * 0.5f;
    CVector vecHalfScreenSize = CVector( m_fViewportSizeX, m_fViewportSizeY, 1000 ) * 0.5f;

    // Rotation
    CVector useCenOfRot = t.vecRotCenOffset * vecHalfScreenSize;
    if ( t.bRotCenOffsetOriginIsScreen )
    {
        useCenOfRot += vecCenterOfScreen;
    }
    else
    {
        useCenOfRot += vecCenterOfTile;
    }

    D3DXMATRIX matToCOR, matRot, matFromCOR;
    D3DXMatrixTranslation ( &matToCOR, -useCenOfRot.fX, -useCenOfRot.fY, -useCenOfRot.fZ );
    D3DXMatrixRotationYawPitchRoll ( &matRot, DEG2RAD(t.vecRot.fX), DEG2RAD(t.vecRot.fY), DEG2RAD(t.vecRot.fZ) );
    D3DXMatrixTranslation ( &matFromCOR, useCenOfRot.fX, useCenOfRot.fY, useCenOfRot.fZ );

    matOutWorld = matToCOR * matRot * matFromCOR;


    // Perspective
    CVector2D useCenOfPers = t.vecPersCenOffset * vecHalfScreenSize;
    if ( !t.bPersCenOffsetOriginIsScreen )
    {
        useCenOfPers += vecCenterOfTile - vecCenterOfScreen;
    }

    D3DXMATRIX matFromCOP;
    D3DXMATRIX matToCOP;
    D3DXMatrixIdentity ( &matFromCOP );

    matFromCOP.m[3][0] = useCenOfPers.fX * ( 2.0f / m_fViewportSizeX );
    matFromCOP.m[3][1] = -useCenOfPers.fY * ( 2.0f / m_fViewportSizeY );
    D3DXMatrixInverse ( &matToCOP, NULL, &matFromCOP );

    matOutProjection = matToCOP * m_MatProjection * matFromCOP;

    // Use world transform to centralize perspective offset
    D3DXMATRIX matCentralize;
    D3DXMatrixTranslation ( &matCentralize, -useCenOfPers.fX, -useCenOfPers.fY, 0 );
    matOutWorld = matOutWorld * matCentralize;
}

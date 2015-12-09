/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CMaterialLine3DBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*
*****************************************************************************/

#include <StdInc.h>
#include "CTileBatcher.h"
#include "CMaterialLine3DBatcher.h"

SMaterialLine3DItem* CMaterialLine3DBatcher::ms_pLines = NULL;


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::CMaterialLine3DBatcher
//
//
//
////////////////////////////////////////////////////////////////
CMaterialLine3DBatcher::CMaterialLine3DBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::~CMaterialLine3DBatcher
//
//
//
////////////////////////////////////////////////////////////////
CMaterialLine3DBatcher::~CMaterialLine3DBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CMaterialLine3DBatcher::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;
}


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::Flush
//
// Draw all pending lines
//
////////////////////////////////////////////////////////////////
void CMaterialLine3DBatcher::Flush ( void )
{
    if( m_LineList.empty() )
        return;

    // Get scene matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity ( &matWorld );
    const D3DXMATRIX& matView = g_pDeviceState->TransformState.VIEW;
    const D3DXMATRIX& matProjection = g_pDeviceState->TransformState.PROJECTION;

    // Pre-calc camera position
    D3DXMATRIX matViewInv;
    D3DXMatrixInverse ( &matViewInv, NULL, &matView );
    const CVector vecCameraPos ( matViewInv._41, matViewInv._42, matViewInv._43 );

    // Set transforms
    m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );
    m_pDevice->SetTransform ( D3DTS_VIEW, &matView );
    m_pDevice->SetTransform ( D3DTS_PROJECTION, &matProjection );

    // Set render states
    if ( g_pDeviceState->AdapterState.bRequiresClipping )
        m_pDevice->SetRenderState ( D3DRS_CLIPPING, TRUE );
    m_pDevice->SetRenderState ( D3DRS_ZENABLE,          D3DZB_TRUE );
    m_pDevice->SetRenderState ( D3DRS_ZFUNC,            D3DCMP_LESSEQUAL );
    m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE,     FALSE );
    m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
    m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x01 );
    m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
    m_pDevice->SetRenderState ( D3DRS_LIGHTING,         FALSE);
    m_pDevice->SetRenderState ( D3DRS_FOGENABLE,        TRUE);
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    m_CurrentTextureAddress = (ETextureAddress)0;


    //
    // Sort by distance
    //

    const float fMaxDrawDistance = CCore::GetSingleton().GetMultiplayer()->GetFarClipDistance();
    const float fSmallLineTolerance = 20;
    const float fMaxDrawDistanceIncTol = fMaxDrawDistance + fSmallLineTolerance;

    const float fMaxDrawDistanceSq = fMaxDrawDistance * fMaxDrawDistance;
    const float fMaxDrawDistanceIncTolSq = fMaxDrawDistanceIncTol * fMaxDrawDistanceIncTol;
    const float fSmallLineToleranceSq = fSmallLineTolerance * fSmallLineTolerance;

    const uint uiNumLines = m_LineList.size ();
    std::vector < uint > indexList;
    indexList.resize ( uiNumLines );
    uint* pIndices = &indexList [ 0 ];
    uint uiNumIndicies = 0;

    // Calculate distances
    for ( uint i = 0 ; i < uiNumLines ; i++ )
    {
        SMaterialLine3DItem& item = m_LineList [ i ];
        item.fDistSq = ( item.vecAvgPosition - vecCameraPos ).LengthSquared ();
        if ( item.fDistSq > fMaxDrawDistanceIncTolSq )
        {
            // Check if line bounding sphere is visible
            
            float fRadiusSq = ( item.vecTo - item.vecFrom ).LengthSquared ();
            if ( fRadiusSq < fSmallLineToleranceSq )
                continue;

            float fSphereDist = Max ( 0.f, ( item.vecAvgPosition - vecCameraPos ).Length () - sqrt ( fRadiusSq ) );
            float fSphereDistSq = fSphereDist * fSphereDist;
            if ( fSphereDistSq > fMaxDrawDistanceSq )
                continue;
        }
        // Add the index for this line to the draw list
        pIndices [ uiNumIndicies++ ] = i;
    }

    if ( uiNumIndicies > 0 )
    {
        indexList.resize ( uiNumIndicies );

        // Sort index list by distance
        ms_pLines = &m_LineList [ 0 ];
        sort_inline ( indexList.begin (), indexList.end (), ( const uint a, const uint b ) { return ms_pLines [ a ].fDistSq > ms_pLines [ b ].fDistSq; } );

        pIndices = &indexList [ 0 ];

        // Draw index list, batching by material
        uint uiBatchFirstIndex = 0;
        for ( uint i = 0 ; i < uiNumIndicies ; i++ )
        {
            const SMaterialLine3DItem& item = ms_pLines [ pIndices [ i ] ];

            // Flush batch if this is the last index, or the next one uses a different material
            if ( ( i == uiNumIndicies - 1 ) || item.pMaterial != ms_pLines [ pIndices [ i + 1 ] ].pMaterial )
            {
                uint* pBatchIndices = &pIndices [ uiBatchFirstIndex ];
                uint uiNumBatchLines = i - uiBatchFirstIndex + 1;

                DrawBatch ( vecCameraPos, pBatchIndices, uiNumBatchLines, item.pMaterial );

                uiBatchFirstIndex = i + 1;
            }
        }
    }

    // Clean up
    for ( uint i = 0 ; i < uiNumLines ; i++ )
        m_LineList [ i ].pMaterial->Release ();

    ListClearAndReserve ( m_LineList );

    if ( g_pDeviceState->AdapterState.bRequiresClipping )
        m_pDevice->SetRenderState ( D3DRS_CLIPPING, FALSE );
}


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::DrawBatch
//
// Create vertices and draw
//
////////////////////////////////////////////////////////////////
void CMaterialLine3DBatcher::DrawBatch ( const CVector& vecCameraPos, uint* pBatchIndices, uint uiNumBatchLines, CMaterialItem* pMaterial )
{
    // Prepare vertex buffer
    std::vector < SPDTVertex > vertices;
    vertices.resize ( uiNumBatchLines * 6 );
    SPDTVertex* pBuffer = &vertices[0];

    // For each line
    for ( uint i = 0 ; i < uiNumBatchLines ; i++ )
    {
        const SMaterialLine3DItem& item = m_LineList [ pBatchIndices [ i ] ];

        SColor color = item.ulColor;
        color.R /= 2;
        color.G /= 2;
        color.B /= 2;
        const ulong ulColor = color;
        const CVector& vecA = item.vecFrom;
        const CVector& vecB = item.vecTo;

        const CVector& vecFaceToward = item.bUseFaceToward ? item.vecFaceToward : vecCameraPos;

        // Face toward supplied point
        const CVector vecDif = vecB - vecA;
        const CVector vecToCam = vecFaceToward - vecA;
        const float t = vecDif.DotProduct ( &vecToCam ) / vecDif.DotProduct ( &vecDif );
        const CVector vecClosestPoint = vecA + vecDif * t;
        const CVector vecToLine = vecClosestPoint - vecFaceToward;

        // Calc other direction
        CVector vecLeft = vecDif;
        vecLeft.CrossProduct ( &vecToLine );
        vecLeft.Normalize ();

        // Create rectangle points
        const CVector vecShift = vecLeft * ( item.fWidth * 0.5f );
        const CVector vecA2 = item.vecFrom + vecShift;
        const CVector vecB2 = item.vecTo   + vecShift;
        const CVector vecA1 = item.vecFrom - vecShift;
        const CVector vecB1 = item.vecTo   - vecShift;

        WRITE_PDT_VERTEX( pBuffer, vecA1.fX, vecA1.fY, vecA1.fZ, ulColor, item.fU1, item.fV1 );
        WRITE_PDT_VERTEX( pBuffer, vecA2.fX, vecA2.fY, vecA2.fZ, ulColor, item.fU2, item.fV1 );
        WRITE_PDT_VERTEX( pBuffer, vecB1.fX, vecB1.fY, vecB1.fZ, ulColor, item.fU1, item.fV2 );

        WRITE_PDT_VERTEX( pBuffer, vecA2.fX, vecA2.fY, vecA2.fZ, ulColor, item.fU2, item.fV1 );
        WRITE_PDT_VERTEX( pBuffer, vecB2.fX, vecB2.fY, vecB2.fZ, ulColor, item.fU2, item.fV2 );
        WRITE_PDT_VERTEX( pBuffer, vecB1.fX, vecB1.fY, vecB1.fZ, ulColor, item.fU1, item.fV2 );
    }

    // Set vertex stream
    uint PrimitiveCount                 = vertices.size () / 3;
    const void* pVertexStreamZeroData   = &vertices[0];
    uint VertexStreamZeroStride         = sizeof(SPDTVertex);
    m_pDevice->SetFVF ( SPDTVertex::FVF );

    // Change texture addressing mode if required
    if ( m_CurrentTextureAddress != pMaterial->m_TextureAddress )
    {
        m_CurrentTextureAddress = pMaterial->m_TextureAddress;
        m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, m_CurrentTextureAddress );
        m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, m_CurrentTextureAddress );
    }

    if ( m_CurrentTextureAddress == TADDRESS_BORDER )
        m_pDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, pMaterial->m_uiBorderColor );

    // Draw
    if ( CTextureItem* pTextureItem = DynamicCast < CTextureItem > ( pMaterial ) )
    {
        // Draw using texture
        m_pDevice->SetTexture( 0, pTextureItem->m_pD3DTexture );
        m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
    }
    else
    if ( CShaderInstance* pShaderInstance = DynamicCast < CShaderInstance > ( pMaterial ) )
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
        pShaderInstance->m_pEffectWrap->Begin ( &uiNumPasses, dwFlags );

        for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
        {
            pD3DEffect->BeginPass ( uiPass );
            m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
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
}


////////////////////////////////////////////////////////////////
//
// CMaterialLine3DBatcher::AddLine3D
//
// Add a new line3d to the list
//
////////////////////////////////////////////////////////////////
void CMaterialLine3DBatcher::AddLine3D ( const CVector& vecFrom,
                                      const CVector& vecTo,
                                      float fWidth,
                                      ulong ulColor,
                                      CMaterialItem* pMaterial,
                                      float fU, float fV,
                                      float fSizeU, float fSizeV, 
                                      bool bRelativeUV,
                                      bool bUseFaceToward,
                                      const CVector& vecFaceToward )
{
    if ( !pMaterial )
        return;

    SMaterialLine3DItem item;
    item.vecFrom = vecFrom;
    item.vecTo = vecTo;
    item.vecAvgPosition = ( vecFrom + vecTo ) * 0.5f;
    item.fWidth = fWidth;
    item.ulColor = ulColor;
    item.bUseFaceToward = bUseFaceToward;
    item.vecFaceToward = vecFaceToward;
    item.pMaterial = pMaterial;
    pMaterial->AddRef ();

    item.fU1 = fU;
    item.fV1 = fV;
    item.fU2 = fU + fSizeU;
    item.fV2 = fV + fSizeV;
    if ( !bRelativeUV )
    {
        // If UV's are absolute pixels, then scale the range to 0.0f - 1.0f.
        float fUScale = 1.0f / (float)pMaterial->m_uiSizeX;
        float fVScale = 1.0f / (float)pMaterial->m_uiSizeY;
        item.fU1 *= fUScale;
        item.fV1 *= fVScale;
        item.fU2 *= fUScale;
        item.fV2 *= fVScale;
    }

    m_LineList.push_back ( item );
}

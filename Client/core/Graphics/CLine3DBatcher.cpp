/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CLine3DBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*
*****************************************************************************/

#include <StdInc.h>
#include "CLine3DBatcher.h"

////////////////////////////////////////////////////////////////
//
// CLine3DBatcher::CLine3DBatcher
//
//
//
////////////////////////////////////////////////////////////////
CLine3DBatcher::CLine3DBatcher ( bool bZTest )
{
    m_bZTest = bZTest;
}


////////////////////////////////////////////////////////////////
//
// CLine3DBatcher::~CLine3DBatcher
//
//
//
////////////////////////////////////////////////////////////////
CLine3DBatcher::~CLine3DBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CLine3DBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CLine3DBatcher::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;
}


////////////////////////////////////////////////////////////////
//
// CLine3DBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CLine3DBatcher::Flush ( void )
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

    // Prepare vertex buffer
    std::vector < SPDVertex > vertices;
    vertices.resize ( m_LineList.size () * 6 );
    SPDVertex* pBuffer = &vertices[0];

    // For each line
    for ( std::vector < SLine3DItem > ::iterator iter = m_LineList.begin () ; iter != m_LineList.end () ; ++iter )
    {
        const SLine3DItem& item = *iter;

        const float fWidth = item.fWidth;
        ulong ulColor = item.ulColor;
        const CVector& vecA = item.vecFrom;
        const CVector& vecB = item.vecTo;

        // Get direction to closest point on the line
        const CVector vecDif = vecB - vecA;
        const CVector vecToCam = vecCameraPos - vecA;
        const float t = vecDif.DotProduct ( &vecToCam ) / vecDif.DotProduct ( &vecDif );
        const CVector vecClosestPoint = vecA + vecDif * t;
        const CVector vecToLine = vecClosestPoint - vecCameraPos;

        // Calc other direction
        CVector vecUp = vecDif;
        vecUp.CrossProduct ( &vecToLine );
        vecUp.Normalize ();

        // Create rectangle points
        const CVector vecShift = vecUp * ( fWidth * 0.5f );
        const CVector vecA2 = vecA + vecShift;
        const CVector vecB2 = vecB + vecShift;
        const CVector vecA1 = vecA - vecShift;
        const CVector vecB1 = vecB - vecShift;

        WRITE_PD_VERTEX( pBuffer, vecA1.fX, vecA1.fY, vecA1.fZ, ulColor );
        WRITE_PD_VERTEX( pBuffer, vecA2.fX, vecA2.fY, vecA2.fZ, ulColor );
        WRITE_PD_VERTEX( pBuffer, vecB1.fX, vecB1.fY, vecB1.fZ, ulColor );

        WRITE_PD_VERTEX( pBuffer, vecA2.fX, vecA2.fY, vecA2.fZ, ulColor );
        WRITE_PD_VERTEX( pBuffer, vecB2.fX, vecB2.fY, vecB2.fZ, ulColor );
        WRITE_PD_VERTEX( pBuffer, vecB1.fX, vecB1.fY, vecB1.fZ, ulColor );
    }

    // Set states
    if ( g_pDeviceState->AdapterState.bRequiresClipping )
        m_pDevice->SetRenderState ( D3DRS_CLIPPING, TRUE );
    m_pDevice->SetRenderState ( D3DRS_ZENABLE, m_bZTest ? D3DZB_TRUE : D3DZB_FALSE );
    m_pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
    m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
    m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x01 );
    m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
    m_pDevice->SetRenderState ( D3DRS_LIGHTING,         FALSE);
    m_pDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,    D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,    D3DTOP_DISABLE );
    m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );

    // Set transforms
    m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );
    m_pDevice->SetTransform ( D3DTS_VIEW, &matView );
    m_pDevice->SetTransform ( D3DTS_PROJECTION, &matProjection );

    // Set vertex stream
    uint PrimitiveCount                 = vertices.size () / 3;
    const void* pVertexStreamZeroData   = &vertices[0];
    uint VertexStreamZeroStride         = sizeof(SPDVertex);
    m_pDevice->SetFVF ( SPDVertex::FVF );

    // Draw
    m_pDevice->SetTexture( 0, NULL );
    m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );

    // Clean up
    size_t prevSize = m_LineList.size ();
    m_LineList.clear ();
    m_LineList.reserve ( prevSize );

    if ( g_pDeviceState->AdapterState.bRequiresClipping )
        m_pDevice->SetRenderState ( D3DRS_CLIPPING, FALSE );
}


////////////////////////////////////////////////////////////////
//
// CLine3DBatcher::AddLine3D
//
// Add a new line3d to the list
//
////////////////////////////////////////////////////////////////
void CLine3DBatcher::AddLine3D ( const CVector& vecFrom,
                                      const CVector& vecTo,
                                      float fWidth,
                                      ulong ulColor )
{
    SLine3DItem item;
    item.vecFrom = vecFrom;
    item.vecTo = vecTo;
    item.fWidth = fWidth * ( 1 / 75.f );
    item.ulColor = ulColor;

    m_LineList.push_back ( item );
}

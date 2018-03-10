/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTriangleBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  forkerer, based on CLine3DBatcher and CTileBatcher by vidiot
*
*
*****************************************************************************/

#include <StdInc.h>
#include "CTriangleBatcher.h"

////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::CTriangleBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTriangleBatcher::CTriangleBatcher ( bool m_bZTest )
{
    m_bZTest = m_bZTest;
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::~CTriangleBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTriangleBatcher::~CTriangleBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;

    // Cache matrices
    UpdateMatrices ( fViewportSizeX, fViewportSizeY );
}

////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    Flush ();

    // Make new projection transform
    UpdateMatrices ( uiNewViewportSizeX, uiNewViewportSizeY );
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::UpdateMatrices ( float fViewportSizeX, float fViewportSizeY )
{
    m_fViewportSizeX = fViewportSizeX;
    m_fViewportSizeY = fViewportSizeY;

    D3DXMatrixIdentity ( &m_MatView );
    D3DXMatrixIdentity ( &m_MatProjection );

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
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::Flush ( void )
{
    if (m_triangleList.empty ())
        return;
    
    // Save render states
    IDirect3DStateBlock9* pSavedStateBlock = NULL;
    m_pDevice->CreateStateBlock ( D3DSBT_ALL, &pSavedStateBlock );

    // Set transformations
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity ( &matWorld );

    m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );
    m_pDevice->SetTransform ( D3DTS_VIEW, &m_MatView );
    m_pDevice->SetTransform ( D3DTS_PROJECTION, &m_MatProjection );

    // Set vertex stream
    uint PrimitiveCount = m_triangleList.size()/3;
    const void* pVertexStreamZeroData = &m_triangleList[0];
    uint VertexStreamZeroStride = sizeof ( TriangleVertice );
    m_pDevice->SetFVF ( D3DFVF_XYZ | D3DFVF_DIFFUSE );
    
    // Set states
    m_pDevice->SetRenderState ( D3DRS_ZENABLE, m_bZTest ? D3DZB_TRUE : D3DZB_FALSE );
    m_pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
    m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
    m_pDevice->SetRenderState ( D3DRS_ALPHAREF, 0x01 );
    m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
    m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    // Draw
    m_pDevice->SetTexture ( 0, NULL );
    m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );

    // Clean up
    ClearQueue ();

    // Restore render states
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply ();
        SAFE_RELEASE ( pSavedStateBlock );
    }
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::ClearQueue
//
// Clears all triangles for current queue
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::ClearQueue ()
{
    // Clean up
    size_t prevSize = m_triangleList.size ();
    m_triangleList.clear ();
    m_triangleList.reserve ( prevSize );
}


////////////////////////////////////////////////////////////////
//
// CTriangleBatcher::AddTriangle
//
// Add a new triangle to the list
//
////////////////////////////////////////////////////////////////
void CTriangleBatcher::AddTriangle ( CVector2D vecPos1,
                                     CVector2D vecPos2,
                                     CVector2D vecPos3,
                                     unsigned long ulColorVert1,
                                     unsigned long ulColorVert2,
                                     unsigned long ulColorVert3 )
{
    TriangleVertice vert1 = { vecPos1.fX,vecPos1.fY, 0, ulColorVert1 };
    TriangleVertice vert2 = { vecPos2.fX,vecPos2.fY, 0, ulColorVert2 };
    TriangleVertice vert3 = { vecPos3.fX,vecPos3.fY, 0, ulColorVert3 };

    m_triangleList.push_back ( vert1 );
    m_triangleList.push_back ( vert2 );
    m_triangleList.push_back ( vert3 );
}

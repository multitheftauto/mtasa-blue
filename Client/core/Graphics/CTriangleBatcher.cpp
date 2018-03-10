/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTriangleBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  forkerer, based on CLine3DBatcher by vidiot
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

    // Set vertex stream
    uint PrimitiveCount = m_triangleList.size()/3;
    const void* pVertexStreamZeroData = &m_triangleList[0];
    uint VertexStreamZeroStride = sizeof ( TriangleVertice );
    m_pDevice->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
    
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
    size_t prevSize = m_triangleList.size ();
    m_triangleList.clear ();
    m_triangleList.reserve ( prevSize );

    // Restore render states
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply ();
        SAFE_RELEASE ( pSavedStateBlock );
    }
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
    TriangleVertice vert1 = { vecPos1.fX,vecPos1.fY, 0, 1, ulColorVert1 };
    TriangleVertice vert2 = { vecPos2.fX,vecPos2.fY, 0, 1, ulColorVert2 };
    TriangleVertice vert3 = { vecPos3.fX,vecPos3.fY, 0, 1, ulColorVert3 };

    m_triangleList.push_back ( vert1 );
    m_triangleList.push_back ( vert2 );
    m_triangleList.push_back ( vert3 );
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.RenderTarget.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::PostConstruct ( CRenderItemManager* pManager, uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel )
{
    Super::PostConstruct ( pManager );
    m_uiSizeX = uiSizeX;
    m_uiSizeY = uiSizeY;
    m_bWithAlphaChannel = bWithAlphaChannel;

    // Initial creation of d3d data
    CreateUnderlyingData ();
}


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CRenderTargetItem::IsValid ( void )
{
    return m_pD3DTexture && m_pD3DRenderTargetSurface && m_pD3DZStencilSurface;
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::OnLostDevice ( void )
{
    ReleaseUnderlyingData ();
}


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::OnResetDevice ( void )
{
    CreateUnderlyingData ();
}


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::CreateUnderlyingData ( void )
{
    assert ( !m_pD3DRenderTargetSurface );
    assert ( !m_pD3DTexture );
    assert ( !m_pD3DZStencilSurface );

    for ( uint i = 0 ; i < 4 ; i++ )
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
	    D3DFORMAT Format = i & 1 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
        if ( m_bWithAlphaChannel )
            Format = D3DFMT_A8R8G8B8;
        if( SUCCEEDED( m_pDevice->CreateTexture( m_uiSizeX, m_uiSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&m_pD3DTexture, NULL ) ) )
            break;

        // c'mon
        if ( i == 1 )
            m_pDevice->EvictManagedResources ();
    }

    // Check texture created
    if ( !m_pD3DTexture )
        return;


    for ( uint i = 0 ; i < 4 ; i++ )
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
	    D3DFORMAT Format = i & 1 ? D3DFMT_D16 : D3DFMT_D24X8;
        if( SUCCEEDED( m_pDevice->CreateDepthStencilSurface( m_uiSizeX, m_uiSizeY, Format, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pD3DZStencilSurface, NULL ) ) )
            break;

        // c'mon c'mon
        if ( i == 1 )
            m_pDevice->EvictManagedResources ();
    }

    // Check depth buffer created
    if ( !m_pD3DZStencilSurface )
    {
        SAFE_RELEASE( m_pD3DTexture );
        return;
    }

    // Get the render target surface here for convenience
    ((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel ( 0, &m_pD3DRenderTargetSurface );

    // Clear incase it gets used before first copy
    m_pDevice->ColorFill( m_pD3DRenderTargetSurface, NULL, 0x00000000 );
}


////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::ReleaseUnderlyingData ( void )
{
    SAFE_RELEASE( m_pD3DRenderTargetSurface )
    SAFE_RELEASE( m_pD3DTexture )
    SAFE_RELEASE( m_pD3DZStencilSurface )
}

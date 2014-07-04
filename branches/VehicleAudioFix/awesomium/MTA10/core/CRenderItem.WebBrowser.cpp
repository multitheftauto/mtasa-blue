/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.WebBrowser.cpp
*  PURPOSE:     Web browser texture item class
*
*****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::PostConstruct ( CRenderItemManager* pRenderItemManager, uint uiSizeX, uint uiSizeY, bool bIsLocal )
{
    Super::PostConstruct ( pRenderItemManager );
    m_uiSizeX = uiSizeX;
    m_uiSizeY = uiSizeY;
    m_uiSurfaceSizeX = uiSizeX;
    m_uiSurfaceSizeY = uiSizeY;
    m_pWebView = NULL;

    // Initial creation of d3d data
    CreateUnderlyingData ( bIsLocal );
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CWebBrowserItem::IsValid ( void )
{
    return m_pD3DTexture && m_pD3DRenderTargetSurface && m_pWebView;
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnLostDevice ( void )
{
    
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnResetDevice ( void )
{
    
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::CreateUnderlyingData ( bool bIsLocal )
{
    assert ( !m_pD3DRenderTargetSurface );
    assert ( !m_pD3DTexture );

    D3DXCreateTexture ( m_pDevice, m_uiSizeX, m_uiSizeY, 1, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&m_pD3DTexture );
    
    // Check texture created
    if ( !m_pD3DTexture )
        return;

    // Get the render target surface here for convenience
    ((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel ( 0, &m_pD3DRenderTargetSurface );

    // Update surface size, although it probably will be unchanged | Todo: Remove this
    D3DSURFACE_DESC desc;
    m_pD3DRenderTargetSurface->GetDesc ( &desc );
    m_uiSurfaceSizeX = desc.Width;
    m_uiSurfaceSizeY = desc.Height;

    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage ( m_pD3DRenderTargetSurface );

    // Create the web view
    m_pWebView = g_pCore->GetWebCore ()->CreateWebView ( m_uiSizeX, m_uiSizeY, bIsLocal );
}


////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::ReleaseUnderlyingData ( void )
{
    SAFE_RELEASE( m_pD3DRenderTargetSurface )
    SAFE_RELEASE( m_pD3DTexture )
    
    g_pCore->GetWebCore ()->DestroyWebView ( m_pWebView );
    m_pWebView = NULL;
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItemManager.cpp
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::CRenderItemManager ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::~CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::~CRenderItemManager ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;
    m_uiDefaultViewportSizeX = fViewportSizeX;
    m_uiDefaultViewportSizeY = fViewportSizeY;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnLostDevice ( void )
{
    for ( std::set < CRenderItem* >::iterator iter = m_CreatedItemList.begin () ; iter != m_CreatedItemList.end () ; iter++ )
        (*iter)->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnResetDevice ( void )
{
    for ( std::set < CRenderItem* >::iterator iter = m_CreatedItemList.begin () ; iter != m_CreatedItemList.end () ; iter++ )
        (*iter)->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateTexture
//
// TODO: Make underlying data for textures shared
//
////////////////////////////////////////////////////////////////
CTextureItem* CRenderItemManager::CreateTexture ( const SString& strFullFilePath )
{
    CFileTextureItem* pTextureItem = new CFileTextureItem ();
    pTextureItem->PostConstruct ( this, strFullFilePath );

    if ( !pTextureItem->IsValid () )
    {
        SAFE_RELEASE ( pTextureItem );
        return NULL;
    }

    return pTextureItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
CRenderTargetItem* CRenderItemManager::CreateRenderTarget ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel )
{
    CRenderTargetItem* pRenderTargetItem = new CRenderTargetItem ();
    pRenderTargetItem->PostConstruct ( this, uiSizeX, uiSizeY, bWithAlphaChannel );

    if ( !pRenderTargetItem->IsValid () )
    {
        SAFE_RELEASE ( pRenderTargetItem );
        return NULL;
    }

    return pRenderTargetItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
CScreenSourceItem* CRenderItemManager::CreateScreenSource ( uint uiSizeX, uint uiSizeY )
{
    CScreenSourceItem* pScreenSourceItem = new CScreenSourceItem ();
    pScreenSourceItem->PostConstruct ( this, uiSizeX, uiSizeY );

    if ( !pScreenSourceItem->IsValid () )
    {
        SAFE_RELEASE ( pScreenSourceItem );
        return NULL;
    }

    return pScreenSourceItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateShader
//
// Create a D3DX effect from .fx file
//
////////////////////////////////////////////////////////////////
CShaderItem* CRenderItemManager::CreateShader ( const SString& strFullFilePath, SString& strOutStatus )
{
    strOutStatus = "";

    CShaderItem* pShaderItem = new CShaderItem ();
    pShaderItem->PostConstruct ( this, strFullFilePath, strOutStatus );

    if ( !pShaderItem->IsValid () )
    {
        SAFE_RELEASE ( pShaderItem );
        return NULL;
    }

    return pShaderItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateDxFont
//
// TODO: Make underlying data for fonts shared
//
////////////////////////////////////////////////////////////////
CDxFontItem* CRenderItemManager::CreateDxFont ( const SString& strFullFilePath, uint uiSize, bool bBold )
{
    CDxFontItem* pDxFontItem = new CDxFontItem ();
    pDxFontItem->PostConstruct ( this, strFullFilePath, uiSize, bBold );

    if ( !pDxFontItem->IsValid () )
    {
        SAFE_RELEASE ( pDxFontItem );
        return NULL;
    }

    return pDxFontItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateGuiFont
//
// TODO: Make underlying data for fonts shared
//
////////////////////////////////////////////////////////////////
CGuiFontItem* CRenderItemManager::CreateGuiFont ( const SString& strFullFilePath, const SString& strFontName, uint uiSize )
{
    CGuiFontItem* pGuiFontItem = new CGuiFontItem ();
    pGuiFontItem->PostConstruct ( this, strFullFilePath, strFontName, uiSize );

    if ( !pGuiFontItem->IsValid () )
    {
        SAFE_RELEASE ( pGuiFontItem );
        return NULL;
    }

    return pGuiFontItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseRenderItem
//
// Decrement reference count on a render item, and delete if necessary
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseRenderItem ( CRenderItem* pItem )
{
    SAFE_RELEASE ( pItem );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyContructRenderItem
//
// Add to managers list
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyContructRenderItem ( CRenderItem* pItem )
{
    assert ( !MapContains ( m_CreatedItemList, pItem ) );
    MapInsert ( m_CreatedItemList, pItem );

    if ( CScreenSourceItem* pScreenSourceItem = DynamicCast < CScreenSourceItem > ( pItem ) )
        m_bBackBufferCopyMaybeNeedsResize = true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::NotifyDestructRenderItem
//
// Remove from managers list
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::NotifyDestructRenderItem ( CRenderItem* pItem )
{
    assert ( MapContains ( m_CreatedItemList, pItem ) );
    MapRemove ( m_CreatedItemList, pItem );

    if ( CScreenSourceItem* pScreenSourceItem = DynamicCast < CScreenSourceItem > ( pItem ) )
        m_bBackBufferCopyMaybeNeedsResize = true;
}



//
//
// Shaders
//
//

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set one texture
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( CShaderItem* pShaderItem, const SString& strName, CTextureItem* pTextureItem )
{
    return pShaderItem->SetValue ( strName, pTextureItem );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set one bool
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( CShaderItem* pShaderItem, const SString& strName, bool bValue )
{
    return pShaderItem->SetValue ( strName, bValue );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set up to 16 floats
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( CShaderItem* pShaderItem, const SString& strName, const float* pfValues, uint uiCount )
{
    return pShaderItem->SetValue ( strName, pfValues, uiCount );
}



//
//
// Render targets and back buffers
//
//

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopy
//
// Save back buffer pixels in our special place
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopy ( void )
{
    // Update our info about what rendertarget is active
    IDirect3DSurface9* pActiveD3DRenderTarget;
    IDirect3DSurface9* pActiveD3DZStencilSurface;
    m_pDevice->GetRenderTarget ( 0, &pActiveD3DRenderTarget );
    m_pDevice->GetDepthStencilSurface ( &pActiveD3DZStencilSurface );

    if ( pActiveD3DRenderTarget != m_pDefaultD3DRenderTarget
        || pActiveD3DZStencilSurface != m_pDefaultD3DZStencilSurface )
    {
        m_pDefaultD3DRenderTarget = pActiveD3DRenderTarget;
        m_pDefaultD3DZStencilSurface = pActiveD3DZStencilSurface;
    }

    // Don't hold any refs because it goes all funny during fullscreen minimize/maximize, even if they are released at onLostDevice
    SAFE_RELEASE ( pActiveD3DRenderTarget )
    SAFE_RELEASE ( pActiveD3DZStencilSurface )

    if ( m_bBackBufferCopyMaybeNeedsResize )
        UpdateBackBufferCopySize ();

    // Don't bother doing this unless at least one screen stream in active
    if ( !m_pBackBufferCopy )
        return;

    // Try to get the back buffer
	IDirect3DSurface9* pD3DBackBufferSurface = NULL;
    m_pDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DBackBufferSurface );
    if ( !pD3DBackBufferSurface )
        return;

    // Copy back buffer into our private render target
    D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
    HRESULT hr = m_pDevice->StretchRect( pD3DBackBufferSurface, NULL, m_pBackBufferCopy->m_pD3DRenderTargetSurface, NULL, FilterType );

    m_uiBackBufferCopyRevision++;

    // Clean up
	SAFE_RELEASE( pD3DBackBufferSurface );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateScreenSource
//
// Copy from back buffer store to screen source
// TODO - Optimize the case where the screen source is the same size as the back buffer copy (i.e. Use back buffer copy resources instead)
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateScreenSource ( CScreenSourceItem* pScreenSourceItem )
{
    // Only do update if back buffer copy has changed
    if ( pScreenSourceItem->m_uiRevision == m_uiBackBufferCopyRevision )
        return;

    pScreenSourceItem->m_uiRevision = m_uiBackBufferCopyRevision;

    if ( m_pBackBufferCopy )
    {
        D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
        m_pDevice->StretchRect( m_pBackBufferCopy->m_pD3DRenderTargetSurface, NULL, pScreenSourceItem->m_pD3DRenderTargetSurface, NULL, FilterType );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopySize
//
// Create/resize/destroy back buffer copy depending on what is required
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopySize ( void )
{
    m_bBackBufferCopyMaybeNeedsResize = false;

    // Set what the max size requirement is for the back buffer copy
    uint uiSizeX = 0;
    uint uiSizeY = 0;
    for ( std::set < CRenderItem* >::iterator iter = m_CreatedItemList.begin () ; iter != m_CreatedItemList.end () ; iter++ )
    {
        if ( CScreenSourceItem* pScreenSourceItem = DynamicCast < CScreenSourceItem > ( *iter ) )
        {
            uiSizeX = Max ( uiSizeX, pScreenSourceItem->m_uiSizeX );
            uiSizeY = Max ( uiSizeY, pScreenSourceItem->m_uiSizeY );
        }
    }

    // Update?
    if ( !m_pBackBufferCopy || m_pBackBufferCopy->m_uiSizeX != uiSizeX || m_pBackBufferCopy->m_uiSizeY != uiSizeY )
    {
        // Delete old one if it exists
        if ( m_pBackBufferCopy )
        {
            ReleaseRenderItem ( m_pBackBufferCopy );
            m_pBackBufferCopy = NULL;
        }

        // Try to create new one if needed
        if ( uiSizeX > 0 )
            m_pBackBufferCopy = CreateRenderTarget ( uiSizeX, uiSizeY, false );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetRenderTarget
//
// Set current render target to a custom one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetRenderTarget ( CRenderTargetItem* pItem, bool bClear )
{
    if ( !CGraphics::GetSingleton().CanSetRenderTarget () )
        return false;

    ChangeRenderTarget ( pItem->m_uiSizeX, pItem->m_uiSizeY, pItem->m_pD3DRenderTargetSurface, pItem->m_pD3DZStencilSurface );

    if ( bClear )
        m_pDevice->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1, 0 );

    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RestoreDefaultRenderTarget
//
// Set render target back to the default one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::RestoreDefaultRenderTarget ( void )
{
    if ( !CGraphics::GetSingleton().CanSetRenderTarget () )
        return false;

    // Only need to change if we have info
    if ( m_pDefaultD3DRenderTarget )
        ChangeRenderTarget ( m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, m_pDefaultD3DRenderTarget, m_pDefaultD3DZStencilSurface );

    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ChangeRenderTarget
//
// Worker function to change the D3D render target
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ChangeRenderTarget ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface )
{
    // Check if we need to change
    IDirect3DSurface9* pCurrentRenderTarget;
    IDirect3DSurface9* pCurrentZStencilSurface;
    m_pDevice->GetRenderTarget ( 0, &pCurrentRenderTarget );
    m_pDevice->GetDepthStencilSurface ( &pCurrentZStencilSurface );

    bool bAlreadySet = ( pD3DRenderTarget == pCurrentRenderTarget && pD3DZStencilSurface == pCurrentZStencilSurface );

    SAFE_RELEASE ( pCurrentRenderTarget );
    SAFE_RELEASE ( pCurrentZStencilSurface );

    // Already set?
    if ( bAlreadySet )
        return;

    // Tell graphics things are about to change
    CGraphics::GetSingleton().OnChangingRenderTarget ( uiSizeX, uiSizeY );

    // Do change
    m_pDevice->SetRenderTarget ( 0, pD3DRenderTarget );
    m_pDevice->SetDepthStencilSurface ( pD3DZStencilSurface );

    D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = uiSizeX;
    viewport.Height = uiSizeY;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    m_pDevice->SetViewport ( &viewport );
}

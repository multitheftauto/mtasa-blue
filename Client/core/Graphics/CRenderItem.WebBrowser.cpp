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
void CWebBrowserItem::PostConstruct(CRenderItemManager* pRenderItemManager, uint uiSizeX, uint uiSizeY)
{
    Super::PostConstruct(pRenderItemManager);
    m_uiSizeX = uiSizeX;
    m_uiSizeY = uiSizeY;
    m_uiSurfaceSizeX = uiSizeX;
    m_uiSurfaceSizeY = uiSizeY;

    // Initial creation of d3d data
    CreateUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CWebBrowserItem::IsValid()
{
    return m_pD3DTexture && m_pD3DRenderTargetSurface;
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::OnLostDevice
//
// Release device stuff - D3DPOOL_DEFAULT textures must be released
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnLostDevice()
{
    ReleaseUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::OnResetDevice
//
// Recreate device stuff - D3DPOOL_DEFAULT textures must be recreated
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnResetDevice()
{
    CreateUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::CreateUnderlyingData()
{
    assert(!m_pD3DRenderTargetSurface);
    assert(!m_pD3DTexture);

    // Use D3DPOOL_DEFAULT with D3DUSAGE_DYNAMIC for better performance:
    // - Dynamic textures are optimized for frequent Lock/Unlock operations
    // - D3DLOCK_DISCARD can be used effectively to avoid stalls
    // - No system memory copy (unlike D3DPOOL_MANAGED), reducing memory usage
    // Note: Must handle device lost/reset as DEFAULT pool textures don't survive resets
    if (FAILED(m_pDevice->CreateTexture(m_uiSizeX, m_uiSizeY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&m_pD3DTexture, nullptr)) ||
        !m_pD3DTexture)
        return;

    // Get the render target surface here for convenience
    if (FAILED(((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel(0, &m_pD3DRenderTargetSurface)) || !m_pD3DRenderTargetSurface)
    {
        SAFE_RELEASE(m_pD3DTexture);
        return;
    }

    // Update surface size - dynamic textures may have different dimensions than requested
    D3DSURFACE_DESC desc;
    m_pD3DRenderTargetSurface->GetDesc(&desc);
    m_uiSurfaceSizeX = desc.Width;
    m_uiSurfaceSizeY = desc.Height;

    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DRenderTargetSurface);
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DRenderTargetSurface)
    SAFE_RELEASE(m_pD3DTexture)
}

void CWebBrowserItem::Resize(const CVector2D& size)
{
    // Update size
    m_uiSizeX = static_cast<uint>(size.fX);
    m_uiSizeY = static_cast<uint>(size.fY);

    // Recreate texture
    ReleaseUnderlyingData();
    CreateUnderlyingData();
}

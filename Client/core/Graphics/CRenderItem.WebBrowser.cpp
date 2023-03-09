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
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnLostDevice()
{
}

////////////////////////////////////////////////////////////////
//
// CWebBrowserItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CWebBrowserItem::OnResetDevice()
{
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

    // Check if texture is actually created. It can be failed in some conditions(e.g. lack of memory).
    if (FAILED(D3DXCreateTexture(m_pDevice, m_uiSizeX, m_uiSizeY, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&m_pD3DTexture)) || !m_pD3DTexture)
        return;

    // Get the render target surface here for convenience
    if (FAILED(((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel(0, &m_pD3DRenderTargetSurface)) || !m_pD3DRenderTargetSurface)
    {
        SAFE_RELEASE(m_pD3DTexture);
        return;
    }

    // D3DXCreateTexture sets width and height to 1 if the argument value was 0
    // See: https://docs.microsoft.com/en-us/windows/desktop/direct3d9/d3dxcreatetexture
    if (m_uiSizeX == 0)
        m_uiSizeX = 1;

    if (m_uiSizeY == 0)
        m_uiSizeY = 1;    

    // Update surface size, although it probably will be unchanged | Todo: Remove this
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

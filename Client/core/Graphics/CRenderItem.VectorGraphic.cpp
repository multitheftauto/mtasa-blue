/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::PostConstruct(CRenderItemManager* pManager, uint width, uint height)
{
    Super::PostConstruct(pManager);

    m_uiSizeX = width;
    m_uiSizeY = height;
    m_uiSurfaceSizeX = width;
    m_uiSurfaceSizeY = height;

    CreateUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CVectorGraphicItem::IsValid()
{
    return m_pD3DTexture != nullptr;
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::TryEnsureValid
//
// Attempt to (re)create device resources if missing
//
////////////////////////////////////////////////////////////////
bool CVectorGraphicItem::TryEnsureValid()
{
    if (IsValid())
        return true;

    if (!m_pManager || m_pManager->GetDeviceCooperativeLevel("VectorGraphicTryEnsureValid", false) != D3D_OK)
        return false;

    const uint kRetryIntervalMinMs = 250;
    const uint kRetryIntervalMaxMs = 2000;
    const uint uiNow = GetTickCount32();
    if (m_uiEnsureDelayMs == 0)
        m_uiEnsureDelayMs = kRetryIntervalMinMs;

    if (uiNow - m_uiLastEnsureAttempt < m_uiEnsureDelayMs)
        return false;

    m_uiLastEnsureAttempt = uiNow;

    if (m_pD3DRenderTargetSurface || m_pD3DTexture)
        ReleaseUnderlyingData();

    CreateUnderlyingData();
    if (IsValid())
    {
        m_uiEnsureDelayMs = kRetryIntervalMinMs;
        return true;
    }

    m_uiEnsureDelayMs = std::min(m_uiEnsureDelayMs * 2, kRetryIntervalMaxMs);
    return false;
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::OnLostDevice()
{
    // Nothing required for CVectorGraphicItem
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::OnResetDevice()
{
    // Nothing required for CVectorGraphicItem
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::CreateUnderlyingData
//
// From file
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::CreateUnderlyingData()
{
    assert(!m_pD3DRenderTargetSurface);
    assert(!m_pD3DTexture);

    D3DXCreateTexture(m_pDevice, m_uiSizeX, m_uiSizeY, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&m_pD3DTexture);

    // Check texture created
    if (!m_pD3DTexture)
        return;

    // D3DXCreateTexture sets width and height to 1 if the argument value was 0
    // See: https://docs.microsoft.com/en-us/windows/desktop/direct3d9/d3dxcreatetexture
    if (m_uiSizeX == 0)
        m_uiSizeX = 1;

    if (m_uiSizeY == 0)
        m_uiSizeY = 1;

    // Get the render target surface here for convenience
    ((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel(0, &m_pD3DRenderTargetSurface);

    // Update surface size, although it probably will be unchanged | Todo: Remove this
    D3DSURFACE_DESC desc;
    m_pD3DRenderTargetSurface->GetDesc(&desc);
    m_uiSurfaceSizeX = desc.Width;
    m_uiSurfaceSizeY = desc.Height;

    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DRenderTargetSurface);
}

////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CVectorGraphicItem::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DRenderTargetSurface);
    SAFE_RELEASE(m_pD3DTexture);
}

void CVectorGraphicItem::Resize(const CVector2D& size)
{
    // Update size
    m_uiSizeX = static_cast<uint>(size.fX);
    m_uiSizeY = static_cast<uint>(size.fY);

    // Recreate texture
    ReleaseUnderlyingData();
    CreateUnderlyingData();
}

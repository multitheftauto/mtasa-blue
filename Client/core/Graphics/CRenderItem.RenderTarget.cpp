/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.RenderTarget.cpp
 *  PURPOSE:
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
void CRenderTargetItem::PostConstruct(CRenderItemManager* pManager, uint uiSizeX, uint uiSizeY, bool bHasSurfaceFormat, bool bWithAlphaChannel,
                                      int surfaceFormat, bool bIncludeInMemoryStats)
{
    Super::PostConstruct(pManager, bIncludeInMemoryStats);
    m_uiSizeX = uiSizeX;
    m_uiSizeY = uiSizeY;
    m_uiSurfaceSizeX = uiSizeX;
    m_uiSurfaceSizeY = uiSizeY;
    m_bHasSurfaceFormat = bHasSurfaceFormat;
    m_bWithAlphaChannel = bWithAlphaChannel;
    m_eSurfaceFormat = surfaceFormat;

    // Initial creation of d3d data
    CreateUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CRenderTargetItem::IsValid()
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
void CRenderTargetItem::OnLostDevice()
{
    ReleaseUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::OnResetDevice()
{
    CreateUnderlyingData();
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::CreateUnderlyingData()
{
    assert(!m_pD3DRenderTargetSurface);
    assert(!m_pD3DTexture);
    assert(!m_pD3DZStencilSurface);

    for (uint i = 0; i < 4; i++)
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
        D3DFORMAT Format;
        if (m_bHasSurfaceFormat)
            Format = (D3DFORMAT)m_eSurfaceFormat;
        else
        {
            Format = i & 1 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
            if (m_bWithAlphaChannel)
                Format = D3DFMT_A8R8G8B8;
        }
        if (SUCCEEDED(
                m_pDevice->CreateTexture(m_uiSizeX, m_uiSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&m_pD3DTexture, NULL)))
            break;

        // c'mon
        if (i == 1)
            m_pDevice->EvictManagedResources();
    }

    // Check texture created
    if (!m_pD3DTexture)
        return;

    for (uint i = 0; i < 4; i++)
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
        D3DFORMAT Format = i & 1 ? D3DFMT_D16 : D3DFMT_D24X8;
        if (SUCCEEDED(m_pDevice->CreateDepthStencilSurface(m_uiSizeX, m_uiSizeY, Format, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pD3DZStencilSurface, NULL)))
            break;

        // c'mon c'mon
        if (i == 1)
            m_pDevice->EvictManagedResources();
    }

    // Check depth buffer created
    if (!m_pD3DZStencilSurface)
    {
        SAFE_RELEASE(m_pD3DTexture);
        return;
    }

    // Get the render target surface here for convenience
    ((IDirect3DTexture9*)m_pD3DTexture)->GetSurfaceLevel(0, &m_pD3DRenderTargetSurface);

    // Update surface size, although it probably will be unchanged
    D3DSURFACE_DESC desc;
    m_pD3DRenderTargetSurface->GetDesc(&desc);
    m_uiSurfaceSizeX = desc.Width;
    m_uiSurfaceSizeY = desc.Height;

    // Clear incase it gets used before first copy
    m_pDevice->ColorFill(m_pD3DRenderTargetSurface, NULL, 0x00000000);

    // Update memory used
    m_iMemoryKBUsed =
        CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DRenderTargetSurface) + CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DZStencilSurface);

    // Update revision counter
    m_iRevision++;
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CRenderTargetItem::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DRenderTargetSurface)
    SAFE_RELEASE(m_pD3DTexture)
    SAFE_RELEASE(m_pD3DZStencilSurface)
    SAFE_RELEASE(m_pD3DReadSurface)
}

////////////////////////////////////////////////////////////////
//
// CRenderTargetItem::ReadPixels
//
// Read render target pixels into the buffer.
// Not very quick thing to do.
//
////////////////////////////////////////////////////////////////
bool CRenderTargetItem::ReadPixels(CBuffer& outBuffer, SString& strOutError)
{
    HRESULT hr;

    D3DSURFACE_DESC Desc;
    m_pD3DRenderTargetSurface->GetDesc(&Desc);

    int iBitsPerPixel = CRenderItemManager::GetBitsPerPixel(Desc.Format);
    if (iBitsPerPixel != 32 && iBitsPerPixel != 16)
    {
        strOutError = "BitsPerPixel error";
        return false;
    }

    // Make sure we have an offscreen surface which is exactly the same as the render target
    if (m_pD3DReadSurface)
    {
        D3DSURFACE_DESC ReadDesc;
        m_pD3DReadSurface->GetDesc(&ReadDesc);
        if (ReadDesc.Width != Desc.Width || ReadDesc.Height != Desc.Height || ReadDesc.Format != Desc.Format)
        {
            SAFE_RELEASE(m_pD3DReadSurface)
        }
    }

    if (!m_pD3DReadSurface)
    {
        hr = m_pDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, D3DPOOL_SYSTEMMEM, &m_pD3DReadSurface, NULL);
        if (FAILED(hr))
        {
            strOutError = SString("CreateOffscreenPlainSurface failed (0x%08x)", hr);
            return false;
        }
    }

    // Copy render target to m_pD3DReadSurface
    hr = m_pDevice->GetRenderTargetData(m_pD3DRenderTargetSurface, m_pD3DReadSurface);
    if (FAILED(hr))
    {
        strOutError = SString("GetRenderTargetData failed (0x%08x)", hr);
        return false;
    }

    // Get pixels from m_pD3DReadSurface
    D3DLOCKED_RECT LockedRect;
    hr = m_pD3DReadSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
    {
        strOutError = SString("LockRect failed (0x%08x)", hr);
        return false;
    }

    // Allocate 32 bit buffer
    uint ulScreenWidth = Desc.Width;
    uint ulScreenHeight = Desc.Height;
    uint ulLineBytes = ulScreenWidth * 4;
    outBuffer.SetSize(ulLineBytes * ulScreenHeight);
    char* pDest = outBuffer.GetData();

    // Copy lines into a buffer
    void* ms_pBits = LockedRect.pBits;
    uint  ms_ulPitch = LockedRect.Pitch;
    if (iBitsPerPixel == 32)
    {
        // 32 bit source
        for (unsigned int i = 0; i < ulScreenHeight; i++)
            memcpy(pDest + ulLineBytes * i, (BYTE*)ms_pBits + i * ms_ulPitch, ulLineBytes);
    }
    else
    {
        // 16 bit source
        for (unsigned int i = 0; i < ulScreenHeight; i++)
        {
            const WORD* pSrc16 = (WORD*)((BYTE*)ms_pBits + i * ms_ulPitch);
            DWORD*      pDest32 = (DWORD*)(pDest + ulLineBytes * i);
            for (unsigned int x = 0; x < ulScreenWidth; x++)
            {
                WORD r5g6b5 = pSrc16[x];
                BYTE r = (r5g6b5 & 0xF800) >> 11 << 3;
                BYTE g = (r5g6b5 & 0x7E0) >> 5 << 2;
                BYTE b = (r5g6b5 & 0x1F) << 3;

                DWORD x8r8g8b8 = (r << 16) | (g << 8) | b;
                x8r8g8b8 |= 0xFF070307;
                pDest32[x] = x8r8g8b8;
            }
        }
    }

    m_pD3DReadSurface->UnlockRect();
    return true;
}

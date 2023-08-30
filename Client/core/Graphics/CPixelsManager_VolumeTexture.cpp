/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFileFormat.h"
#include "CPixelsManager.h"

////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetVolumeTexturePixels
//
// Copy pixels from texture
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetVolumeTexturePixels(IDirect3DVolumeTexture9* pD3DVolumeTexture, CPixels& outPixels, const RECT* pRect, uint uiSlice)
{
    if (!pD3DVolumeTexture)
        return false;

    IDirect3DVolume9*                pVolume = NULL;
    CAutoReleaseMe<IDirect3DVolume9> Thanks(pVolume);

    UINT Level = 0;
    pD3DVolumeTexture->GetVolumeLevel(Level, &pVolume);
    if (!pVolume)
        return false;

    bool           bResult = false;
    D3DVOLUME_DESC Desc;
    pVolume->GetDesc(&Desc);

    if (Desc.Usage == 0)
    {
        if (Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8 || Desc.Format == D3DFMT_R5G6B5)
        {
            // Direct reading will work here
            bResult = GetVolumePixels(pVolume, outPixels, pRect, uiSlice);
        }
        else
        {
            return false;
        }
    }

    return bResult;
}

////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetVolumeTexturePixels
//
// Copy pixels into texture
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetVolumeTexturePixels(IDirect3DVolumeTexture9* pD3DVolumeTexture, const CPixels& pixels, const RECT* pRect, uint uiSlice)
{
    if (!pD3DVolumeTexture)
        return false;

    IDirect3DVolume9*                pVolume = NULL;
    CAutoReleaseMe<IDirect3DVolume9> Thanks(pVolume);

    UINT Level = 0;
    pD3DVolumeTexture->GetVolumeLevel(Level, &pVolume);
    if (!pVolume)
        return false;

    bool           bResult = false;
    D3DVOLUME_DESC Desc;
    pVolume->GetDesc(&Desc);

    if (Desc.Usage == 0)
    {
        if (Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8 || Desc.Format == D3DFMT_R5G6B5)
        {
            // Direct reading will work here
            bResult = SetVolumePixels(pVolume, pixels, pRect, uiSlice);
        }
        else
        {
            return false;
        }
    }

    return bResult;
}

////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetVolumePixels
//
// Here we have a surface which:
//          1 - is D3DFMT_R5G6B5 or D3DFMT_X8R8G8B8/D3DFMT_A8R8G8B8
//          2 - can be locked
//
// Returns PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetVolumePixels(IDirect3DVolume9* pD3DSurface, CPixels& outPixels, const RECT* pRect, uint uiSlice)
{
    if (!pD3DSurface)
        return false;

    // Calc surface rect
    D3DVOLUME_DESC SurfDesc;
    pD3DSurface->GetDesc(&SurfDesc);

    POINT SurfSize = {static_cast<int>(SurfDesc.Width), static_cast<int>(SurfDesc.Height)};
    RECT  SurfRect = {0, 0, static_cast<int>(SurfDesc.Width), static_cast<int>(SurfDesc.Height)};
    if (pRect)
        SurfRect = *pRect;

    // Calc pixels rect
    uint uiPixelsWidth = GetRectWidth(SurfRect);
    uint uiPixelsHeight = GetRectHeight(SurfRect);

    POINT PixelsSize = {static_cast<int>(uiPixelsWidth), static_cast<int>(uiPixelsHeight)};
    RECT  PixelsRect = {0, 0, static_cast<int>(uiPixelsWidth), static_cast<int>(uiPixelsHeight)};

    // Validate rects
    if (!ClipRects(PixelsSize, PixelsRect, SurfSize, SurfRect))
        return false;

    // Check if pRect can be NULL
    pRect = &SurfRect;
    if (SurfRect.left == 0 && SurfRect.top == 0 && SurfRect.right == SurfDesc.Width && SurfRect.bottom == SurfDesc.Height)
        pRect = NULL;

    // Get pixels from pD3DSurface
    D3DLOCKED_RECT LockedRect;
    if (FAILED(LockVolumeRect(pD3DSurface, &LockedRect, pRect, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK, uiSlice)))
        return false;

    // Get sizes
    uint SurfRectWidth = GetRectWidth(SurfRect);
    uint SurfRectHeight = GetRectHeight(SurfRect);

    uint PixelsRectWidth = GetRectWidth(PixelsRect);
    uint PixelsRectHeight = GetRectHeight(PixelsRect);

    uint CopyWidth = std::min(SurfRectWidth, PixelsRectWidth);
    uint CopyHeight = std::min(SurfRectHeight, PixelsRectHeight);

    // Prepare pixels
    uint ulPixelsPitch = uiPixelsWidth * XRGB_BYTES_PER_PIXEL;

    try
    {
        outPixels.SetSize(ulPixelsPitch * uiPixelsHeight + SIZEOF_PLAIN_TAIL);
    }
    catch (const std::bad_alloc&)
    {
        pD3DSurface->UnlockBox();
        return false;
    }

    memset(outPixels.GetData(), 0x81, outPixels.GetSize());
    char* pPixelsData = outPixels.GetData();
    pPixelsData += PixelsRect.left * XRGB_BYTES_PER_PIXEL;
    pPixelsData += PixelsRect.top * ulPixelsPitch;

    uint  ulSurfPitch = LockedRect.Pitch;
    BYTE* pSurfBits = (BYTE*)LockedRect.pBits;

    if (CRenderItemManager::GetBitsPerPixel(SurfDesc.Format) == 32)
    {
        if (SurfDesc.Format == D3DFMT_A8R8G8B8)
        {
            uint uiCopyWidthBytes = CopyWidth * XRGB_BYTES_PER_PIXEL;
            // Copy lines into buffer
            for (uint i = 0; i < CopyHeight; i++)
            {
                memcpy(pPixelsData + ulPixelsPitch * i, pSurfBits + i * ulSurfPitch, uiCopyWidthBytes);
            }
        }
        else
        {
            // Copy lines into buffer
            for (uint i = 0; i < CopyHeight; i++)
            {
                DWORD*       pLinePixelsStart = (DWORD*)(pPixelsData + ulPixelsPitch * i);
                const DWORD* pLineSurfStart = (DWORD*)(pSurfBits + ulSurfPitch * i);
                for (uint x = 0; x < CopyWidth; x++)
                {
                    DWORD x8r8g8b8 = pLineSurfStart[x];
                    x8r8g8b8 |= 0xff000000;
                    pLinePixelsStart[x] = x8r8g8b8;
                }
            }
        }
    }
    else if (CRenderItemManager::GetBitsPerPixel(SurfDesc.Format) == 16)
    {
        // Copy lines into buffer
        for (uint i = 0; i < CopyHeight; i++)
        {
            DWORD*      pLinePixelsStart = (DWORD*)(pPixelsData + ulPixelsPitch * i);
            const WORD* pLineSurfStart = (WORD*)(pSurfBits + ulSurfPitch * i);
            for (uint x = 0; x < CopyWidth; x++)
            {
                WORD r5g6b5 = pLineSurfStart[x];
                BYTE r = (r5g6b5 & 0xF800) >> 11 << 3;
                BYTE g = (r5g6b5 & 0x7E0) >> 5 << 2;
                BYTE b = (r5g6b5 & 0x1F) << 3;

                DWORD x8r8g8b8 = (r << 16) | (g << 8) | b;
                x8r8g8b8 |= 0xFF070307;
                pLinePixelsStart[x] = x8r8g8b8;
            }
        }
    }

    pD3DSurface->UnlockBox();

    // Fixup plain format attributes
    return SetPlainDimensions(outPixels, uiPixelsWidth, uiPixelsHeight);
}

////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetVolumePixels
//
// Here we have a surface which:
//          1 - is D3DFMT_R5G6B5 or D3DFMT_X8R8G8B8/D3DFMT_A8R8G8B8
//          2 - can be locked
//
// Requires PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetVolumePixels(IDirect3DVolume9* pD3DSurface, const CPixels& pixels, const RECT* pRect, uint uiSlice)
{
    if (!pD3DSurface)
        return false;

    // Calc surface rect
    D3DVOLUME_DESC SurfDesc;
    pD3DSurface->GetDesc(&SurfDesc);

    POINT SurfSize = {static_cast<int>(SurfDesc.Width), static_cast<int>(SurfDesc.Height)};
    RECT  SurfRect = {0, 0, static_cast<int>(SurfDesc.Width), static_cast<int>(SurfDesc.Height)};
    if (pRect)
        SurfRect = *pRect;

    // Calc pixels rect
    uint uiPixelsWidth, uiPixelsHeight;
    if (!GetPlainDimensions(pixels, uiPixelsWidth, uiPixelsHeight))
        return false;

    POINT PixelsSize = {static_cast<int>(uiPixelsWidth), static_cast<int>(uiPixelsHeight)};
    RECT  PixelsRect = {0, 0, static_cast<int>(uiPixelsWidth), static_cast<int>(uiPixelsHeight)};

    // Validate rects
    if (!ClipRects(PixelsSize, PixelsRect, SurfSize, SurfRect))
        return false;

    // Check if pRect can be NULL
    pRect = &SurfRect;
    if (SurfRect.left == 0 && SurfRect.top == 0 && SurfRect.right == SurfDesc.Width && SurfRect.bottom == SurfDesc.Height)
        pRect = NULL;

    // Get pixels from pD3DSurface
    D3DLOCKED_RECT LockedRect;
    if (FAILED(LockVolumeRect(pD3DSurface, &LockedRect, pRect, D3DLOCK_NOSYSLOCK, uiSlice)))
        return false;

    // Get sizes
    uint SurfRectWidth = GetRectWidth(SurfRect);
    uint SurfRectHeight = GetRectHeight(SurfRect);

    uint PixelsRectWidth = GetRectWidth(PixelsRect);
    uint PixelsRectHeight = GetRectHeight(PixelsRect);

    uint CopyWidth = std::min(SurfRectWidth, PixelsRectWidth);
    uint CopyHeight = std::min(SurfRectHeight, PixelsRectHeight);

    // Prepare pixels
    uint        ulPixelsPitch = uiPixelsWidth * XRGB_BYTES_PER_PIXEL;
    const char* pPixelsData = pixels.GetData();
    pPixelsData += PixelsRect.left * XRGB_BYTES_PER_PIXEL;
    pPixelsData += PixelsRect.top * ulPixelsPitch;

    uint  ulSurfPitch = LockedRect.Pitch;
    BYTE* pSurfBits = (BYTE*)LockedRect.pBits;

    if (CRenderItemManager::GetBitsPerPixel(SurfDesc.Format) == 32)
    {
        if (SurfDesc.Format == D3DFMT_A8R8G8B8)
        {
            uint uiCopyWidthBytes = CopyWidth * XRGB_BYTES_PER_PIXEL;
            // Copy lines into surface
            for (uint i = 0; i < CopyHeight; i++)
            {
                memcpy(pSurfBits + ulSurfPitch * i, pPixelsData + ulPixelsPitch * i, uiCopyWidthBytes);
            }
        }
        else
        {
            // Copy lines into surface
            for (uint i = 0; i < CopyHeight; i++)
            {
                const DWORD* pLinePixelsStart = (DWORD*)(pPixelsData + ulPixelsPitch * i);
                DWORD*       pLineSurfStart = (DWORD*)(pSurfBits + ulSurfPitch * i);
                for (uint x = 0; x < CopyWidth; x++)
                {
                    DWORD x8r8g8b8 = pLinePixelsStart[x];
                    x8r8g8b8 |= 0xff000000;
                    pLineSurfStart[x] = x8r8g8b8;
                }
            }
        }
    }
    else if (CRenderItemManager::GetBitsPerPixel(SurfDesc.Format) == 16)
    {
        // Copy lines into surface
        for (uint i = 0; i < CopyHeight; i++)
        {
            const DWORD* pLinePixelsStart = (DWORD*)(pPixelsData + ulPixelsPitch * i);
            WORD*        pLineSurfStart = (WORD*)(pSurfBits + ulSurfPitch * i);
            for (uint x = 0; x < CopyWidth; x++)
            {
                DWORD x8r8g8b8 = pLinePixelsStart[x];
                WORD  r = (x8r8g8b8 & 0xF80000) >> 8;
                WORD  g = (x8r8g8b8 & 0xFC00) >> 5;
                WORD  b = (x8r8g8b8 & 0xF8) >> 3;
                WORD  r5g6b5 = r | g | b;
                pLineSurfStart[x] = r5g6b5;
            }
        }
    }

    pD3DSurface->UnlockBox();
    return true;
}

////////////////////////////////////////////////////////////////
//
// CPixelsManager::D3DXGetVolumePixels
//
// Returns D3DXIMAGE_FILEFORMAT pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::D3DXGetVolumePixels(IDirect3DVolumeTexture9* pD3DVolumeTexture, CPixels& outPixels, EPixelsFormatType pixelsFormat,
                                         ERenderFormat renderFormat, bool bMipMaps, const RECT* pRect, uint uiSlice)
{
    if (!pD3DVolumeTexture)
        return false;

    IDirect3DVolume9*                pVolume = NULL;
    CAutoReleaseMe<IDirect3DVolume9> Thanks1(pVolume);

    pD3DVolumeTexture->GetVolumeLevel(0, &pVolume);
    if (!pVolume)
        return false;

    D3DVOLUME_DESC Desc;
    pVolume->GetDesc(&Desc);

    D3DBOX box;
    if (pRect)
    {
        assert(pRect->left >= 0 && pRect->top >= 0 && pRect->right <= (int)Desc.Width && pRect->bottom <= (int)Desc.Height);
        box.Left = pRect->left;
        box.Top = pRect->top;
        box.Right = pRect->right;
        box.Bottom = pRect->bottom;
        Desc.Width = pRect->right - pRect->left;
        Desc.Height = pRect->bottom - pRect->top;
    }
    else
    {
        box.Left = 0;
        box.Top = 0;
        box.Right = Desc.Width;
        box.Bottom = Desc.Height;
    }
    box.Front = uiSlice;
    box.Back = uiSlice + 1;

    D3DXIMAGE_FILEFORMAT dxFileFormat = D3DXIFF_DDS;
    switch (pixelsFormat)
    {
        case EPixelsFormat::PNG:
            dxFileFormat = D3DXIFF_PNG;
            break;
        case EPixelsFormat::JPEG:
            dxFileFormat = D3DXIFF_JPG;
            break;
    }

    D3DFORMAT dxFormat = (D3DFORMAT)renderFormat;
    if (dxFormat == D3DFMT_UNKNOWN)
        dxFormat = Desc.Format;

    bool bNeedToConvert = dxFileFormat == D3DXIFF_DDS;
    if (bNeedToConvert && Desc.Format == dxFormat && !bMipMaps)
        bNeedToConvert = false; // No need to convert DDS if compression is the same and no mipmaps required

    ID3DXBuffer*                dxBuffer;
    CAutoReleaseMe<ID3DXBuffer> Thanks2(dxBuffer);
    if (!FAILED(D3DXSaveVolumeToFileInMemory(&dxBuffer, bNeedToConvert ? D3DXIFF_DDS : dxFileFormat, pVolume, NULL, &box)))
    {
        if (bNeedToConvert)
        {
            // Convert volume slice to DDS texture of requested format
            IDirect3DTexture9*                pD3DTempTexture = NULL;
            IDirect3DSurface9*                pD3DTempSurface = NULL;
            CAutoReleaseMe<IDirect3DTexture9> Thanks3(pD3DTempTexture);
            CAutoReleaseMe<IDirect3DSurface9> Thanks4(pD3DTempSurface);

            if (FAILED(m_pDevice->CreateTexture(Desc.Width, Desc.Height, !bMipMaps, NULL, dxFormat, D3DPOOL_SYSTEMMEM, &pD3DTempTexture, NULL)))
                return false;

            if (FAILED(pD3DTempTexture->GetSurfaceLevel(0, &pD3DTempSurface)))
                return false;

            if (FAILED(D3DXLoadSurfaceFromFileInMemory(pD3DTempSurface, NULL, NULL, dxBuffer->GetBufferPointer(), dxBuffer->GetBufferSize(), NULL,
                                                       D3DX_FILTER_NONE, 0, NULL)))
                return false;

            // Extract pixels from converted texture
            if (!FAILED(D3DXSaveTextureToFileInMemory(&dxBuffer, dxFileFormat, pD3DTempTexture, NULL)))
            {
                outPixels.SetSize(dxBuffer->GetBufferSize());
                char* pPixelsData = outPixels.GetData();
                memcpy(pPixelsData, dxBuffer->GetBufferPointer(), outPixels.GetSize());
                return true;
            }
        }
        else
        {
            // Use source pixels buffer
            outPixels.SetSize(dxBuffer->GetBufferSize());
            char* pPixelsData = outPixels.GetData();
            memcpy(pPixelsData, dxBuffer->GetBufferPointer(), outPixels.GetSize());
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////
//
// CPixelsManager::LockVolumeRect
//
// Static utility functions
// Surface lock with bounds checking
//
////////////////////////////////////////////////////////////////
HRESULT CPixelsManager::LockVolumeRect(IDirect3DVolume9* pD3DVolume, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, uint uiSlice)
{
    D3DBOX         box;
    D3DVOLUME_DESC Desc;
    pD3DVolume->GetDesc(&Desc);

    if (pRect)
    {
        assert(pRect->left >= 0 && pRect->top >= 0 && pRect->right <= (int)Desc.Width && pRect->bottom <= (int)Desc.Height);

        box.Left = pRect->left;
        box.Top = pRect->top;
        box.Right = pRect->right;
        box.Bottom = pRect->bottom;
    }
    else
    {
        box.Left = 0;
        box.Top = 0;
        box.Right = Desc.Width;
        box.Bottom = Desc.Height;
    }

    box.Front = uiSlice;
    box.Back = uiSlice + 1;

    D3DLOCKED_BOX lockedBox;
    HRESULT       hr = pD3DVolume->LockBox(&lockedBox, &box, Flags);

    if (SUCCEEDED(hr))
    {
        pLockedRect->Pitch = lockedBox.RowPitch;
        pLockedRect->pBits = lockedBox.pBits;
    }
    return hr;
}

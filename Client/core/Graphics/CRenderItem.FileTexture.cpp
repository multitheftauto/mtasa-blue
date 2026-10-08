/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.FileTexture.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::PostConstruct(CRenderItemManager* pManager, const SString& strFilename, const CPixels* pPixels, bool bMipMaps, uint uiSizeX,
                                     uint uiSizeY, ERenderFormat format, ETextureAddress textureAddress, ETextureType textureType, uint uiVolumeDepth)
{
    Super::PostConstruct(pManager);

    m_uiVolumeDepth = uiVolumeDepth;
    m_TextureType = textureType;
    m_TextureAddress = textureAddress;

    // Initial creation of d3d data
    if (pPixels)
        CreateUnderlyingData(pPixels, bMipMaps, format);
    else if (!strFilename.empty())
        CreateUnderlyingData(strFilename, bMipMaps, uiSizeX, uiSizeY, format);
    else
        CreateUnderlyingData(bMipMaps, uiSizeX, uiSizeY, format, textureType, uiVolumeDepth);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CFileTextureItem::IsValid()
{
    return m_pD3DTexture != NULL;
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::OnLostDevice()
{
    // Nothing required for CFileTextureItem
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::OnResetDevice()
{
    // Nothing required for CFileTextureItem
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::CreateUnderlyingData
//
// From file
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::CreateUnderlyingData(const SString& strFilename, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format)
{
    assert(!m_pD3DTexture);

    D3DXIMAGE_INFO imageInfo;
    if (FAILED(D3DXGetImageInfoFromFile(strFilename, &imageInfo)))
        return;

    D3DFORMAT D3DFormat = (D3DFORMAT)format;
    int       iMipMaps = bMipMaps ? D3DX_DEFAULT : 1;
    if (uiSizeX != D3DX_DEFAULT)
        imageInfo.Width = uiSizeX;
    if (uiSizeY != D3DX_DEFAULT)
        imageInfo.Height = uiSizeY;

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;
    m_uiSurfaceSizeX = imageInfo.Width;
    m_uiSurfaceSizeY = imageInfo.Height;

    if (imageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE)
    {
        // It's a volume texture!
        if (FAILED(D3DXCreateVolumeTextureFromFileEx(m_pDevice, strFilename, uiSizeX, uiSizeY, D3DX_DEFAULT, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED,
                                                     D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9**)&m_pD3DTexture)))
            return;
    }
    else if (imageInfo.ResourceType == D3DRTYPE_CUBETEXTURE)
    {
        // It's a cubemap texture!
        if (FAILED(D3DXCreateCubeTextureFromFileEx(m_pDevice, strFilename, uiSizeX, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                   NULL, NULL, (IDirect3DCubeTexture9**)&m_pD3DTexture)))
            return;
    }
    else
    {
        // It's none of the above!

        // If size not specified, try to use exact image size to prevent blurring
        if (uiSizeX == D3DX_DEFAULT)
            uiSizeX = D3DX_DEFAULT_NONPOW2;
        if (uiSizeY == D3DX_DEFAULT)
            uiSizeY = D3DX_DEFAULT_NONPOW2;

        if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice, strFilename, uiSizeX, uiSizeY, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                               NULL, NULL, (IDirect3DTexture9**)&m_pD3DTexture)))
            return;

        // Update surface size if it's a normal texture
        D3DSURFACE_DESC desc;
        ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc(0, &desc);
        m_uiSurfaceSizeX = desc.Width;
        m_uiSurfaceSizeY = desc.Height;
    }

    // Calc memory usage
    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::CreateUnderlyingData
//
// From pixels
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::CreateUnderlyingData(const CPixels* pInPixels, bool bMipMaps, ERenderFormat format)
{
    assert(!m_pD3DTexture);

    CPixelsManagerInterface* pPixelsManager = CCore::GetSingleton().GetGraphics()->GetPixelsManager();

    // Copy from plain
    const CPixels* pPixels = pInPixels;
    CPixels        pixelsTemp;
    if (pPixelsManager->GetPixelsFormat(*pPixels) == EPixelsFormat::PLAIN)
    {
        pPixelsManager->ChangePixelsFormat(*pPixels, pixelsTemp, EPixelsFormat::PNG);
        pPixels = &pixelsTemp;
    }

    D3DXIMAGE_INFO imageInfo;
    D3DFORMAT      D3DFormat = (D3DFORMAT)format;
    int            iMipMaps = bMipMaps ? D3DX_DEFAULT : 1;

    if (FAILED(D3DXCreateTextureFromFileInMemoryEx(m_pDevice,           //__in     LPDIRECT3DDEVICE9 pDevice,
                                                   pPixels->GetData(),  //__in     LPCVOID pSrcData,
                                                   pPixels->GetSize(),  //__in     UINT SrcDataSize,
                                                   D3DX_DEFAULT,        //__in     UINT Width,
                                                   D3DX_DEFAULT,        //__in     UINT Height,
                                                   iMipMaps,            //__in     UINT MipLevels,
                                                   0,                   //__in     DWORD Usage,
                                                   D3DFormat,           //__in     D3DFORMAT Format,
                                                   D3DPOOL_MANAGED,     //__in     D3DPOOL Pool,
                                                   D3DX_DEFAULT,        //__in     DWORD Filter,
                                                   D3DX_DEFAULT,        //__in     DWORD MipFilter,
                                                   0,                   //__in     D3DCOLOR ColorKey,
                                                   &imageInfo,          //__inout  D3DXIMAGE_INFO *pSrcInfo,
                                                   NULL,                //__out    PALETTEENTRY *pPalette,
                                                   (IDirect3DTexture9**)&m_pD3DTexture)))
        return;

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;
    m_uiSurfaceSizeX = imageInfo.Width;
    m_uiSurfaceSizeY = imageInfo.Height;

    // Update surface size if it's a normal texture
    D3DSURFACE_DESC desc;
    ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc(0, &desc);
    m_uiSurfaceSizeX = desc.Width;
    m_uiSurfaceSizeY = desc.Height;

    // Calc memory usage
    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::CreateUnderlyingData
//
// Blank sized
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::CreateUnderlyingData(bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format, ETextureType textureType, uint uiVolumeDepth)
{
    assert(!m_pD3DTexture);

    D3DFORMAT D3DFormat = (D3DFORMAT)format;
    int       iMipMaps = bMipMaps ? D3DX_DEFAULT : 1;

    m_uiSizeX = uiSizeX;
    m_uiSizeY = uiSizeY;
    m_uiSurfaceSizeX = uiSizeX;
    m_uiSurfaceSizeY = uiSizeY;

    if (textureType == D3DRTYPE_VOLUMETEXTURE)
    {
        if (FAILED(D3DXCreateVolumeTexture(m_pDevice,        //__in   LPDIRECT3DDEVICE9 pDevice,
                                           uiSizeX,          //__in   UINT Width,
                                           uiSizeY,          //__in   UINT Height,
                                           uiVolumeDepth,    //__in   UINT Depth,
                                           iMipMaps,         //__in   UINT MipLevels,
                                           0,                //__in   DWORD Usage,
                                           D3DFormat,        //__in   D3DFORMAT Format,
                                           D3DPOOL_MANAGED,  //__in   D3DPOOL Pool,
                                           (IDirect3DVolumeTexture9**)&m_pD3DTexture)))
            return;
    }
    else if (textureType == D3DRTYPE_CUBETEXTURE)
    {
        if (FAILED(D3DXCreateCubeTexture(m_pDevice,        //__in   LPDIRECT3DDEVICE9 pDevice,
                                         uiSizeX,          //__in   UINT Width,
                                         iMipMaps,         //__in   UINT MipLevels,
                                         0,                //__in   DWORD Usage,
                                         D3DFormat,        //__in   D3DFORMAT Format,
                                         D3DPOOL_MANAGED,  //__in   D3DPOOL Pool,
                                         (IDirect3DCubeTexture9**)&m_pD3DTexture)))
            return;
    }
    else
    {
        if (FAILED(D3DXCreateTexture(m_pDevice,        //__in   LPDIRECT3DDEVICE9 pDevice,
                                     uiSizeX,          //__in   UINT Width,
                                     uiSizeY,          //__in   UINT Height,
                                     iMipMaps,         //__in   UINT MipLevels,
                                     0,                //__in   DWORD Usage,
                                     D3DFormat,        //__in   D3DFORMAT Format,
                                     D3DPOOL_MANAGED,  //__in   D3DPOOL Pool,
                                     (IDirect3DTexture9**)&m_pD3DTexture)))
            return;

        // Update surface size if it's a normal texture
        D3DSURFACE_DESC desc;
        ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc(0, &desc);
        m_uiSurfaceSizeX = desc.Width;
        m_uiSurfaceSizeY = desc.Height;
    }

    // Calc memory usage
    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::CreateUnderlyingDataFromFileMemory
//
// Same as CreateUnderlyingData(filename), but from an already-read file buffer.
// D3D9 device calls stay on the calling thread (must be the main thread).
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::CreateUnderlyingDataFromFileMemory(const void* pData, uint uiSize, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format)
{
    assert(!m_pD3DTexture);

    if (!pData || uiSize == 0)
        return;

    D3DXIMAGE_INFO imageInfo;
    if (FAILED(D3DXGetImageInfoFromFileInMemory(pData, uiSize, &imageInfo)))
        return;

    D3DFORMAT D3DFormat = (D3DFORMAT)format;
    int       iMipMaps = bMipMaps ? D3DX_DEFAULT : 1;
    if (uiSizeX != D3DX_DEFAULT)
        imageInfo.Width = uiSizeX;
    if (uiSizeY != D3DX_DEFAULT)
        imageInfo.Height = uiSizeY;

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;
    m_uiSurfaceSizeX = imageInfo.Width;
    m_uiSurfaceSizeY = imageInfo.Height;

    if (imageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE)
    {
        if (FAILED(D3DXCreateVolumeTextureFromFileInMemoryEx(m_pDevice, pData, uiSize, uiSizeX, uiSizeY, D3DX_DEFAULT, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED,
                                                             D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9**)&m_pD3DTexture)))
            return;
    }
    else if (imageInfo.ResourceType == D3DRTYPE_CUBETEXTURE)
    {
        if (FAILED(D3DXCreateCubeTextureFromFileInMemoryEx(m_pDevice, pData, uiSize, uiSizeX, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT,
                                                           D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DCubeTexture9**)&m_pD3DTexture)))
            return;
    }
    else
    {
        if (uiSizeX == D3DX_DEFAULT)
            uiSizeX = D3DX_DEFAULT_NONPOW2;
        if (uiSizeY == D3DX_DEFAULT)
            uiSizeY = D3DX_DEFAULT_NONPOW2;

        if (FAILED(D3DXCreateTextureFromFileInMemoryEx(m_pDevice, pData, uiSize, uiSizeX, uiSizeY, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT,
                                                       D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DTexture9**)&m_pD3DTexture)))
            return;

        D3DSURFACE_DESC desc;
        ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc(0, &desc);
        m_uiSurfaceSizeX = desc.Width;
        m_uiSurfaceSizeY = desc.Height;
    }

    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DTexture);
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::ReplaceFromFileMemory
//
// Upload a decoded image over the current D3D texture without replacing the CFileTextureItem.
// Create the new resource first so a failed decode keeps the previous (placeholder) texture drawable.
//
////////////////////////////////////////////////////////////////
bool CFileTextureItem::ReplaceFromFileMemory(const void* pData, uint uiSize, bool bMipMaps, ERenderFormat format)
{
    IDirect3DBaseTexture9* pOldTexture = m_pD3DTexture;
    const uint             uiOldSizeX = m_uiSizeX;
    const uint             uiOldSizeY = m_uiSizeY;
    const uint             uiOldSurfaceSizeX = m_uiSurfaceSizeX;
    const uint             uiOldSurfaceSizeY = m_uiSurfaceSizeY;
    const int              iOldMemoryKBUsed = m_iMemoryKBUsed;

    m_pD3DTexture = NULL;
    CreateUnderlyingDataFromFileMemory(pData, uiSize, bMipMaps, RDEFAULT, RDEFAULT, format);
    if (!m_pD3DTexture)
    {
        m_pD3DTexture = pOldTexture;
        m_uiSizeX = uiOldSizeX;
        m_uiSizeY = uiOldSizeY;
        m_uiSurfaceSizeX = uiOldSurfaceSizeX;
        m_uiSurfaceSizeY = uiOldSurfaceSizeY;
        m_iMemoryKBUsed = iOldMemoryKBUsed;
        return false;
    }

    SAFE_RELEASE(pOldTexture);
    m_iRevision++;
    m_pManager->UpdateMemoryUsage();
    return true;
}

////////////////////////////////////////////////////////////////
//
// CFileTextureItem::ReplaceFromPixels
//
// Same as ReplaceFromFileMemory, but through the pixels decoder (PLAIN is converted to PNG first).
//
////////////////////////////////////////////////////////////////
bool CFileTextureItem::ReplaceFromPixels(const CPixels* pPixels, bool bMipMaps, ERenderFormat format)
{
    if (!pPixels || !pPixels->GetSize())
        return false;

    IDirect3DBaseTexture9* pOldTexture = m_pD3DTexture;
    const uint             uiOldSizeX = m_uiSizeX;
    const uint             uiOldSizeY = m_uiSizeY;
    const uint             uiOldSurfaceSizeX = m_uiSurfaceSizeX;
    const uint             uiOldSurfaceSizeY = m_uiSurfaceSizeY;
    const int              iOldMemoryKBUsed = m_iMemoryKBUsed;

    m_pD3DTexture = NULL;
    CreateUnderlyingData(pPixels, bMipMaps, format);
    if (!m_pD3DTexture)
    {
        m_pD3DTexture = pOldTexture;
        m_uiSizeX = uiOldSizeX;
        m_uiSizeY = uiOldSizeY;
        m_uiSurfaceSizeX = uiOldSurfaceSizeX;
        m_uiSurfaceSizeY = uiOldSurfaceSizeY;
        m_iMemoryKBUsed = iOldMemoryKBUsed;
        return false;
    }

    SAFE_RELEASE(pOldTexture);
    m_iRevision++;
    m_pManager->UpdateMemoryUsage();
    return true;
}

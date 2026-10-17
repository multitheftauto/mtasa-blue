/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.FileTexture.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    // Upload PLAIN (BGRA) CPixels to a new D3DFMT_A8R8G8B8 texture, used for any input format that D3DX cannot decode itself
    // The caller must have already converted the source to EPixelsFormat::PLAIN via CPixelsManager
    bool CreateD3DTextureFromPlainPixels(IDirect3DDevice9* pDevice, CPixelsManagerInterface* pPixelsManager, const CPixels& plain, bool bMipMaps,
                                         IDirect3DTexture9*& pOutTexture, uint& uiOutWidth, uint& uiOutHeight)
    {
        pOutTexture = NULL;

        uint uiWidth = 0, uiHeight = 0;
        if (!pPixelsManager->GetPixelsSize(plain, uiWidth, uiHeight) || uiWidth == 0 || uiHeight == 0)
            return false;

        IDirect3DTexture9* pD3DTex = NULL;
        int                iMipLevels = bMipMaps ? 0 : 1;  // 0 = full chain
        if (FAILED(D3DXCreateTexture(pDevice, uiWidth, uiHeight, iMipLevels, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pD3DTex)))
            return false;

        D3DLOCKED_RECT locked = {};
        if (FAILED(pD3DTex->LockRect(0, &locked, NULL, 0)))
        {
            pD3DTex->Release();
            return false;
        }

        const uint  uiRowBytes = uiWidth * 4;
        const BYTE* pSrc = reinterpret_cast<const BYTE*>(plain.GetData());
        BYTE*       pDst = reinterpret_cast<BYTE*>(locked.pBits);
        for (uint y = 0; y < uiHeight; ++y)
            memcpy(pDst + y * locked.Pitch, pSrc + y * uiRowBytes, uiRowBytes);

        pD3DTex->UnlockRect(0);

        if (bMipMaps)
            D3DXFilterTexture(pD3DTex, NULL, D3DX_DEFAULT, D3DX_DEFAULT);

        pOutTexture = pD3DTex;
        uiOutWidth = uiWidth;
        uiOutHeight = uiHeight;
        return true;
    }
}

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

    // D3DX cannot decode every format we support, so we need to perform the following steps:
    // Load the file, ask CPixelsManager what it is, and if it's not something D3DX can handle, convert to PLAIN and upload manually
    if (strFilename.EndsWithI(".webp"))
    {
        std::vector<char> fileBytes;
        if (FileLoad(strFilename, fileBytes) && !fileBytes.empty())
        {
            CPixelsManagerInterface* pPixelsManager = CCore::GetSingleton().GetGraphics()->GetPixelsManager();

            CPixels srcPixels;
            srcPixels.externalData.pData = fileBytes.data();
            srcPixels.externalData.uiSize = (uint)fileBytes.size();

            if (pPixelsManager->GetPixelsFormat(srcPixels) == EPixelsFormat::WEBP)
            {
                CPixels plainPixels;
                if (!pPixelsManager->ChangePixelsFormat(srcPixels, plainPixels, EPixelsFormat::PLAIN))
                    return;

                IDirect3DTexture9* pD3DTex = NULL;
                uint               uiWidth = 0, uiHeight = 0;
                if (!CreateD3DTextureFromPlainPixels(m_pDevice, pPixelsManager, plainPixels, bMipMaps, pD3DTex, uiWidth, uiHeight))
                    return;

                m_uiSizeX = uiWidth;
                m_uiSizeY = uiHeight;

                D3DSURFACE_DESC desc;
                pD3DTex->GetLevelDesc(0, &desc);
                m_uiSurfaceSizeX = desc.Width;
                m_uiSurfaceSizeY = desc.Height;

                m_pD3DTexture = pD3DTex;
                m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
                return;
            }
        }
    }

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
    const CPixels*    pPixels = pInPixels;
    CPixels           pixelsTemp;
    EPixelsFormatType inFormat = pPixelsManager->GetPixelsFormat(*pPixels);
    if (inFormat == EPixelsFormat::PLAIN)
    {
        pPixelsManager->ChangePixelsFormat(*pPixels, pixelsTemp, EPixelsFormat::PNG);
        pPixels = &pixelsTemp;
    }
    else if (inFormat == EPixelsFormat::WEBP)  // any format that D3DX cannot decode itself
    {
        CPixels plainPixels;
        if (!pPixelsManager->ChangePixelsFormat(*pPixels, plainPixels, EPixelsFormat::PLAIN))
            return;

        IDirect3DTexture9* pD3DTex = NULL;
        uint               uiWidth = 0, uiHeight = 0;
        if (!CreateD3DTextureFromPlainPixels(m_pDevice, pPixelsManager, plainPixels, bMipMaps, pD3DTex, uiWidth, uiHeight))
            return;

        m_uiSizeX = uiWidth;
        m_uiSizeY = uiHeight;

        D3DSURFACE_DESC desc;
        pD3DTex->GetLevelDesc(0, &desc);
        m_uiSurfaceSizeX = desc.Width;
        m_uiSurfaceSizeY = desc.Height;

        m_pD3DTexture = pD3DTex;
        m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage(m_pD3DTexture);
        return;
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
// CFileTextureItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DTexture);
}

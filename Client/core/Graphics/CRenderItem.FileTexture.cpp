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

    // Store for recreation
    m_bMipMaps = bMipMaps;
    m_uiCreateSizeX = uiSizeX;
    m_uiCreateSizeY = uiSizeY;
    m_CreateFormat = format;

    // Initial creation of d3d data
    if (pPixels)
    {
        m_bIsFromFile = false;
        m_bIsBlank = false;
        m_PixelData.SetSize(pPixels->GetSize());
        memcpy(m_PixelData.GetData(), pPixels->GetData(), pPixels->GetSize());
        CreateUnderlyingData(pPixels, bMipMaps, format);
    }
    else if (!strFilename.empty())
    {
        m_bIsFromFile = true;
        m_bIsBlank = false;
        m_strFilename = strFilename;
        CreateUnderlyingData(strFilename, bMipMaps, uiSizeX, uiSizeY, format);
    }
    else
    {
        m_bIsFromFile = false;
        m_bIsBlank = true;
        CreateUnderlyingData(bMipMaps, uiSizeX, uiSizeY, format, textureType, uiVolumeDepth);
    }
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
    // Release D3DPOOL_DEFAULT textures
    ReleaseUnderlyingData();
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
    // Recreate D3DPOOL_DEFAULT textures
    if (m_bIsFromFile)
    {
        CreateUnderlyingData(m_strFilename, m_bMipMaps, m_uiCreateSizeX, m_uiCreateSizeY, m_CreateFormat);
    }
    else if (!m_bIsBlank)
    {
        if (m_PixelData.GetSize() > 0)
        {
            CPixels pixels;
            pixels.SetSize(m_PixelData.GetSize());
            memcpy(pixels.GetData(), m_PixelData.GetData(), m_PixelData.GetSize());
            CreateUnderlyingData(&pixels, m_bMipMaps, m_CreateFormat);
        }
    }
    else
    {
        CreateUnderlyingData(m_bMipMaps, m_uiCreateSizeX, m_uiCreateSizeY, m_CreateFormat, m_TextureType, m_uiVolumeDepth);
    }
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

    // Read file into VirtualAlloc memory to ensure it returns to OS
    FILE* fh = File::Fopen(strFilename, "rb");
    if (!fh)
        return;
    
    fseek(fh, 0, SEEK_END);
    size_t fileSize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    if (fileSize == 0 || fileSize > 1024 * 1024 * 512) // Limit 512MB
    {
        fclose(fh);
        return;
    }
    
    // VirtualAlloc with MEM_COMMIT | MEM_RESERVE
    void* fileData = VirtualAlloc(NULL, fileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!fileData)
    {
        fclose(fh);
        return;
    }
    
    size_t bytesRead = fread(fileData, 1, fileSize, fh);
    fclose(fh);
    
    if (bytesRead != fileSize)
    {
        VirtualFree(fileData, 0, MEM_RELEASE);
        return;
    }

    D3DXIMAGE_INFO imageInfo;
    if (FAILED(D3DXGetImageInfoFromFileInMemory(fileData, fileSize, &imageInfo)))
    {
        VirtualFree(fileData, 0, MEM_RELEASE);
        return;
    }

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

    HRESULT hr = E_FAIL;
    
    // Changed D3DPOOL_MANAGED to D3DPOOL_DEFAULT to prevent system memory backup leak
    if (imageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE)
    {
        // It's a volume texture!
        hr = D3DXCreateVolumeTextureFromFileInMemoryEx(m_pDevice, fileData, fileSize, uiSizeX, uiSizeY, D3DX_DEFAULT, iMipMaps, 0, D3DFormat, D3DPOOL_DEFAULT,
                                                     D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9**)&m_pD3DTexture);
    }
    else if (imageInfo.ResourceType == D3DRTYPE_CUBETEXTURE)
    {
        // It's a cubemap texture!
        hr = D3DXCreateCubeTextureFromFileInMemoryEx(m_pDevice, fileData, fileSize, uiSizeX, iMipMaps, 0, D3DFormat, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                   NULL, NULL, (IDirect3DCubeTexture9**)&m_pD3DTexture);
    }
    else
    {
        // It's none of the above!

        // If size not specified, try to use exact image size to prevent blurring
        if (uiSizeX == D3DX_DEFAULT)
            uiSizeX = D3DX_DEFAULT_NONPOW2;
        if (uiSizeY == D3DX_DEFAULT)
            uiSizeY = D3DX_DEFAULT_NONPOW2;

        hr = D3DXCreateTextureFromFileInMemoryEx(m_pDevice, fileData, fileSize, uiSizeX, uiSizeY, iMipMaps, 0, D3DFormat, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                               NULL, NULL, (IDirect3DTexture9**)&m_pD3DTexture);

        if (SUCCEEDED(hr) && m_pD3DTexture)
        {
            // Update surface size if it's a normal texture
            D3DSURFACE_DESC desc;
            ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc(0, &desc);
            m_uiSurfaceSizeX = desc.Width;
            m_uiSurfaceSizeY = desc.Height;
        }
    }
    
    // Free file buffer
    VirtualFree(fileData, 0, MEM_RELEASE);

    if (FAILED(hr))
    {
        // Ensure m_pD3DTexture is null if failed
        if (m_pD3DTexture)
        {
            m_pD3DTexture->Release();
            m_pD3DTexture = nullptr;
        }
        return;
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

    // Changed D3DPOOL_MANAGED to D3DPOOL_DEFAULT
    if (FAILED(D3DXCreateTextureFromFileInMemoryEx(m_pDevice,                     //__in     LPDIRECT3DDEVICE9 pDevice,
                                                   pPixels->GetData(),            //__in     LPCVOID pSrcData,
                                                   pPixels->GetSize(),            //__in     UINT SrcDataSize,
                                                   D3DX_DEFAULT,                  //__in     UINT Width,
                                                   D3DX_DEFAULT,                  //__in     UINT Height,
                                                   iMipMaps,                      //__in     UINT MipLevels,
                                                   0,                             //__in     DWORD Usage,
                                                   D3DFormat,                     //__in     D3DFORMAT Format,
                                                   D3DPOOL_DEFAULT,               //__in     D3DPOOL Pool,
                                                   D3DX_DEFAULT,                  //__in     DWORD Filter,
                                                   D3DX_DEFAULT,                  //__in     DWORD MipFilter,
                                                   0,                             //__in     D3DCOLOR ColorKey,
                                                   &imageInfo,                    //__inout  D3DXIMAGE_INFO *pSrcInfo,
                                                   NULL,                          //__out    PALETTEENTRY *pPalette,
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

    // Changed D3DPOOL_MANAGED to D3DPOOL_DEFAULT
    if (textureType == D3DRTYPE_VOLUMETEXTURE)
    {
        if (FAILED(D3DXCreateVolumeTexture(m_pDevice,                  //__in   LPDIRECT3DDEVICE9 pDevice,
                                           uiSizeX,                    //__in   UINT Width,
                                           uiSizeY,                    //__in   UINT Height,
                                           uiVolumeDepth,              //__in   UINT Depth,
                                           iMipMaps,                   //__in   UINT MipLevels,
                                           0,                          //__in   DWORD Usage,
                                           D3DFormat,                  //__in   D3DFORMAT Format,
                                           D3DPOOL_DEFAULT,            //__in   D3DPOOL Pool,
                                           (IDirect3DVolumeTexture9**)&m_pD3DTexture)))
            return;
    }
    else if (textureType == D3DRTYPE_CUBETEXTURE)
    {
        if (FAILED(D3DXCreateCubeTexture(m_pDevice,                  //__in   LPDIRECT3DDEVICE9 pDevice,
                                         uiSizeX,                    //__in   UINT Width,
                                         iMipMaps,                   //__in   UINT MipLevels,
                                         0,                          //__in   DWORD Usage,
                                         D3DFormat,                  //__in   D3DFORMAT Format,
                                         D3DPOOL_DEFAULT,            //__in   D3DPOOL Pool,
                                         (IDirect3DCubeTexture9**)&m_pD3DTexture)))
            return;
    }
    else
    {
        if (FAILED(D3DXCreateTexture(m_pDevice,                  //__in   LPDIRECT3DDEVICE9 pDevice,
                                     uiSizeX,                    //__in   UINT Width,
                                     uiSizeY,                    //__in   UINT Height,
                                     iMipMaps,                   //__in   UINT MipLevels,
                                     0,                          //__in   DWORD Usage,
                                     D3DFormat,                  //__in   D3DFORMAT Format,
                                     D3DPOOL_DEFAULT,            //__in   D3DPOOL Pool,
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
    if (m_pD3DTexture && m_pDevice)
    {
        // Force unbind from all stages to ensure immediate VRAM release
        for (DWORD i = 0; i < 8; i++)
        {
            IDirect3DBaseTexture9* pCurTex = NULL;
            if (SUCCEEDED(m_pDevice->GetTexture(i, &pCurTex)))
            {
                if (pCurTex == m_pD3DTexture)
                {
                    m_pDevice->SetTexture(i, NULL);
                }
                if (pCurTex)
                    pCurTex->Release();
            }
        }
    }
    SAFE_RELEASE(m_pD3DTexture);
}

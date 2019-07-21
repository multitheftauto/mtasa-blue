#pragma once

#include "StdInc.h"
#include "CDXTexture.h"

CDXTexture::CDXTexture()
{
    Initialize();
}

CDXTexture::CDXTexture(eCustomRWTextureDelete deleteSetting)
{
    Initialize();
    m_deleteSetting = deleteSetting;
}

CDXTexture::CDXTexture(RwTexture* pTexture, eCustomRWTextureDelete deleteSetting)
{
    Initialize();
    m_deleteSetting = deleteSetting;
    m_OriginalTexture = pTexture;
    m_pTexture = pTexture;
    _rwD3D9RasterExt* rasterExt = pRenderware->GetRasterExt(m_pTexture->raster);
    //printf("[Constructor] CDXTexture::CDXTexture: %s\n", m_pTexture->name);
    if (rasterExt->d3dFormat != m_kTextureFormat)
    {
        m_pTexture = pRenderware->RwTextureCreateWithFormat(m_pTexture, m_kTextureFormat, rwRASTERFORMAT8888);
        rasterExt = pRenderware->GetRasterExt(m_pTexture->raster);
        bNewRwTextureCreated = m_pTexture ? true : false;
    }
    dxTexture = rasterExt->texture;
    imageWidth = m_pTexture->raster->width;
    imageHeight = m_pTexture->raster->height;
}

CDXTexture::~CDXTexture()
{
    UnlockTexture();

    if (bNewRwTextureCreated)
    {
        if (m_deleteSetting == RW_TEXTURE_DELETE_ON_UNLOAD)
        {
            //printf("[Destructor] ~CDXTexture called | m_pTexture: %s\n", m_pTexture->name);
            pRenderware->DestroyTextureForcefully(m_pTexture);
            m_pTexture = nullptr;
        }
    }
}

bool CDXTexture::Compress(D3DFORMAT format)
{
    _rwD3D9RasterExt* rasterExt = pRenderware->GetRasterExt(m_pTexture->raster);
    if (rasterExt->d3dFormat != format)
    {
        RwTexture* pNewTexture = pRenderware->RwTextureCreateWithFormat(m_pTexture, format, rwRASTERFORMAT1555);
        if (!pNewTexture)
        {
            return false;
        }
        if (bNewRwTextureCreated)
        {
            pRenderware->DestroyTextureForcefully(m_pTexture);
        }
        m_pTexture = pNewTexture;
    }
    return true;
}

void CDXTexture::SaveTextureToFile(std::string& name)
{
    UnlockTexture();

    HRESULT hr = D3DXSaveTextureToFile(("C:\\Users\\danish\\Desktop\\" + name + ".png").c_str(), D3DXIFF_PNG, dxTexture, NULL);
    if (FAILED(hr))
    {
        printf("failed to save texture file | %s | hr = %#.8x\n", ("C:\\Users\\danish\\Desktop\\" + name).c_str(), hr);
    }
}

void CDXTexture::Initialize()
{
    m_OriginalTexture = nullptr;
    m_deleteSetting = RW_TEXTURE_DONT_DELETE;
    dxTexture = nullptr;
    bNewRwTextureCreated = false;
    bTextureLocked = false;
    pixels = nullptr;
    imageWidth = 0;
    imageHeight = 0;
    m_pixelSizeInBytes = 4;
    memset(&textureLockedRect, 0, sizeof(D3DLOCKED_RECT));
    pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();
}

void* CDXTexture::GetPixel(int x, int y)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    return (void*)(pixels + theOffset);
}

bool CDXTexture::LockTexture(DWORD Flags)
{
    RECT textureRect;
    textureRect.left = textureRect.top = 0;
    textureRect.right = imageWidth;
    textureRect.bottom = imageHeight;

    HRESULT hr = dxTexture->LockRect(0, &textureLockedRect, &textureRect, Flags);
    if (hr != S_OK)
    {
        printf("pTexture->LockRect: failed to lock textureRect\n");
        return false;
    }

    pixels = (unsigned char*)textureLockedRect.pBits;
    m_pixelPitch = textureLockedRect.Pitch;
    bTextureLocked = true;
    return true;
}

void CDXTexture::UnlockTexture()
{
    if (bTextureLocked)
    {
        HRESULT hr = dxTexture->UnlockRect(0);
        assert(hr == S_OK);
        bTextureLocked = false;
    }
}

bool CDXTexture::CreateTextureLocked(unsigned int width, unsigned int height, DWORD lockFlags)
{
    CRenderWare* pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();

    auto RwTextureCreate = (RwTexture * (__cdecl*)(RwRaster * raster))0x007F37C0;
    auto RwRasterLock = (RwUInt8 * (__cdecl*)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode))0x07FB2D0;
    auto RwRasterUnlock = (RwRaster * (__cdecl*)(RwRaster * raster))0x7FAEC0;
    auto RwD3D9RasterCreate = (RwRaster * (__cdecl*)(RwUInt32 width, RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags))0x4CD050;

    imageWidth = width;
    imageHeight = height;

    RwRaster* raster = RwD3D9RasterCreate(width, height, m_kTextureFormat, rwRASTERTYPETEXTURE | (m_kTextureFormat & 0x9000));
    if (!raster)
    {
        std::printf("CreateAtlasTextureResource: RwD3D9RasterCreate Failed\n");
        return false;
    }

    m_pTexture = RwTextureCreate(raster);
    if (!m_pTexture)
    {
        std::printf("CreateAtlasTextureResource: RwTextureCreate Failed\n");
        return false;
    }
    else
    {
        bNewRwTextureCreated = true;
        _rwD3D9RasterExt* destinationRasterExt = pRenderware->GetRasterExt(raster);
        dxTexture = destinationRasterExt->texture;
        printf("texture loaded with width, height = %u, %u\n", imageWidth, imageHeight);
        if (!LockTexture(lockFlags))
        {
            printf("LockTexture() fail \n");
            return false;
        }
    }

    return true;
}

void CDXTexture::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    PixelColor*  pPixel = (PixelColor*)(pixels + theOffset);
    pPixel->a = alpha;
    pPixel->r = red;
    pPixel->g = green;
    pPixel->b = blue;
}

void CDXTexture::setPixel(int x, int y, PixelColor* thePixel)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    PixelColor*  pPixel = (PixelColor*)(pixels + theOffset);
    *pPixel = *thePixel;
}

void CDXTexture::fillPixels(int red, int green, int blue)
{
    for (int y = 0; y < imageHeight; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {
            setPixel(x, y, red, green, blue, 255);
        }
    }
}

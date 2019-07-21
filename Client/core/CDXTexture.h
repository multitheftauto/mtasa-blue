#pragma once

#include <d3d9.h>
#include <D3dx9tex.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include "..\Client\sdk\game\RenderWare.h"

struct PixelColor
{
    unsigned char a, r, g, b;
};

enum eCustomRWTextureDelete
{
    RW_TEXTURE_DELETE_ON_UNLOAD = 0,
    RW_TEXTURE_DONT_DELETE
};

class CDXTexture
{
public:
    RwTexture*             m_OriginalTexture;
    const D3DFORMAT        m_kTextureFormat = D3DFMT_A8R8G8B8;
    RwTexture*             m_pTexture;
    IDirect3DTexture9*     dxTexture;
    unsigned int           imageWidth, imageHeight;
    unsigned char*         pixels;
    int                    m_pixelSizeInBytes;
    int                    m_pixelPitch;
    D3DLOCKED_RECT         textureLockedRect;
    bool                   bTextureLocked;
    bool                   bNewRwTextureCreated;
    eCustomRWTextureDelete m_deleteSetting;
    CRenderWare*           pRenderware;

    CDXTexture();
    CDXTexture(eCustomRWTextureDelete deleteSetting);
    CDXTexture(RwTexture* pTexture, eCustomRWTextureDelete deleteSetting);
    ~CDXTexture();

    void         Initialize();
    bool         Compress(D3DFORMAT format);
    void         SaveTextureToFile(std::string& name);
    unsigned int GetWidth() { return imageWidth; }
    unsigned int GetHeight() { return imageHeight; }

    RwTexture*  GetRwTexture() { return m_pTexture; }
    RwTexture*  GetOriginalRwTexture() { return m_OriginalTexture; }
    void*       GetPixel(int x, int y);

    bool LockTexture(DWORD Flags);
    void UnlockTexture();

    bool CreateTextureLocked(unsigned int width, unsigned int height, DWORD lockFlags);

    void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void setPixel(int x, int y, PixelColor* thePixel);
    void fillPixels(int red, int green, int blue);
};

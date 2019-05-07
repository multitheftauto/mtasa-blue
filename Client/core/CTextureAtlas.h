#pragma once
#include <dxgiformat.h>

class CTextureAtlas : public CTextureAtlasInterface
{
public:
    CTextureAtlas();
    CTextureAtlas(std::vector <CTextureInfo>& vecTexturesInfo);
    void Initialize();
    bool CreateAtlas();
    bool CreateAtlasTextureResource(float fWidth, float fHeight);
    bool LoadTextureFromFile(CTextureInfo &textureInfo);
    bool CopyTextureToAtlas(CTextureInfo& textureInfo);
    bool IsSupportedFormat(D3DFORMAT format);
    int SizeOfTexel(D3DFORMAT format);
    bool IsDXTnFormat(D3DFORMAT format);

    IDirect3DTexture9* GetAtlasTexture() { return m_pAtlasTexture; }
    const CVector2D& GetSize() { return vecAtlasSize; }
    float GetWidth() { return vecAtlasSize.fX; }
    float GetHeight() { return vecAtlasSize.fY; }

private:
    std::vector <CTextureInfo>* m_pVecTexturesInfo;
    IDirect3DTexture9* m_pAtlasTexture;
    DWORD m_dwMaximumMipMapLevel;
    CVector2D vecAtlasSize;
    const D3DFORMAT m_kTextureFormat = D3DFMT_A8R8G8B8;
    const DXGI_FORMAT m_kTextureDxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
};

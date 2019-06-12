#pragma once

class CTextureAtlas
{
public:
    CTextureAtlas();

    bool RemoveTextureInfoTillSuccess(std::vector <RwTexture*>& vecTexturesRemoved);
    void AddTextureInfo(RwTexture* pTexture, const float theWidth, const float theHeight);
    bool CreateAtlasTextureResource(const float fWidth, const float fHeight);
    bool CopyTextureToAtlas(CTextureInfo& textureInfo);

    eTextureAtlasErrorCodes CreateAtlas();
    void        GetRasterRect(RwRaster* raster, RECT& rect);
    bool       IsTextureFormatDifferentFromAtlas(CTextureInfo& textureInfo);
    RwTexture* CreateTextureWithAtlasFormat(CTextureInfo &textureInfo);

    bool IsSupportedFormat(D3DFORMAT format);
    int  SizeOfTexel(D3DFORMAT format);
    bool IsDXTnFormat(D3DFORMAT format);

    size_t GetTextureInfoCount() { return m_vecTexturesInfo.size(); }
    CTextureInfo& GetTextureInfo(size_t index) { return m_vecTexturesInfo[index]; }
    CTextureInfo* GetTextureInfoByName(unsigned int uiTextureNameHash);

    RwTexture * GetAtlasTexture() { return m_pAtlasTexture; }
    const CVector2D& GetSize() { return vecAtlasSize; }
    float GetWidth() { return vecAtlasSize.fX; }
    float GetHeight() { return vecAtlasSize.fY; }

private:
    const D3DFORMAT m_kTextureFormat = D3DFMT_A8R8G8B8;
    std::vector <CTextureInfo> m_vecTexturesInfo;
    RwTexture* m_pAtlasTexture;
    CVector2D vecAtlasSize;
};

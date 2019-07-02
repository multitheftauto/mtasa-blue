#pragma once
#include <xatlas.h>
#include "CDXTexture.h"
#include "xatlas_repack.h"

class CTextureAtlas
{
public:
    CTextureAtlas(xatlas::Atlas* atlas, xatlas::PackOptions& packOptions, std::vector<uint16_t>& vertexToMaterial, std::vector<CDXTexture>& texturesCache,
                  std::vector<uint32_t>& textures, std::vector<Vector2>& uvs);
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


    std::vector<CDXTexture> atlasDXTextures;

private:
    
};

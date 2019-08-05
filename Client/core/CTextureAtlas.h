#pragma once
#include <xatlas.h>
#include "CDXTexture.h"
#include "xatlas_repack.h"

struct RpAtomic;

class CTextureAtlas
{
public:
    CTextureAtlas(RpClump* pClump, xatlas::Atlas* atlas, xatlas::PackOptions& packOptions, std::vector<uint16_t>& vertexToMaterial,
                  std::vector<CDXTexture>& texturesCache, std::vector<uint32_t>& textures, std::vector<Vector2>& uvs, std::vector<RpAtomic*>& outAtomicList);

    eTextureAtlasErrorCodes CreateAtlas();
    std::vector<CDXTexture> atlasDXTextures;

private:
    
};

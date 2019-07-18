#include "StdInc.h"
#include <rectpack2D\src\finders_interface.h>
#include "CTextureAtlas.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <xatlas.h>
#include "xatlas_repack.h"

#include "..\..\Client\sdk\core\CTextureAtlasInterface.h"

using namespace rectpack2D;

auto RwTextureCreate = (RwTexture * (__cdecl*)(RwRaster * raster))0x007F37C0;
auto RwRasterLock = (RwUInt8 * (__cdecl*)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode))0x07FB2D0;
auto RwRasterUnlock = (RwRaster * (__cdecl*)(RwRaster * raster))0x7FAEC0;
auto RwD3D9RasterCreate = (RwRaster * (__cdecl*)(RwUInt32 width, RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags))0x4CD050;
auto WriteRaster = (bool(__cdecl*)(RwRaster* raster, char* filename))0x5A4150;

extern CCore* g_pCore;

void DilateAtlasTextures(xatlas::Atlas* atlas, xatlas::PackOptions& packOptions, uint8_t* atlasTexture, AtlasLookupTexel* atlasLookup,
                         unsigned int atlasTextureSize, unsigned int atlasLookupSize, std::vector<CDXTexture>& texturesCache, std::vector<uint32_t>& textures)
{
#define DEBUG_DILATE 0
    // Run a dilate filter on the atlas texture to fill in padding around charts so bilinear filtering doesn't sample empty texels.
    // Sample from the source texture(s).
    printf("Dilating atlas texture\n");
    std::vector<uint8_t> tempAtlasTexture;
    tempAtlasTexture.resize(atlasTextureSize);
    std::vector<AtlasLookupTexel> tempAtlasLookup;
    tempAtlasLookup.resize(atlasLookupSize);
    const int sampleXOffsets[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int sampleYOffsets[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for (uint32_t i = 0; i < packOptions.padding; i++)
    {
        memcpy(tempAtlasTexture.data(), atlasTexture, atlasTextureSize * sizeof(uint8_t));
        memcpy(tempAtlasLookup.data(), atlasLookup, atlasLookupSize * sizeof(AtlasLookupTexel));
        for (uint32_t y = 0; y < atlas->height; y++)
        {
            for (uint32_t x = 0; x < atlas->width; x++)
            {
                const uint32_t atlasDataOffset = x * 4 + y * (atlas->width * 4);
                if (tempAtlasTexture[atlasDataOffset + 3] != 0)
                    continue;            // Alpha != 0, already data here.
                // Try to sample directly from the source texture.
                // Need to find source texel position by checking surrounding texels in the atlas.
                bool foundSample = false;
                for (uint32_t si = 0; si < 8; si++)
                {
                    const int sx = (int)x + sampleXOffsets[si];
                    const int sy = (int)y + sampleYOffsets[si];
                    if (sx < 0 || sy < 0 || sx >= (int)atlas->width || sy >= (int)atlas->height)
                        continue;            // Sample position is outside of atlas texture.
                    const AtlasLookupTexel& lookup = tempAtlasLookup[sx + sy * (int)atlas->width];
                    if (lookup.materialIndex == UINT16_MAX || textures[lookup.materialIndex] == UINT32_MAX)
                        continue;            // No source data here.
                                             // This atlas texel has a corresponding position for the source texel.
                                             // Subtract the sample offset to get the source position.
                    CDXTexture* sourceTexture = &texturesCache[textures[lookup.materialIndex]];
                    const int   ssx = (int)lookup.x - sampleXOffsets[si];
                    const int   ssy = (int)lookup.y - sampleYOffsets[si] * -1;            // need to flip y?
                    if (ssx < 0 || ssy < 0 || ssx >= (int)sourceTexture->GetWidth() || ssy >= (int)sourceTexture->GetHeight())
                        continue;            // Sample position is outside of source texture.
                    // Valid sample.
                    // const uint8_t* rgbaSource =
                    //    &sourceTexture->data[ssx * sourceTexture->numComponents + ssy * (sourceTexture->width * sourceTexture->numComponents)];
                    const PixelColor* argbSource = (const PixelColor*)sourceTexture->GetPixel(ssx, ssy);
                    uint8_t*          rgbaDest = &atlasTexture[atlasDataOffset];
#if DEBUG_DILATE
                    rgbaDest[0] = 0;
                    rgbaDest[1] = 255;
                    rgbaDest[2] = 0;
                    rgbaDest[3] = 255;
#else

                    rgbaDest[0] = argbSource->r;
                    rgbaDest[1] = argbSource->g;
                    rgbaDest[2] = argbSource->b;
                    rgbaDest[3] = argbSource->a;
#endif
                    atlasLookup[x + y * (int)atlas->width].x = (uint16_t)ssx;
                    atlasLookup[x + y * (int)atlas->width].y = (uint16_t)ssy;
                    atlasLookup[x + y * (int)atlas->width].materialIndex = lookup.materialIndex;
                    foundSample = true;
                    break;
                }
                if (foundSample)
                    continue;
                // Sample up to 8 surrounding texels in the source texture, average their color and assign it to this texel.
                float rgbSum[4] = {0.0f, 0.0f, 0.0f, 0.0f}, n = 0;
                for (uint32_t si = 0; si < 8; si++)
                {
                    const int sx = (int)x + sampleXOffsets[si];
                    const int sy = (int)y + sampleYOffsets[si];
                    if (sx < 0 || sy < 0 || sx >= (int)atlas->width || sy >= (int)atlas->height)
                        continue;            // Sample position is outside of atlas texture.
                    const AtlasLookupTexel& lookup = tempAtlasLookup[sx + sy * (int)atlas->width];
                    if (lookup.materialIndex == UINT16_MAX || textures[lookup.materialIndex] == UINT32_MAX)
                        continue;            // No source data here.
                    CDXTexture* sourceTexture = &texturesCache[textures[lookup.materialIndex]];
                    const int   ssx = (int)lookup.x + sampleXOffsets[si];
                    const int   ssy = (int)lookup.y + sampleYOffsets[si];
                    if (ssx < 0 || ssy < 0 || ssx >= (int)sourceTexture->GetWidth() || ssy >= (int)sourceTexture->GetHeight())
                        continue;            // Sample position is outside of source texture.
                    // Valid sample.
                    const PixelColor* rgba = (const PixelColor*)sourceTexture->GetPixel(ssx, ssy);
                    // const uint8_t* rgba =
                    //    &sourceTexture->data[ssx * sourceTexture->numComponents + ssy * (sourceTexture->width * sourceTexture->numComponents)];
                    rgbSum[0] += (float)rgba->r;
                    rgbSum[1] += (float)rgba->g;
                    rgbSum[2] += (float)rgba->b;
                    rgbSum[3] += (float)rgba->a;
                    n++;
                }
                if (n != 0)
                {
                    const float invn = 1.0f / (float)n;
                    uint8_t*    rgba = &atlasTexture[atlasDataOffset];
#if DEBUG_DILATE
                    rgba[0] = 255;
                    rgba[1] = 0;
                    rgba[2] = 255;
                    rgba[3] = 255;
#else
                    rgba[0] = uint8_t(rgbSum[0] * invn);
                    rgba[1] = uint8_t(rgbSum[1] * invn);
                    rgba[2] = uint8_t(rgbSum[2] * invn);
                    rgba[3] = uint8_t(rgbSum[3] * invn);
#endif
                    continue;
                }
                // Sample up to 8 surrounding texels in the atlas texture, average their color and assign it to this texel.
                rgbSum[0] = rgbSum[1] = rgbSum[2] = rgbSum[3] = 0.0f;
                n = 0;
                for (uint32_t si = 0; si < 8; si++)
                {
                    const int sx = (int)x + sampleXOffsets[si];
                    const int sy = (int)y + sampleYOffsets[si];
                    if (sx < 0 || sy < 0 || sx >= (int)atlas->width || sy >= (int)atlas->height)
                        continue;            // Sample position is outside of atlas texture.
                    const uint8_t* rgba = &tempAtlasTexture[sx * 4 + sy * (atlas->width * 4)];
                    if (rgba[3] == 0)
                        continue;
                    rgbSum[0] += (float)rgba[0];
                    rgbSum[1] += (float)rgba[1];
                    rgbSum[2] += (float)rgba[2];
                    rgbSum[3] += (float)rgba[3];
                    n++;
                }
                if (n != 0)
                {
                    const float invn = 1.0f / (float)n;
                    uint8_t*    rgba = &atlasTexture[atlasDataOffset];
#if DEBUG_DILATE
                    rgba[0] = 0;
                    rgba[1] = 255;
                    rgba[2] = 255;
                    rgba[3] = 255;
#else
                    rgba[0] = uint8_t(rgbSum[0] * invn);
                    rgba[1] = uint8_t(rgbSum[1] * invn);
                    rgba[2] = uint8_t(rgbSum[2] * invn);
                    rgba[3] = uint8_t(rgbSum[3] * invn);
#endif
                }
            }
        }
    }
}

CTextureAtlas::CTextureAtlas(RpClump* pClump, xatlas::Atlas* atlas, xatlas::PackOptions& packOptions, std::vector<uint16_t>& vertexToMaterial,
                             std::vector<CDXTexture>& texturesCache, std::vector<uint32_t>& textures, std::vector<Vector2>& uvs)
{
    unsigned int atlasTextureSize = atlas->width * atlas->height * 4;
    unsigned int atlasLookupSize = atlas->width * atlas->height;

    // Create a texture for the atlas.
    std::vector<uint8_t> atlasTexture;
    atlasTexture.resize(atlas->atlasCount * atlasTextureSize);
    memset(atlasTexture.data(), 0, atlasTexture.size() * sizeof(uint8_t));

    // Need to lookup source position and material for dilation.
    std::vector<AtlasLookupTexel> atlasLookup;
    atlasLookup.resize(atlas->atlasCount * atlasLookupSize);
    for (size_t i = 0; i < atlasLookup.size(); i++)
    {
        atlasLookup[i].materialIndex = UINT16_MAX;
    }

    CRenderWare*           pRenderWare = g_pCore->GetGame()->GetRenderWare();
    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    size_t FirstVertexIndex = 0;
    // Rasterize chart triangles.
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*           pAtomic = outAtomicList[geometryIndex];
        RpGeometry*         pGeometry = pAtomic->geometry;
        const xatlas::Mesh& mesh = atlas->meshes[geometryIndex];
        for (uint32_t j = 0; j < mesh.chartCount; j++)
        {
            const xatlas::Chart& chart = mesh.chartArray[j];
            SetAtlasTexelArgs    args;
            args.materialIndex = vertexToMaterial[chart.indexArray[0]];
            if (args.materialIndex == UINT16_MAX || textures[args.materialIndex] == UINT32_MAX)
                args.sourceTexture = nullptr;
            else
                args.sourceTexture = &texturesCache[textures[args.materialIndex]];

            for (uint32_t k = 0; k < chart.indexCount / 3; k++)
            {
                Vector2 v[3];
                int32_t atlasIndex = -1;
                for (uint32_t l = 0; l < 3; l++)
                {
                    const uint32_t        index = chart.indexArray[k * 3 + l];
                    const xatlas::Vertex& vertex = mesh.vertexArray[index];
                    size_t                sourceVertexIndex = vertex.xref + FirstVertexIndex;
                    v[l] = Vector2(vertex.uv[0], vertex.uv[1]);
                    args.sourceUv[l] = uvs[sourceVertexIndex];
                    args.sourceUv[l].y = 1.0f - args.sourceUv[l].y;

                    atlasIndex = vertex.atlasIndex;
                }

                if (atlasIndex < 0)
                {
                    atlasIndex = 0;
                }
                Triangle tri(v[0], v[1], v[2], Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
                args.atlasData = ((uint8_t*)atlasTexture.data()) + (atlasTextureSize * atlasIndex);
                args.atlasWidth = atlas->width;
                args.atlasLookup = ((AtlasLookupTexel*)atlasLookup.data()) + (atlasLookupSize * atlasIndex);
                tri.drawAA(setAtlasTexel, &args);
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
    }
    if (packOptions.padding > 0)
    {
        for (uint32_t atlasIndex = 0; atlasIndex < atlas->atlasCount; atlasIndex++)
        {
            uint8_t*          theAtlasTexture = ((uint8_t*)atlasTexture.data()) + (atlasTextureSize * atlasIndex);
            AtlasLookupTexel* theAtlasLookup = ((AtlasLookupTexel*)atlasLookup.data()) + (atlasLookupSize * atlasIndex);

            DilateAtlasTextures(atlas, packOptions, theAtlasTexture, theAtlasLookup, atlasTextureSize, atlasLookupSize, texturesCache, textures);

            CDXTexture dxTexture;
            atlasDXTextures.push_back(dxTexture);
            CDXTexture& theTexture = atlasDXTextures[atlasDXTextures.size() - 1];

            if (theTexture.CreateTextureLocked(atlas->width, atlas->height, 0))
            {
                for (int y = 0; y < atlas->height; y++)
                {
                    for (int x = 0; x < atlas->width; x++)
                    {
                        const uint32_t atlasDataOffset = x * 4 + y * (atlas->width * 4);
                        const uint8_t* thePixel = &theAtlasTexture[atlasDataOffset];
                        theTexture.setPixel(x, y, thePixel[0], thePixel[1], thePixel[2], thePixel[3]);
                    }
                }

                char buffer[100];
                sprintf(buffer, "myAtlas%d", atlasIndex);
                std::string atlasName = buffer;
                theTexture.UnlockTexture();
                //theTexture.SaveTextureToFile(atlasName);
            }
        }
    }
}

eTextureAtlasErrorCodes CTextureAtlas::CreateAtlas()
{
    return TEX_ATLAS_SUCCESS;
}

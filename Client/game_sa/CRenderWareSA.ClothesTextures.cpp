/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.ClothesTextures.cpp
 *  PURPOSE:     Resolution-agnostic reimplementation of the GTA SA clothes
 *               texture compositing primitives, allowing custom CJ clothing
 *               textures of any size (up to 1024x1024) instead of 256x256
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CVector.h>
#include <game/RenderWareD3D.h>
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "gamesa_renderware.h"
#include <cstdint>
#include <cstring>

extern CGameSA* pGame;

namespace
{
    constexpr int MAX_DIMENSIONS = 1024;

    // RenderWare raster constants
    constexpr int          RW_RASTER_TYPE_TEXTURE = 0x04;
    constexpr int          RW_RASTER_LOCK_READ = 0x01;
    constexpr int          RW_RASTER_LOCK_WRITE = 0x02;
    constexpr int          RW_RASTER_LOCK_READWRITE = 0x03;
    constexpr unsigned int RW_FILTER_LINEAR = 0x02;

    // GTA SA clothes builder texture compositing primitives
    constexpr DWORD ADDR_CopyTexture = 0x5A5730;
    constexpr DWORD ADDR_PlaceTextureOnTopOfTexture = 0x5A57B0;
    constexpr DWORD ADDR_BlendTextures_DstSrc = 0x5A5820;
    constexpr DWORD ADDR_BlendTextures_DstSrc1Src2 = 0x5A59C0;
    constexpr DWORD ADDR_BlendTextures_DstSrc1Src2Tat = 0x5A5BC0;

    inline std::uint8_t WeightedByte(std::uint8_t a, float ra, std::uint8_t b, float rb)
    {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(a * ra + b * rb + 0.5f), 0, 255));
    }

    inline std::uint8_t WeightedByte(std::uint8_t a, float ra, std::uint8_t b, float rb, std::uint8_t c, float rc)
    {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(a * ra + b * rb + c * rc + 0.5f), 0, 255));
    }

    inline std::uint8_t LerpByte(std::uint8_t a, std::uint8_t b, float t)
    {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(a * (1.0f - t) + b * t + 0.5f), 0, 255));
    }

    //
    // Bilinear resample of a 32bpp (4 bytes/pixel) image. Channels are treated
    // independently so the byte order (BGRA/ARGB) is irrelevant. The clothes
    // rasters are power-of-two so a tightly packed width*4 stride matches what
    // the game itself assumes in its compositing loops.
    //
    void ResampleRGBA32(const std::uint32_t* pSrc, int srcWidth, int srcHeight, std::uint32_t* pDst, int dstWidth, int dstHeight)
    {
        if (srcWidth == dstWidth && srcHeight == dstHeight)
        {
            std::memcpy(pDst, pSrc, static_cast<size_t>(dstWidth) * dstHeight * 4);
            return;
        }

        for (int y = 0; y < dstHeight; ++y)
        {
            const float fy = (dstHeight > 1) ? static_cast<float>(y) * (srcHeight - 1) / (dstHeight - 1) : 0.0f;
            const int   y0 = static_cast<int>(fy);
            const int   y1 = std::min(y0 + 1, srcHeight - 1);
            const float wy = fy - y0;

            for (int x = 0; x < dstWidth; ++x)
            {
                const float fx = (dstWidth > 1) ? static_cast<float>(x) * (srcWidth - 1) / (dstWidth - 1) : 0.0f;
                const int   x0 = static_cast<int>(fx);
                const int   x1 = std::min(x0 + 1, srcWidth - 1);
                const float wx = fx - x0;

                const std::uint32_t c00 = pSrc[y0 * srcWidth + x0];
                const std::uint32_t c10 = pSrc[y0 * srcWidth + x1];
                const std::uint32_t c01 = pSrc[y1 * srcWidth + x0];
                const std::uint32_t c11 = pSrc[y1 * srcWidth + x1];

                std::uint32_t out = 0;
                for (int shift = 0; shift < 32; shift += 8)
                {
                    const float top = ((c00 >> shift) & 0xFF) * (1.0f - wx) + ((c10 >> shift) & 0xFF) * wx;
                    const float bot = ((c01 >> shift) & 0xFF) * (1.0f - wx) + ((c11 >> shift) & 0xFF) * wx;
                    int         v = static_cast<int>(top * (1.0f - wy) + bot * wy + 0.5f);
                    v = std::clamp(v, 0, 255);
                    out |= static_cast<std::uint32_t>(v) << shift;
                }
                pDst[y * dstWidth + x] = out;
            }
        }
    }

    //
    // Create a new raster (dstWidth x dstHeight) whose pixels are a resampled
    // copy of pSrcRaster. Mirrors the raster flags the game's own CopyTexture
    // uses: (format << 8) | rwRASTERTYPETEXTURE. Returns nullptr on failure.
    //
    RwRaster* CreateResampledRaster(RwRaster* pSrcRaster, int dstWidth, int dstHeight)
    {
        if (!pSrcRaster)
            return nullptr;

        const int flags = (static_cast<int>(pSrcRaster->format) << 8) | RW_RASTER_TYPE_TEXTURE;
        RwRaster* pNewRaster = RwRasterCreate(dstWidth, dstHeight, pSrcRaster->depth, flags);
        if (!pNewRaster)
            return nullptr;

        auto* pSrcBits = reinterpret_cast<std::uint32_t*>(RwRasterLock(pSrcRaster, 0, RW_RASTER_LOCK_READ));
        auto* pDstBits = reinterpret_cast<std::uint32_t*>(RwRasterLock(pNewRaster, 0, RW_RASTER_LOCK_WRITE));

        if (pSrcBits && pDstBits)
            ResampleRGBA32(pSrcBits, pSrcRaster->width, pSrcRaster->height, pDstBits, dstWidth, dstHeight);

        if (pDstBits)
            RwRasterUnlock(pNewRaster);
        if (pSrcBits)
            RwRasterUnlock(pSrcRaster);

        return pNewRaster;
    }

    //
    // Grow/shrink an owned accumulator texture's raster in place to the given
    // size. The old raster is destroyed. Returns false on failure (texture
    // left untouched).
    //
    bool ResizeAccumulatorInPlace(RwTexture* pTexture, int width, int height)
    {
        RwRaster* pOldRaster = pTexture->raster;
        if (!pOldRaster)
            return false;
        if (pOldRaster->width == width && pOldRaster->height == height)
            return true;

        RwRaster* pNewRaster = CreateResampledRaster(pOldRaster, width, height);
        if (!pNewRaster)
            return false;

        pTexture->raster = pNewRaster;
        RwRasterDestroy(pOldRaster);
        return true;
    }

    //
    // Return a texture whose raster is exactly width x height. If pSrcTexture
    // already matches, it is returned as-is (*pbIsTemporary = false). Otherwise
    // a temporary resampled texture is created (*pbIsTemporary = true) which the
    // caller must RwTextureDestroy. Shared/streamed source textures must never
    // be mutated, hence the copy.
    //
    RwTexture* GetSourceTextureAtSize(RwTexture* pSrcTexture, int width, int height, bool* pbIsTemporary)
    {
        *pbIsTemporary = false;
        RwRaster* pSrcRaster = pSrcTexture->raster;
        if (!pSrcRaster)
            return pSrcTexture;

        // A non-32bpp source (e.g. a DXT-compressed replacement) must be decompressed to 32bpp
        // before the CPU pixel compositing can use it. CreateResizedTexture decompresses and
        // resizes to A8R8G8B8 in one step.
        if (pSrcRaster->depth != 32)
        {
            SString    strError;
            RwTexture* pConverted = pGame->GetRenderWareSA()->CreateResizedTexture(pSrcTexture, width, height, strError, D3DFMT_A8R8G8B8, true);
            if (!pConverted)
                return pSrcTexture;
            *pbIsTemporary = true;
            return pConverted;
        }

        // Already 32bpp and the right size: use as-is
        if (pSrcRaster->width == width && pSrcRaster->height == height)
            return pSrcTexture;

        // 32bpp but a different size: CPU resample
        RwRaster* pNewRaster = CreateResampledRaster(pSrcRaster, width, height);
        if (!pNewRaster)
            return pSrcTexture;

        RwTexture* pTempTexture = RwTextureCreate(pNewRaster);
        if (!pTempTexture)
        {
            RwRasterDestroy(pNewRaster);
            return pSrcTexture;
        }

        *pbIsTemporary = true;
        return pTempTexture;
    }

    inline int CapDimension(int value)
    {
        return std::min(value, MAX_DIMENSIONS);
    }

    // The clothes compositing is CPU pixel math, so it only works on uncompressed 32bpp rasters.
    // Non-32bpp sources are decompressed/expanded to 32bpp on the fly; this is a last-resort guard
    // so a raster that still isn't 32bpp (a conversion failure) is never read as 32bpp here, which
    // would read past the (smaller) buffer and crash.
    inline bool IsRaster32bpp(RwRaster* pRaster)
    {
        return pRaster && pRaster->depth == 32;
    }
}  // namespace

//
// Replacement for CClothesBuilder::CopyTexture (0x5A5730).
// Clones a texture as a fresh compositing base. Caps the clone at MAX_DIMENSIONS and decompresses
// non-32bpp (e.g. DXT) sources to 32bpp. Also folds in the NULL-check that multiplayer_sa used to
// provide as a separate hook at this address.
//
static RwTexture* __cdecl HOOK_CClothesBuilder_CopyTexture(RwTexture* pSrcTexture)
{
    if (!pSrcTexture || !pSrcTexture->raster)
    {
        AddReportLog(9420, "ClothesTextures: CopyTexture called with NULL texture/raster");
        return nullptr;
    }

    RwRaster* pSrcRaster = pSrcTexture->raster;
    const int width = CapDimension(pSrcRaster->width);
    const int height = CapDimension(pSrcRaster->height);

    // Non-32bpp (e.g. DXT) source: decompress to a 32bpp A8R8G8B8 base
    if (pSrcRaster->depth != 32)
    {
        SString    strError;
        RwTexture* pConverted = pGame->GetRenderWareSA()->CreateResizedTexture(pSrcTexture, width, height, strError, D3DFMT_A8R8G8B8, true);
        if (!pConverted)
        {
            AddReportLog(9421, SString("ClothesTextures: CopyTexture could not decompress a non-32bpp raster (depth %d): %s", pSrcRaster->depth, *strError));
            return nullptr;
        }
        pConverted->flags = (pConverted->flags & 0xFFFFFF00) | RW_FILTER_LINEAR;
        return pConverted;
    }

    RwRaster* pNewRaster = CreateResampledRaster(pSrcRaster, width, height);
    if (!pNewRaster)
        return nullptr;

    RwTexture* pNewTexture = RwTextureCreate(pNewRaster);
    if (!pNewTexture)
    {
        RwRasterDestroy(pNewRaster);
        return nullptr;
    }

    // Match the original: set filter mode to linear (byte at RwTexture+0x50)
    pNewTexture->flags = (pNewTexture->flags & 0xFFFFFF00) | RW_FILTER_LINEAR;
    return pNewTexture;
}

//
// Replacement for CClothesBuilder::PlaceTextureOnTopOfTexture (0x5A57B0).
// Overlays the (alpha != 0) pixels of srcTex onto dstTex. Equalizes both
// textures to a common size first (growing the owned dst accumulator and
// resampling the shared src into a temporary), eliminating the original's
// fixed-size assumption that caused the heap overflow.
//
static void __cdecl HOOK_CClothesBuilder_PlaceTextureOnTopOfTexture(RwTexture* pDstTexture, RwTexture* pSrcTexture)
{
    if (!pDstTexture || !pSrcTexture || !pDstTexture->raster || !pSrcTexture->raster)
        return;

    const int width = CapDimension(std::max(pDstTexture->raster->width, pSrcTexture->raster->width));
    const int height = CapDimension(std::max(pDstTexture->raster->height, pSrcTexture->raster->height));

    ResizeAccumulatorInPlace(pDstTexture, width, height);

    bool       bSrcTemporary = false;
    RwTexture* pSrc = GetSourceTextureAtSize(pSrcTexture, width, height, &bSrcTemporary);

    RwRaster* pDstRaster = pDstTexture->raster;
    RwRaster* pSrcRaster = pSrc->raster;

    // Safety net: both must be 32bpp now (sources are decompressed by GetSourceTextureAtSize). Skip
    // if a conversion failed rather than read a smaller buffer as 32bpp.
    if (!IsRaster32bpp(pDstRaster) || !IsRaster32bpp(pSrcRaster))
    {
        if (bSrcTemporary)
            RwTextureDestroy(pSrc);
        return;
    }

    // Defensive: never iterate past the smaller of the two buffers
    const int pixelCount = std::min(pDstRaster->width * pDstRaster->height, pSrcRaster->width * pSrcRaster->height);

    auto* pDstBits = reinterpret_cast<std::uint32_t*>(RwRasterLock(pDstRaster, 0, RW_RASTER_LOCK_READWRITE));
    auto* pSrcBits = reinterpret_cast<std::uint32_t*>(RwRasterLock(pSrcRaster, 0, RW_RASTER_LOCK_READWRITE));

    if (pDstBits && pSrcBits)
    {
        for (int i = 0; i < pixelCount; ++i)
        {
            if (pSrcBits[i] & 0xFF000000)
                pDstBits[i] = pSrcBits[i];
        }
    }

    if (pSrcBits)
        RwRasterUnlock(pSrcRaster);
    if (pDstBits)
        RwRasterUnlock(pDstRaster);

    if (bSrcTemporary)
        RwTextureDestroy(pSrc);
}

//
// Replacement for CClothesBuilder::BlendTextures (0x5A5820) - "Dst-Src".
// Per RGB channel: dst = dst*r1 + src*r2 (alpha preserved). Used e.g. for the
// face fatness blend. Equalizes sizes so a replaced face texture can be HD.
//
static void __cdecl HOOK_CClothesBuilder_BlendTextures_DstSrc(RwTexture* pDstTexture, RwTexture* pSrcTexture, float r1, float r2, int /*numColors*/)
{
    if (!pDstTexture || !pSrcTexture || !pDstTexture->raster || !pSrcTexture->raster)
        return;

    const int width = CapDimension(std::max(pDstTexture->raster->width, pSrcTexture->raster->width));
    const int height = CapDimension(std::max(pDstTexture->raster->height, pSrcTexture->raster->height));

    ResizeAccumulatorInPlace(pDstTexture, width, height);
    bool       bSrcTemp = false;
    RwTexture* pSrc = GetSourceTextureAtSize(pSrcTexture, width, height, &bSrcTemp);

    RwRaster* pDstRaster = pDstTexture->raster;
    RwRaster* pSrcRaster = pSrc->raster;

    if (!IsRaster32bpp(pDstRaster) || !IsRaster32bpp(pSrcRaster))
    {
        if (bSrcTemp)
            RwTextureDestroy(pSrc);
        return;
    }

    const int pixelCount = std::min(pDstRaster->width * pDstRaster->height, pSrcRaster->width * pSrcRaster->height);

    auto* pDst = reinterpret_cast<std::uint8_t*>(RwRasterLock(pDstRaster, 0, RW_RASTER_LOCK_READWRITE));
    auto* pSrcBits = reinterpret_cast<std::uint8_t*>(RwRasterLock(pSrcRaster, 0, RW_RASTER_LOCK_READ));

    if (pDst && pSrcBits)
    {
        for (int i = 0; i < pixelCount; ++i)
        {
            std::uint8_t* d = pDst + i * 4;
            std::uint8_t* s = pSrcBits + i * 4;
            d[0] = WeightedByte(d[0], r1, s[0], r2);
            d[1] = WeightedByte(d[1], r1, s[1], r2);
            d[2] = WeightedByte(d[2], r1, s[2], r2);
            // alpha (d[3]) unchanged
        }
    }

    if (pSrcBits)
        RwRasterUnlock(pSrcRaster);
    if (pDst)
        RwRasterUnlock(pDstRaster);
    if (bSrcTemp)
        RwTextureDestroy(pSrc);
}

//
// Replacement for CClothesBuilder::BlendTextures (0x5A59C0) - "Dst-Src1-Src2".
// Per RGB channel: dst = dst*r1 + src1*r2 + src2*r3 (alpha preserved). This is
// the body shape (normal/fat/muscle) blend, used when no clothing overlay is
// present for the part.
//
static void __cdecl HOOK_CClothesBuilder_BlendTextures_DstSrc1Src2(RwTexture* pDstTexture, RwTexture* pSrc1Texture, RwTexture* pSrc2Texture, float r1, float r2,
                                                                   float r3, int /*numColors*/)
{
    if (!pDstTexture || !pSrc1Texture || !pSrc2Texture || !pDstTexture->raster || !pSrc1Texture->raster || !pSrc2Texture->raster)
        return;

    int width = std::max({pDstTexture->raster->width, pSrc1Texture->raster->width, pSrc2Texture->raster->width});
    int height = std::max({pDstTexture->raster->height, pSrc1Texture->raster->height, pSrc2Texture->raster->height});
    width = CapDimension(width);
    height = CapDimension(height);

    ResizeAccumulatorInPlace(pDstTexture, width, height);
    bool       bSrc1Temp = false, bSrc2Temp = false;
    RwTexture* pSrc1 = GetSourceTextureAtSize(pSrc1Texture, width, height, &bSrc1Temp);
    RwTexture* pSrc2 = GetSourceTextureAtSize(pSrc2Texture, width, height, &bSrc2Temp);

    RwRaster* pDstRaster = pDstTexture->raster;
    RwRaster* pSrc1Raster = pSrc1->raster;
    RwRaster* pSrc2Raster = pSrc2->raster;

    if (!IsRaster32bpp(pDstRaster) || !IsRaster32bpp(pSrc1Raster) || !IsRaster32bpp(pSrc2Raster))
    {
        if (bSrc2Temp)
            RwTextureDestroy(pSrc2);
        if (bSrc1Temp)
            RwTextureDestroy(pSrc1);
        return;
    }

    const int pixelCount =
        std::min({pDstRaster->width * pDstRaster->height, pSrc1Raster->width * pSrc1Raster->height, pSrc2Raster->width * pSrc2Raster->height});

    auto* pDst = reinterpret_cast<std::uint8_t*>(RwRasterLock(pDstRaster, 0, RW_RASTER_LOCK_READWRITE));
    auto* pS1 = reinterpret_cast<std::uint8_t*>(RwRasterLock(pSrc1Raster, 0, RW_RASTER_LOCK_READ));
    auto* pS2 = reinterpret_cast<std::uint8_t*>(RwRasterLock(pSrc2Raster, 0, RW_RASTER_LOCK_READ));

    if (pDst && pS1 && pS2)
    {
        for (int i = 0; i < pixelCount; ++i)
        {
            std::uint8_t* d = pDst + i * 4;
            std::uint8_t* a = pS1 + i * 4;
            std::uint8_t* b = pS2 + i * 4;
            d[0] = WeightedByte(d[0], r1, a[0], r2, b[0], r3);
            d[1] = WeightedByte(d[1], r1, a[1], r2, b[1], r3);
            d[2] = WeightedByte(d[2], r1, a[2], r2, b[2], r3);
            // alpha (d[3]) unchanged
        }
    }

    if (pS2)
        RwRasterUnlock(pSrc2Raster);
    if (pS1)
        RwRasterUnlock(pSrc1Raster);
    if (pDst)
        RwRasterUnlock(pDstRaster);
    if (bSrc2Temp)
        RwTextureDestroy(pSrc2);
    if (bSrc1Temp)
        RwTextureDestroy(pSrc1);
}

//
// Replacement for CClothesBuilder::BlendTextures (0x5A5BC0) - "Dst-Src1-Src2-Tat".
// Body shape blend (dst*r1 + src1*r2 + src2*r3) then alpha-composite the
// "tattoo" layer on top using the tattoo's own per-pixel alpha. For clothing
// this "tattoo" layer is the accumulated clothing/design overlay, so this is the
// key path for an HD shirt to reach the final torso texture.
//
static void __cdecl HOOK_CClothesBuilder_BlendTextures_DstSrc1Src2Tat(RwTexture* pDstTexture, RwTexture* pSrc1Texture, RwTexture* pSrc2Texture, float r1,
                                                                      float r2, float r3, int /*numColors*/, RwTexture* pTatTexture)
{
    if (!pDstTexture || !pSrc1Texture || !pSrc2Texture || !pTatTexture || !pDstTexture->raster || !pSrc1Texture->raster || !pSrc2Texture->raster ||
        !pTatTexture->raster)
        return;

    int width = std::max({pDstTexture->raster->width, pSrc1Texture->raster->width, pSrc2Texture->raster->width, pTatTexture->raster->width});
    int height = std::max({pDstTexture->raster->height, pSrc1Texture->raster->height, pSrc2Texture->raster->height, pTatTexture->raster->height});
    width = CapDimension(width);
    height = CapDimension(height);

    ResizeAccumulatorInPlace(pDstTexture, width, height);
    bool       bSrc1Temp = false, bSrc2Temp = false, bTatTemp = false;
    RwTexture* pSrc1 = GetSourceTextureAtSize(pSrc1Texture, width, height, &bSrc1Temp);
    RwTexture* pSrc2 = GetSourceTextureAtSize(pSrc2Texture, width, height, &bSrc2Temp);
    RwTexture* pTat = GetSourceTextureAtSize(pTatTexture, width, height, &bTatTemp);

    RwRaster* pDstRaster = pDstTexture->raster;
    RwRaster* pSrc1Raster = pSrc1->raster;
    RwRaster* pSrc2Raster = pSrc2->raster;
    RwRaster* pTatRaster = pTat->raster;

    if (!IsRaster32bpp(pDstRaster) || !IsRaster32bpp(pSrc1Raster) || !IsRaster32bpp(pSrc2Raster) || !IsRaster32bpp(pTatRaster))
    {
        if (bTatTemp)
            RwTextureDestroy(pTat);
        if (bSrc2Temp)
            RwTextureDestroy(pSrc2);
        if (bSrc1Temp)
            RwTextureDestroy(pSrc1);
        return;
    }

    const int pixelCount = std::min({pDstRaster->width * pDstRaster->height, pSrc1Raster->width * pSrc1Raster->height, pSrc2Raster->width * pSrc2Raster->height,
                                     pTatRaster->width * pTatRaster->height});

    auto* pDst = reinterpret_cast<std::uint8_t*>(RwRasterLock(pDstRaster, 0, RW_RASTER_LOCK_READWRITE));
    auto* pS1 = reinterpret_cast<std::uint8_t*>(RwRasterLock(pSrc1Raster, 0, RW_RASTER_LOCK_READ));
    auto* pS2 = reinterpret_cast<std::uint8_t*>(RwRasterLock(pSrc2Raster, 0, RW_RASTER_LOCK_READ));
    auto* pT = reinterpret_cast<std::uint8_t*>(RwRasterLock(pTatRaster, 0, RW_RASTER_LOCK_READ));

    if (pDst && pS1 && pS2 && pT)
    {
        for (int i = 0; i < pixelCount; ++i)
        {
            std::uint8_t* d = pDst + i * 4;
            std::uint8_t* a = pS1 + i * 4;
            std::uint8_t* b = pS2 + i * 4;
            std::uint8_t* t = pT + i * 4;
            const float   tatAlpha = t[3] / 255.0f;
            for (int c = 0; c < 3; ++c)
            {
                const std::uint8_t body = WeightedByte(d[c], r1, a[c], r2, b[c], r3);
                d[c] = LerpByte(body, t[c], tatAlpha);
            }
            // alpha (d[3]) unchanged
        }
    }

    if (pT)
        RwRasterUnlock(pTatRaster);
    if (pS2)
        RwRasterUnlock(pSrc2Raster);
    if (pS1)
        RwRasterUnlock(pSrc1Raster);
    if (pDst)
        RwRasterUnlock(pDstRaster);
    if (bTatTemp)
        RwTextureDestroy(pTat);
    if (bSrc2Temp)
        RwTextureDestroy(pSrc2);
    if (bSrc1Temp)
        RwTextureDestroy(pSrc1);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Install the clothes texture compositing hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetClothesTexturesHooks()
{
    HookInstall(ADDR_CopyTexture, (DWORD)HOOK_CClothesBuilder_CopyTexture, 5);
    HookInstall(ADDR_PlaceTextureOnTopOfTexture, (DWORD)HOOK_CClothesBuilder_PlaceTextureOnTopOfTexture, 5);
    HookInstall(ADDR_BlendTextures_DstSrc, (DWORD)HOOK_CClothesBuilder_BlendTextures_DstSrc, 5);
    HookInstall(ADDR_BlendTextures_DstSrc1Src2, (DWORD)HOOK_CClothesBuilder_BlendTextures_DstSrc1Src2, 5);
    HookInstall(ADDR_BlendTextures_DstSrc1Src2Tat, (DWORD)HOOK_CClothesBuilder_BlendTextures_DstSrc1Src2Tat, 5);
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.PoolMemorySaver.cpp
 *  PURPOSE:     Convert engineLoadTXD replacement textures from D3DPOOL_MANAGED
 *               to D3DPOOL_DEFAULT to drop the system-memory shadow copy
 *               (issue #4062). Handles device-lost/reset for those textures
 *               since DEFAULT-pool resources do not auto-restore. Covers both
 *               2D and cube-map rasters.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include <core/CGraphicsInterface.h>
#include <game/RenderWareD3D.h>
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "gamesa_renderware.h"

extern CCoreInterface* g_pCore;

namespace
{
    // GTA's RW raster type tag for a regular 2D texture raster (RwRaster::type field).
    // 0 = normal, 1 = z-buffer, 2 = camera, 4 = texture, 5 = camera-texture.
    // Replacement TXDs always produce type=4 entries (cube maps included; the
    // distinction is in RwD3D9Raster::cubeTextureFlags, not RwRaster::type).
    constexpr unsigned char kRwRasterTypeTexture = 4;

    // RwRaster::flags bit 0x80 is rwRASTERDONTALLOCATE; if set the raster has no
    // backing D3D resource yet and must be skipped.
    constexpr unsigned char kRwRasterFlagDontAllocate = 0x80;

    // RwD3D9Raster::cubeTextureFlags low bit marks the raster as a cube map.
    // (The IDA struct calls this field cube_face; high nibble tracks the current
    // face during stream-read iteration but is reset to 0 when decoding finishes.)
    constexpr unsigned char kRwD3DRasterCubeMap = 0x01;

    // RwD3D9Raster::textureFlags low nibble holds the D3DUSAGE_AUTOGENMIPMAP flag.
    // Such textures are created with USAGE_AUTOGENMIPMAP, which is incompatible
    // with D3DPOOL_SYSTEMMEM (per d3d9types.h table) so our SYSTEMMEM-staging
    // path can't be used; we leave them MANAGED. They are uncommon in user TXDs.
    constexpr unsigned char kRwD3DRasterAutoMipGenMask = 0x0F;

    // Number of bytes in one row of a compressed-block surface, given the
    // surface width in pixels and the per-block size.
    UINT CompressedRowBytes(UINT widthPixels, UINT blockBytes)
    {
        const UINT widthBlocks = (widthPixels + 3u) / 4u;
        return widthBlocks * blockBytes;
    }

    // Number of compressed-block rows for a surface height in pixels.
    UINT CompressedRowCount(UINT heightPixels)
    {
        return (heightPixels + 3u) / 4u;
    }

    // Returns true and fills outPerRow / outRowCount with the actual data span
    // on each mip level for the given format. Pitch reported by LockRect can be
    // larger than this on some drivers; we always copy outPerRow bytes per row
    // and step by the LockRect-reported pitch.
    bool GetCopyParams(D3DFORMAT format, UINT widthPixels, UINT heightPixels, UINT& outPerRow, UINT& outRowCount)
    {
        switch (format)
        {
            case D3DFMT_DXT1:
                outPerRow = CompressedRowBytes(widthPixels, 8);
                outRowCount = CompressedRowCount(heightPixels);
                return true;
            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
            case D3DFMT_DXT4:
            case D3DFMT_DXT5:
                outPerRow = CompressedRowBytes(widthPixels, 16);
                outRowCount = CompressedRowCount(heightPixels);
                return true;
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
            case D3DFMT_A8B8G8R8:
            case D3DFMT_X8B8G8R8:
                outPerRow = widthPixels * 4u;
                outRowCount = heightPixels;
                return true;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:
            case D3DFMT_A1R5G5B5:
            case D3DFMT_A4R4G4B4:
            case D3DFMT_X4R4G4B4:
                outPerRow = widthPixels * 2u;
                outRowCount = heightPixels;
                return true;
            case D3DFMT_R8G8B8:
                outPerRow = widthPixels * 3u;
                outRowCount = heightPixels;
                return true;
            case D3DFMT_L8:
            case D3DFMT_A8:
                outPerRow = widthPixels;
                outRowCount = heightPixels;
                return true;
            case D3DFMT_A8L8:
                outPerRow = widthPixels * 2u;
                outRowCount = heightPixels;
                return true;
            default:
                // Unknown format: refuse to convert. The caller will leave the
                // texture in MANAGED, sacrificing memory savings for safety.
                return false;
        }
    }

    // Inner memcpy used by both 2D and cube paths. Locks are already held by
    // the caller; we just copy bytes between two locked rects.
    void CopyLockedRect(const D3DLOCKED_RECT& srcRect, const D3DLOCKED_RECT& dstRect, UINT perRow, UINT rowCount)
    {
        const std::uint8_t* pSrcBytes = static_cast<const std::uint8_t*>(srcRect.pBits);
        std::uint8_t*       pDstBytes = static_cast<std::uint8_t*>(dstRect.pBits);
        const UINT          srcPitch = static_cast<UINT>(srcRect.Pitch);
        const UINT          dstPitch = static_cast<UINT>(dstRect.Pitch);
        // min() guards against drivers reporting a smaller pitch than the data
        // we computed (shouldn't happen, but cheap to clamp).
        const UINT copyBytesPerRow = std::min(perRow, std::min(srcPitch, dstPitch));

        for (UINT row = 0; row < rowCount; ++row)
        {
            std::memcpy(pDstBytes + row * dstPitch, pSrcBytes + row * srcPitch, copyBytesPerRow);
        }
    }

    // Copy every mip level of a 2D pSrc (any pool) into a 2D pDst (must be
    // SYSTEMMEM). Returns true on success; on failure the caller releases both
    // textures and aborts the conversion.
    bool CopyAllMipsToSystemMem(IDirect3DTexture9* pSrc, IDirect3DTexture9* pDst)
    {
        const UINT levelCount = pSrc->GetLevelCount();
        if (levelCount == 0 || pDst->GetLevelCount() != levelCount)
            return false;

        for (UINT level = 0; level < levelCount; ++level)
        {
            D3DSURFACE_DESC desc{};
            if (FAILED(pSrc->GetLevelDesc(level, &desc)))
                return false;

            UINT perRow = 0;
            UINT rowCount = 0;
            if (!GetCopyParams(desc.Format, desc.Width, desc.Height, perRow, rowCount))
                return false;

            D3DLOCKED_RECT srcRect{};
            D3DLOCKED_RECT dstRect{};

            // READONLY + NO_DIRTY_UPDATE keeps the source clean (we are not
            // mutating it; we just want its bytes). NOSYSLOCK lets us run
            // outside the global D3D9 lock when the driver allows it.
            if (FAILED(pSrc->LockRect(level, &srcRect, nullptr, D3DLOCK_READONLY | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK)))
                return false;

            if (FAILED(pDst->LockRect(level, &dstRect, nullptr, 0)))
            {
                pSrc->UnlockRect(level);
                return false;
            }

            CopyLockedRect(srcRect, dstRect, perRow, rowCount);

            pDst->UnlockRect(level);
            pSrc->UnlockRect(level);
        }

        return true;
    }

    // Copy every mip level of all 6 faces of a cube pSrc (any pool) into pDst
    // (must be SYSTEMMEM cube). All 6 faces of a cube share the same mip
    // dimensions and format so we re-derive perRow / rowCount per level only,
    // not per face.
    bool CopyAllCubeMipsToSystemMem(IDirect3DCubeTexture9* pSrc, IDirect3DCubeTexture9* pDst)
    {
        const UINT levelCount = pSrc->GetLevelCount();
        if (levelCount == 0 || pDst->GetLevelCount() != levelCount)
            return false;

        for (UINT level = 0; level < levelCount; ++level)
        {
            D3DSURFACE_DESC desc{};
            if (FAILED(pSrc->GetLevelDesc(level, &desc)))
                return false;

            UINT perRow = 0;
            UINT rowCount = 0;
            if (!GetCopyParams(desc.Format, desc.Width, desc.Height, perRow, rowCount))
                return false;

            for (UINT face = 0; face < 6; ++face)
            {
                const D3DCUBEMAP_FACES faceEnum = static_cast<D3DCUBEMAP_FACES>(face);

                D3DLOCKED_RECT srcRect{};
                D3DLOCKED_RECT dstRect{};

                if (FAILED(pSrc->LockRect(faceEnum, level, &srcRect, nullptr, D3DLOCK_READONLY | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK)))
                    return false;

                if (FAILED(pDst->LockRect(faceEnum, level, &dstRect, nullptr, 0)))
                {
                    pSrc->UnlockRect(faceEnum, level);
                    return false;
                }

                CopyLockedRect(srcRect, dstRect, perRow, rowCount);

                pDst->UnlockRect(faceEnum, level);
                pSrc->UnlockRect(faceEnum, level);
            }
        }

        return true;
    }

    // After the OLD MANAGED texture is released and the NEW DEFAULT one is in
    // place, scrub the cached lock-state fields on the rasterExt so any future
    // code that consults them doesn't see a dangling pointer to a freed surface.
    // Nothing on the replacement-texture path reads these today (RightSizeTexture
    // only operates on freshly-decoded MANAGED textures, never on engineLoadTXD
    // outputs), but leaving stale pointers behind is a cheap-to-eliminate footgun.
    void ScrubLockStateAndSwapTexture(RwD3D9Raster* pD3DRaster, IDirect3DTexture9* pNewTexture)
    {
        pD3DRaster->lockedLevel = 0;
        pD3DRaster->lockedSurface = nullptr;
        pD3DRaster->lockedRect = D3DLOCKED_RECT{0, nullptr};
        pD3DRaster->texture = pNewTexture;
    }

    // CreateTexture wrapper that mirrors CDirect3DEvents9::CreateTexture's
    // retry-on-OOM behaviour without the proxy wrapping. Drops MANAGED resources
    // from vram once on D3DERR_OUTOFVIDEOMEMORY before retrying; further failures
    // propagate up.
    HRESULT CreateTextureWithRetry(IDirect3DDevice9* pDevice, UINT width, UINT height, UINT levels, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9** ppOut)
    {
        HRESULT hr = pDevice->CreateTexture(width, height, levels, 0, format, pool, ppOut, nullptr);
        if (hr != D3DERR_OUTOFVIDEOMEMORY)
            return hr;
        pDevice->EvictManagedResources();
        return pDevice->CreateTexture(width, height, levels, 0, format, pool, ppOut, nullptr);
    }

    HRESULT CreateCubeTextureWithRetry(IDirect3DDevice9* pDevice, UINT edgeLength, UINT levels, D3DFORMAT format, D3DPOOL pool, IDirect3DCubeTexture9** ppOut)
    {
        HRESULT hr = pDevice->CreateCubeTexture(edgeLength, levels, 0, format, pool, ppOut, nullptr);
        if (hr != D3DERR_OUTOFVIDEOMEMORY)
            return hr;
        pDevice->EvictManagedResources();
        return pDevice->CreateCubeTexture(edgeLength, levels, 0, format, pool, ppOut, nullptr);
    }

    // Single 2D raster MANAGED -> DEFAULT conversion.
    //
    // Strategy:
    //   1. Allocate the new D3DPOOL_DEFAULT texture (no system memory shadow).
    //   2. Allocate a transient D3DPOOL_SYSTEMMEM scratch with the same layout.
    //   3. Lock-and-memcpy each mip from MANAGED into SYSTEMMEM. The SYSTEMMEM
    //      lock is taken with flags=0 so D3D records dirty regions; UpdateTexture
    //      below uses those to drive the staging-to-DEFAULT copy.
    //   4. UpdateTexture to push SYSTEMMEM into DEFAULT (canonical D3D9 path:
    //      "the source must be in system memory, the destination in DEFAULT").
    //   5. Release SYSTEMMEM, release MANAGED, swap in DEFAULT.
    //
    // We deliberately bypass D3DResourceSystem::DestroyTexture (gta_sa.exe @
    // 0x730510 / 0x730B70) when releasing the original MANAGED texture: that
    // function caches small (<= 256x256 square) textures in gD3DTextureBuffer
    // for reuse, and the cache assumes everything in it is MANAGED.
    bool Convert2DRaster(IDirect3DDevice9* pDevice, RwD3D9Raster* pD3DRaster, const D3DSURFACE_DESC& desc0, UINT levelCount)
    {
        // 1. Create the destination DEFAULT-pool texture (with one retry on
        //    out-of-video-memory after evicting MANAGED resources).
        IDirect3DTexture9* pDefault = nullptr;
        if (FAILED(CreateTextureWithRetry(pDevice, desc0.Width, desc0.Height, levelCount, desc0.Format, D3DPOOL_DEFAULT, &pDefault)) || !pDefault)
            return false;

        // 2. Create a transient SYSTEMMEM scratch.
        IDirect3DTexture9* pScratch = nullptr;
        if (FAILED(pDevice->CreateTexture(desc0.Width, desc0.Height, levelCount, 0, desc0.Format, D3DPOOL_SYSTEMMEM, &pScratch, nullptr)) || !pScratch)
        {
            pDefault->Release();
            return false;
        }

        // 3. Copy MANAGED -> SYSTEMMEM mip by mip.
        IDirect3DTexture9* pManaged = pD3DRaster->texture;
        if (!CopyAllMipsToSystemMem(pManaged, pScratch))
        {
            pScratch->Release();
            pDefault->Release();
            return false;
        }

        // 4. Push SYSTEMMEM contents to DEFAULT.
        if (FAILED(pDevice->UpdateTexture(pScratch, pDefault)))
        {
            pScratch->Release();
            pDefault->Release();
            return false;
        }

        // 5. Drop originals; rasterExt now owns the DEFAULT texture.
        pScratch->Release();
        pManaged->Release();
        ScrubLockStateAndSwapTexture(pD3DRaster, pDefault);
        return true;
    }

    // Cube-map variant of Convert2DRaster. Differs only in:
    //   - CreateCubeTexture (single EdgeLength param vs Width+Height)
    //   - 6 faces per mip level (looped inside CopyAllCubeMipsToSystemMem)
    //   - rasterExt->texture stores IDirect3DCubeTexture9* via the union; the
    //     reinterpret_cast is well-defined because RwD3D9Raster::texture is a
    //     union member that GTA already populates with cube pointers in
    //     _rwD3D9NativeTextureRead at 0x4CD982.
    //
    // We keep the same SYSTEMMEM-staging + UpdateTexture pattern. UpdateTexture
    // accepts cube-to-cube of identical type/format/level-count and copies all
    // 6 faces in a single device call.
    bool ConvertCubeRaster(IDirect3DDevice9* pDevice, RwD3D9Raster* pD3DRaster, const D3DSURFACE_DESC& desc0, UINT levelCount)
    {
        // For cubes EdgeLength = desc.Width (= desc.Height); D3D9 enforces
        // square faces in CreateCubeTexture, so this is a no-op assertion.
        if (desc0.Width != desc0.Height)
            return false;

        IDirect3DCubeTexture9* pManagedCube = reinterpret_cast<IDirect3DCubeTexture9*>(pD3DRaster->texture);

        IDirect3DCubeTexture9* pDefault = nullptr;
        if (FAILED(CreateCubeTextureWithRetry(pDevice, desc0.Width, levelCount, desc0.Format, D3DPOOL_DEFAULT, &pDefault)) || !pDefault)
            return false;

        IDirect3DCubeTexture9* pScratch = nullptr;
        if (FAILED(pDevice->CreateCubeTexture(desc0.Width, levelCount, 0, desc0.Format, D3DPOOL_SYSTEMMEM, &pScratch, nullptr)) || !pScratch)
        {
            pDefault->Release();
            return false;
        }

        if (!CopyAllCubeMipsToSystemMem(pManagedCube, pScratch))
        {
            pScratch->Release();
            pDefault->Release();
            return false;
        }

        if (FAILED(pDevice->UpdateTexture(pScratch, pDefault)))
        {
            pScratch->Release();
            pDefault->Release();
            return false;
        }

        pScratch->Release();
        pManagedCube->Release();
        // The union slot is typed IDirect3DTexture9* but actually stores a cube
        // texture for cube rasters. GTA already does the same trick at
        // 0x4CD982 (CreateCubeTexture writes into &v8->texture).
        ScrubLockStateAndSwapTexture(pD3DRaster, reinterpret_cast<IDirect3DTexture9*>(pDefault));
        return true;
    }

    // Top-level conversion dispatcher.
    bool ConvertOneRaster(IDirect3DDevice9* pDevice, RwRaster* pRaster)
    {
        if (!pDevice || !pRaster)
            return false;
        if (pRaster->type != kRwRasterTypeTexture)
            return false;
        if ((pRaster->flags & kRwRasterFlagDontAllocate) != 0)
            return false;

        RwD3D9Raster* pD3DRaster = reinterpret_cast<RwD3D9Raster*>(&pRaster->renderResource);

        // Palettised rasters carry an extra 1024-byte indexed palette buffer plus
        // a D3DFMT_P8 texture; D3DPOOL_DEFAULT paletted textures are essentially
        // unsupported on modern hardware (most drivers refuse the format and the
        // ones that don't would still need explicit palette set-up post-Reset).
        // These are vanishingly rare in user-supplied TXDs; leave them MANAGED.
        if (pD3DRaster->palette != nullptr)
            return false;

        // D3DUSAGE_AUTOGENMIPMAP cannot coexist with D3DPOOL_SYSTEMMEM, which
        // breaks our staging path; leave such rasters MANAGED.
        if ((pD3DRaster->textureFlags & kRwD3DRasterAutoMipGenMask) != 0)
            return false;

        IDirect3DTexture9* pManaged = pD3DRaster->texture;
        if (!pManaged)
            return false;

        // GetLevelDesc on a cube texture (called through the IDirect3DTexture9
        // vtable) hits the cube vtable's GetLevelDesc at the same vtable slot
        // and returns the level/face desc with the same struct layout, so this
        // works for both 2D and cube rasters even before we know which it is.
        D3DSURFACE_DESC desc0{};
        if (FAILED(pManaged->GetLevelDesc(0, &desc0)))
            return false;

        // If GTA already gave us a non-MANAGED resource (e.g. driver fallback)
        // there's nothing to optimise; leave it alone.
        if (desc0.Pool != D3DPOOL_MANAGED)
            return false;

        const UINT levelCount = pManaged->GetLevelCount();
        if (levelCount == 0)
            return false;

        // Refuse formats we don't have an explicit byte-count rule for. Keeps
        // exotic FOURCC and proprietary formats on the original MANAGED path.
        UINT perRow = 0;
        UINT rowCount = 0;
        if (!GetCopyParams(desc0.Format, desc0.Width, desc0.Height, perRow, rowCount))
            return false;

        const bool bIsCube = (pD3DRaster->cubeTextureFlags & kRwD3DRasterCubeMap) != 0;
        if (bIsCube)
            return ConvertCubeRaster(pDevice, pD3DRaster, desc0, levelCount);
        return Convert2DRaster(pDevice, pD3DRaster, desc0, levelCount);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ConvertScriptTxdToDefaultPool
//
// Walk a freshly-loaded RwTexDictionary and convert each texture's raster from
// MANAGED to DEFAULT pool. Called from CRenderWareSA::ReadTXD after the GTA RW
// loader has filled the textures and BEFORE ScriptAddedTxd, so the texinfo
// shader-matching map is keyed against the new (DEFAULT) IDirect3DTexture9.
//
// Conversion is best-effort: any single texture that fails (out of vram,
// unknown format) silently stays MANAGED. The caller does not branch on a
// return value because there is no recovery to do beyond carrying on.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ConvertScriptTxdToDefaultPool(RwTexDictionary* pTxd)
{
    if (!pTxd || m_bDeviceLost)
        return;

    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    if (!pDevice)
        return;

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTxd);

    for (RwTexture* pTexture : textureList)
    {
        if (!pTexture || !pTexture->raster)
            continue;

        // Already converted? Can happen when a script re-loads the same
        // raster pointer; not expected in master but cheap to defend.
        if (m_DefaultPoolRasters.count(pTexture->raster) != 0)
            continue;

        if (ConvertOneRaster(pDevice, pTexture->raster))
            m_DefaultPoolRasters.insert(pTexture->raster);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ReleaseTrackedDefaultPoolTexture
//
// Called from DestroyTexture before RwTextureDestroy. If the raster is one we
// converted, release the IDirect3DTexture9 ourselves and NULL out
// rasterExt->texture so _rwD3D9RasterDestroy hits its early-out and bypasses
// D3DResourceSystem::DestroyTexture (the MANAGED-only cache).
//
// Returns true if the raster was tracked (so the caller knows we handled it).
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ReleaseTrackedDefaultPoolTexture(RwRaster* pRaster)
{
    if (!pRaster)
        return false;

    auto it = m_DefaultPoolRasters.find(pRaster);
    if (it == m_DefaultPoolRasters.end())
        return false;

    RwD3D9Raster* pD3DRaster = reinterpret_cast<RwD3D9Raster*>(&pRaster->renderResource);
    if (pD3DRaster->texture)
    {
        pD3DRaster->texture->Release();
        pD3DRaster->texture = nullptr;
    }

    m_DefaultPoolRasters.erase(it);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDeviceLost
//
// Called from CGraphics during a D3DERR_DEVICELOST cooperative-level loss,
// before IDirect3DDevice9::Reset is attempted. Per the D3D9 contract, every
// resource in D3DPOOL_DEFAULT must be released first. We release each
// converted replacement texture and NULL the raster's renderResource pointer
// so any pending render binds it as a null texture rather than a stale
// IDirect3DTexture9.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDeviceLost()
{
    if (m_bDeviceLost)
        return;
    m_bDeviceLost = true;

    for (RwRaster* pRaster : m_DefaultPoolRasters)
    {
        if (!pRaster)
            continue;
        RwD3D9Raster* pD3DRaster = reinterpret_cast<RwD3D9Raster*>(&pRaster->renderResource);
        if (pD3DRaster->texture)
        {
            pD3DRaster->texture->Release();
            pD3DRaster->texture = nullptr;
        }
    }
    // We deliberately keep the raster pointers in m_DefaultPoolRasters so the
    // destroy-intercept still recognises them if a CClientTXD is freed while
    // we're in the lost state (e.g. resource stops between Lost and Reset).
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDeviceReset
//
// Called from CGraphics after IDirect3DDevice9::Reset succeeds. Every
// registered owner is asked to re-decode its source bytes. We snapshot the
// owner list because RebuildReplacementsAfterDeviceReset re-enters
// ModelInfoTXDRemoveTextures + ModelInfoTXDLoadTextures, which mutate
// m_DefaultPoolRasters via the destroy intercept.
//
// File-path TXDs re-read from disk and recover transparently. Buffer-path
// TXDs use the m_FileData buffer kept since Load. If neither is available
// (extremely rare; would require the script to have manually freed the
// buffer mid-frame) the rebuild returns false and the affected models fall
// back to the original GTA textures until the script reloads.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDeviceReset()
{
    if (!m_bDeviceLost)
        return;
    m_bDeviceLost = false;

    const std::vector<CRwReplacementOwner*> ownersSnapshot = m_ReplacementOwners;
    for (CRwReplacementOwner* pOwner : ownersSnapshot)
    {
        if (!pOwner)
            continue;
        // Defensive: if an owner unregistered itself between snapshot and now
        // (e.g. its destructor ran during the iteration, which shouldn't be
        // possible in master because OnRestore is synchronous, but is cheap to
        // guard against) skip it rather than dereference a dangling pointer.
        if (std::find(m_ReplacementOwners.begin(), m_ReplacementOwners.end(), pOwner) == m_ReplacementOwners.end())
            continue;
        // Best-effort. False return means the owner could not rebuild; the
        // caller-side TXD has already removed itself from GTA's TXDs, so the
        // affected models will fall back to their original textures.
        pOwner->RebuildReplacementsAfterDeviceReset();
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RegisterReplacementOwner / UnregisterReplacementOwner
//
// CClientTXD subscribes itself in its ctor and unsubscribes in its dtor.
// We don't bother with a set: owner counts are small (one per loaded TXD)
// and unregister-on-destruct must not throw, which std::vector::erase
// guarantees.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RegisterReplacementOwner(CRwReplacementOwner* pOwner)
{
    if (!pOwner)
        return;
    if (std::find(m_ReplacementOwners.begin(), m_ReplacementOwners.end(), pOwner) != m_ReplacementOwners.end())
        return;
    m_ReplacementOwners.push_back(pOwner);
}

void CRenderWareSA::UnregisterReplacementOwner(CRwReplacementOwner* pOwner)
{
    if (!pOwner)
        return;
    auto it = std::find(m_ReplacementOwners.begin(), m_ReplacementOwners.end(), pOwner);
    if (it != m_ReplacementOwners.end())
        m_ReplacementOwners.erase(it);
}

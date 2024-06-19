/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.cpp
 *  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
 *               and miscellaneous rendering functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include <game/RenderWareD3D.h>
#include "CGameSA.h"
#include "gamesa_renderware.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

/////////////////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RightSizeTxd
//
// Check textures in a txd and shrink if required.
// Returns true if shrunk file was written
//
/////////////////////////////////////////////////////////////////////////////
bool CRenderWareSA::RightSizeTxd(const SString& strInTxdFilename, const SString& strOutTxdFilename, uint uiSizeLimit)
{
    //
    // Read txd from memory
    //
    RwStream* pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strInTxdFilename);
    if (pStream == NULL)
        return false;

    // Find our txd chunk
    if (RwStreamFindChunk(pStream, 0x16, NULL, NULL) == false)
    {
        RwStreamClose(pStream, NULL);
        return false;
    }

    // read the txd
    RwTexDictionary* pTxd = RwTexDictionaryGtaStreamRead(pStream);
    RwStreamClose(pStream, NULL);
    if (!pTxd)
        return false;

    //
    // Process each texture in the txd
    //
    std::vector<RwTexture*> textureList;
    pGame->GetRenderWareSA()->GetTxdTextures(textureList, pTxd);

    // Double size limit if only one texture in txd
    if (textureList.size() == 1)
        uiSizeLimit *= 2;

    SString strError;
    bool    bChanged = false;
    for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
    {
        RwTexture* pTexture = *iter;
        RwTexture* pNewRwTexture = RightSizeTexture(pTexture, uiSizeLimit, strError);
        if (pNewRwTexture && pNewRwTexture != pTexture)
        {
            // Replace texture in txd if changed
            RwTextureDestroy(pTexture);
            RwTexDictionaryAddTexture(pTxd, pNewRwTexture);
            bChanged = true;
        }
        else
        {
            // Keep texture (Reinsert to preserve order for easier debugging)
            RwTexDictionaryRemoveTexture(pTxd, pTexture);
            RwTexDictionaryAddTexture(pTxd, pTexture);
        }
    }

    // Log last error
    if (!strError.empty())
    {
        AddReportLog(8430, SString("RightSizeTxd problem %s with '%s'", *strError, *strInTxdFilename), 10);
    }

    //
    // Save shrunked txd if changed
    //
    if (bChanged)
    {
        pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_WRITE, *strOutTxdFilename);
        if (pStream)
        {
            RwTexDictionaryStreamWrite(pTxd, pStream);
            RwStreamClose(pStream, NULL);
            RwTexDictionaryDestroy(pTxd);
            return true;
        }
    }

    RwTexDictionaryDestroy(pTxd);
    return false;
}

/////////////////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RightSizeTexture
//
// Check texture and shrink if required.
// Returns new texture if did shrink
//
/////////////////////////////////////////////////////////////////////////////
RwTexture* CRenderWareSA::RightSizeTexture(RwTexture* pTexture, uint uiSizeLimit, SString& strError)
{
    // Validate
    RwRaster* pRaster = pTexture->raster;
    if (!pRaster)
    {
        strError = "pRaster == NULL";
        return NULL;
    }

    RwD3D9Raster* pD3DRaster = (RwD3D9Raster*)(&pRaster->renderResource);
    if (!pD3DRaster->texture)
    {
        strError = "pD3DRaster->texture == NULL";
        return NULL;
    }
    if (!pD3DRaster->lockedSurface)
    {
        strError = "pD3DRaster->lockedSurface == NULL";
        return NULL;
    }
    if (!pD3DRaster->lockedRect.pBits)
    {
        strError = "pD3DRaster->lockedRect.pBits == NULL";
        return NULL;
    }

    // Get texture info
    uint      uiWidth = pRaster->width;
    uint      uiHeight = pRaster->height;
    D3DFORMAT d3dFormat = pD3DRaster->format;
    bool      bHasAlpha = pD3DRaster->alpha != 0;
    bool      bIsCubeTexture = (pD3DRaster->cubeTextureFlags & 0x01) != 0;
    bool      bHasMipMaps = (pRaster->numLevels > 1);
    bool      bIsCompressed = (pD3DRaster->textureFlags & 0x10) != 0;

    // Check we can do this
    if (bIsCubeTexture || !bIsCompressed)
        return NULL;

    // Only process DXT formats
    if (d3dFormat != D3DFMT_DXT1 && d3dFormat != D3DFMT_DXT3 && d3dFormat != D3DFMT_DXT5)
        return NULL;

    // Double size limit if DXT1
    if (d3dFormat == D3DFMT_DXT1)
        uiSizeLimit *= 2;

    // Change size
    uint uiReqWidth = std::min(uiSizeLimit, uiWidth);
    uint uiReqHeight = std::min(uiSizeLimit, uiHeight);
    if (uiReqWidth == uiWidth && uiReqHeight == uiHeight)
        return NULL;

    // Lock mip level 0 if required
    D3DLOCKED_RECT lockedRect = pD3DRaster->lockedRect;
    bool           bNeedOwnLock = (pD3DRaster->lockedLevel != 0) || !pD3DRaster->lockedSurface;
    if (bNeedOwnLock)
        if (FAILED(pD3DRaster->texture->LockRect(0, &lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
        {
            strError = "pD3DRaster->texture->LockRect failed";
            return NULL;
        }

    // Try resize
    CBuffer newPixelBuffer;
    bool    bDidResize =
        g_pCore->GetGraphics()->ResizeTextureData(lockedRect.pBits, lockedRect.Pitch, uiWidth, uiHeight, d3dFormat, uiReqWidth, uiReqHeight, newPixelBuffer);

    if (bNeedOwnLock)
        pD3DRaster->texture->UnlockRect(0);

    if (!bDidResize)
    {
        strError = "ResizeTextureData failed";
        return NULL;
    }

    // Make new RwTexture from pixels
    NativeTexturePC_Header header;
    memset(&header, 0, sizeof(header));
    header.TextureFormat.platformId = 9;
    header.TextureFormat.filterMode = pTexture->flags & 0xff;
    header.TextureFormat.uAddressing = (pTexture->flags & 0xf00) >> 8;
    header.TextureFormat.vAddressing = (pTexture->flags & 0xf000) >> 12;
    memcpy(header.TextureFormat.name, pTexture->name, 32);
    memcpy(header.TextureFormat.maskName, pTexture->mask, 32);
    header.RasterFormat.rasterFormat = (pRaster->format & 0x0f)
                                       << 8;            // ( dxt1 = 0x00000100 or 0x00000200 / dxt3 = 0x00000300 ) | 0x00008000 mipmaps?
    header.RasterFormat.d3dFormat = pD3DRaster->format;
    header.RasterFormat.width = uiReqWidth;
    header.RasterFormat.height = uiReqHeight;
    header.RasterFormat.depth = pRaster->depth;
    header.RasterFormat.numLevels = 1;
    header.RasterFormat.rasterType = pRaster->type;            // dxt1 = 4 / dxt3 = 4
    header.RasterFormat.alpha = bHasAlpha;
    header.RasterFormat.cubeTexture = bIsCubeTexture;
    header.RasterFormat.autoMipMaps = false;
    header.RasterFormat.compressed = bIsCompressed;

    // Create stream containing new texture data
    CBuffer            nativeData;
    CBufferWriteStream stream(nativeData);
    stream.Write(1);
    stream.Write(0);            // Size ignored
    stream.Write(0x1803FFFF);
    stream.WriteBytes(&header, sizeof(header));
    stream.Write(newPixelBuffer.GetSize());
    stream.WriteBytes(newPixelBuffer.GetData(), newPixelBuffer.GetSize());

    RwBuffer buffer;
    buffer.ptr = (void*)nativeData.GetData();
    buffer.size = nativeData.GetSize();
    RwStream* rwStream = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer);
    if (!rwStream)
    {
        strError = "RwStreamOpen failed";
        return NULL;
    }

    // Read new texture
    RwTexture* pNewRwTexture = NULL;
    rwD3D9NativeTextureRead(rwStream, &pNewRwTexture);
    RwStreamClose(rwStream, NULL);

    if (!pNewRwTexture)
    {
        strError = "rwD3D9NativeTextureRead failed";
        return NULL;
    }

    return pNewRwTexture;
}

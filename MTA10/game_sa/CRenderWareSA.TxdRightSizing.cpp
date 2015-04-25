/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.cpp
*  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
*               and miscellaneous rendering functions
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include "StdInc.h"
#include <game/RenderWareD3D.h>
#include "gamesa_renderware.h"


/////////////////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RightSizeTxd
//
// Check textures in a txd and shrink if required.
// Returns true if shrunk file was written
//
/////////////////////////////////////////////////////////////////////////////
bool CRenderWareSA::RightSizeTxd( const CBuffer& inTxd, const SString& strOutTxdFilename, uint uiSizeLimit )
{
    //
    // Read txd from memory
    //
    RwBuffer buffer;
    buffer.ptr = (void*)inTxd.GetData();
    buffer.size = inTxd.GetSize();
    RwStream* pStream = RwStreamOpen( STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer );
    if ( pStream == NULL )
        return false;

    // Find our txd chunk 
    if ( RwStreamFindChunk( pStream, 0x16, NULL, NULL ) == false )
    {
        RwStreamClose( pStream, NULL );
        return false;
    }

    // read the txd
    RwTexDictionary* pTxd = RwTexDictionaryGtaStreamRead( pStream );
    RwStreamClose( pStream, NULL );
    if ( !pTxd )
        return false;

    //
    // Process each texture in the txd
    //
    std::vector < RwTexture* > textureList;
    pGame->GetRenderWareSA()->GetTxdTextures( textureList, pTxd );

    bool bChanged = false;
    for ( std::vector < RwTexture* > ::iterator iter = textureList.begin() ; iter != textureList.end() ; iter++ )
    {
        RwTexture* pTexture = *iter;
        RwTexture* pNewRwTexture = RightSizeTexture( pTexture, uiSizeLimit );
        if ( pNewRwTexture && pNewRwTexture != pTexture )
        {
            // Replace texture in txd if changed
            RwTextureDestroy( pTexture );
            RwTexDictionaryAddTexture( pTxd, pNewRwTexture );
            bChanged = true;
        }
    }


    //
    // Save shrunked txd if changed
    //
    if ( bChanged )
    {
        pStream = RwStreamOpen( STREAM_TYPE_FILENAME, STREAM_MODE_WRITE, *strOutTxdFilename );
        if ( pStream )
        {
            RwTexDictionaryStreamWrite( pTxd, pStream );
            RwStreamClose( pStream, NULL );
            RwTexDictionaryDestroy( pTxd );
            return true;
        }
    }

    RwTexDictionaryDestroy( pTxd );
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
RwTexture* CRenderWareSA::RightSizeTexture( RwTexture* pTexture, uint uiSizeLimit )
{
    // Validate
    RwRaster* pRaster = pTexture->raster;
    if ( !pRaster )
        return NULL;

    RwD3D9Raster* pD3DRaster = (RwD3D9Raster*)( &pRaster->renderResource );
    if ( !pD3DRaster->texture || !pD3DRaster->lockedSurface || !pD3DRaster->lockedRect.pBits )
        return NULL;

    // Get texture info
    uint uiWidth = pRaster->width;
    uint uiHeight = pRaster->height;
    D3DFORMAT d3dFormat = pD3DRaster->format;
    void* pBits = pD3DRaster->lockedRect.pBits;
    int iPitch = pD3DRaster->lockedRect.Pitch;
    bool bHasAlpha = pD3DRaster->alpha != 0;
    bool bIsCubeTexture = ( pD3DRaster->cubeTextureFlags & 0x01 ) != 0;
    bool bHasMipMaps = ( pD3DRaster->textureFlags & 0x01 ) != 0;
    bool bIsCompressed = ( pD3DRaster->textureFlags & 0x10 ) != 0;

    // Check we can do this
    if ( bHasMipMaps || bIsCubeTexture || !bIsCompressed )
        return NULL;

    // Only process DXT formats
    if ( d3dFormat != D3DFMT_DXT1 && d3dFormat != D3DFMT_DXT3 && d3dFormat != D3DFMT_DXT5 )
        return NULL;

    // Change size
    uint uiReqWidth = Min( uiSizeLimit, uiWidth );
    uint uiReqHeight = Min( uiSizeLimit, uiHeight );
    if ( uiReqWidth == uiWidth && uiReqHeight == uiHeight )
        return NULL;

    // Try resize
    CBuffer newPixelBuffer;
    if ( !g_pCore->GetGraphics()->ResizeTextureData( pBits, iPitch, uiWidth, uiHeight, d3dFormat, uiReqWidth, uiReqHeight, newPixelBuffer ) )
        return NULL;

    // Make new RwTexture from pixels
    NativeTexturePC_Header header;
    memset( &header, 0, sizeof( header ) );
    header.TextureFormat.platformId = 9;
    header.TextureFormat.filterMode = pTexture->flags & 0xff;
    header.TextureFormat.uAddressing = ( pTexture->flags & 0xf00 ) >> 8;
    header.TextureFormat.vAddressing = ( pTexture->flags & 0xf000 ) >> 12;
    memcpy( header.TextureFormat.name, pTexture->name, 32 );
    memcpy( header.TextureFormat.maskName, pTexture->mask, 32 );
    header.RasterFormat.rasterFormat = pRaster->format << 8;    // dxt1 = 0x00000100 or 0x00000200 / dxt3 = 0x00000300
    header.RasterFormat.d3dFormat = pD3DRaster->format;
    header.RasterFormat.width = uiReqWidth;
    header.RasterFormat.height = uiReqHeight;
    header.RasterFormat.depth = pRaster->depth;
    header.RasterFormat.numLevels = 1;
    header.RasterFormat.rasterType = pRaster->type;     // dxt1 = 4 / dxt3 = 4
    header.RasterFormat.alpha = bHasAlpha;
    header.RasterFormat.cubeTexture = bIsCubeTexture;
    header.RasterFormat.autoMipMaps = false;
    header.RasterFormat.compressed = bIsCompressed;

    // Create stream containing new texture data
    CBuffer nativeData;
    CBufferWriteStream stream( nativeData );
    stream.Write( 1 );
    stream.Write( 0 );      // Size ignored
    stream.Write( 0x1803FFFF );
    stream.WriteBytes( &header, sizeof( header ) );
    stream.Write( newPixelBuffer.GetSize() );
    stream.WriteBytes( newPixelBuffer.GetData(), newPixelBuffer.GetSize() );

    RwBuffer buffer;
    buffer.ptr = (void*)nativeData.GetData();
    buffer.size = nativeData.GetSize();
    RwStream * rwStream = RwStreamOpen( STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer );
    if ( !rwStream )
        return NULL;

    // Read new texture
    RwTexture* pNewRwTexture = NULL;
    rwD3D9NativeTextureRead( rwStream, &pNewRwTexture );
    RwStreamClose( rwStream, NULL );

    return pNewRwTexture;
}

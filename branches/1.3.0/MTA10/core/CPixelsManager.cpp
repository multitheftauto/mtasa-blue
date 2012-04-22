/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CFileFormat.h"
#include "CPixelsManager.h"


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CPixelsManagerInterface* NewPixelsManager ( void )
{
    return new CPixelsManager ();
}



////////////////////////////////////////////////////////////////
//
// CPixelsManager::CPixelsManager
//
//
//
////////////////////////////////////////////////////////////////
CPixelsManager::CPixelsManager ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::~CPixelsManager
//
//
//
////////////////////////////////////////////////////////////////
CPixelsManager::~CPixelsManager ( void )
{
    SAFE_RELEASE ( m_pTempOffscreenSurface );
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CPixelsManager::OnDeviceCreate ( IDirect3DDevice9* pDevice )
{
    m_pDevice = pDevice;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetTexturePixels
//
// Copy pixels from texture
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetTexturePixels ( IDirect3DBaseTexture9* pD3DBaseTexture, CPixels& outPixels, const RECT* pRect, uint uiSurfaceIndex )
{
    if ( !pD3DBaseTexture )
        return false;

    IDirect3DSurface9* pD3DSurface = NULL;

    D3DRESOURCETYPE resourceType = pD3DBaseTexture->GetType ();
    if ( resourceType == D3DRTYPE_VOLUMETEXTURE )
    {
        return GetVolumeTexturePixels ( (IDirect3DVolumeTexture9*)pD3DBaseTexture, outPixels, pRect, uiSurfaceIndex );
    }
    else
    if ( resourceType == D3DRTYPE_CUBETEXTURE )
    {
        D3DCUBEMAP_FACES FaceType = (D3DCUBEMAP_FACES)uiSurfaceIndex;
        ((IDirect3DCubeTexture9*)pD3DBaseTexture)->GetCubeMapSurface ( FaceType, 0, &pD3DSurface );
        if ( !pD3DSurface )
            return false;
    }
    else
    if ( resourceType == D3DRTYPE_TEXTURE )
    {
        ((IDirect3DTexture9*)pD3DBaseTexture)->GetSurfaceLevel ( 0, &pD3DSurface );
        if ( !pD3DSurface )
            return false;
    }



    bool bResult = false;
    D3DSURFACE_DESC Desc;
    pD3DSurface->GetDesc ( &Desc );

    if ( Desc.Usage == D3DUSAGE_RENDERTARGET )
    {
        // Check we are allowed to read the render target
        bool bAllowScreenUpload = true;
        CVARS_GET ( "allow_screen_upload", bAllowScreenUpload );
        if ( bAllowScreenUpload )
        {
            // Get pixels onto offscreen surface
            IDirect3DSurface9* pLockableSurface = GetRTLockableSurface ( pD3DSurface );

            // Then
            bResult = GetSurfacePixels ( pLockableSurface, outPixels, pRect );
        }
        else
        {
            // If not allowed, return dummy data
            uint uiPixelsWidth = 32;
            uint uiPixelsHeight = 32;
            outPixels.SetSize ( uiPixelsWidth * uiPixelsHeight * XRGB_BYTES_PER_PIXEL + SIZEOF_PLAIN_TAIL );
            memset ( outPixels.GetData (), 0xEF, outPixels.GetSize () );
            bResult = SetPlainDimensions ( outPixels, uiPixelsWidth, uiPixelsHeight );
        }
    }
    else
    if ( Desc.Usage == 0 )
    {
        if ( Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8 || Desc.Format == D3DFMT_R5G6B5 )
        {
            // Direct reading will work here
            bResult = GetSurfacePixels ( pD3DSurface, outPixels, pRect );
        }
        else
        {
            // If not a simple format, use D3DX to convert
            if ( !GetMatchingOffscreenSurface ( pD3DSurface, m_pTempOffscreenSurface, D3DFMT_A8R8G8B8 ) )
                return false;

            if ( FAILED ( D3DXLoadSurfaceFromSurface ( m_pTempOffscreenSurface, NULL, NULL, pD3DSurface, NULL, NULL, D3DX_FILTER_NONE, 0 ) ) )
                return false;

            // Then
            bResult = GetSurfacePixels ( m_pTempOffscreenSurface, outPixels, pRect );
        }
    }

    SAFE_RELEASE ( pD3DSurface );
    return bResult;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetTexturePixels
//
// Copy pixels into texture
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetTexturePixels ( IDirect3DBaseTexture9* pD3DBaseTexture, const CPixels& pixels, const RECT* pRect, uint uiSurfaceIndex )
{
    if ( !pD3DBaseTexture )
        return false;

    IDirect3DSurface9* pD3DSurface = NULL;

    D3DRESOURCETYPE resourceType = pD3DBaseTexture->GetType ();
    if ( resourceType == D3DRTYPE_VOLUMETEXTURE )
    {
        return SetVolumeTexturePixels ( (IDirect3DVolumeTexture9*)pD3DBaseTexture, pixels, pRect, uiSurfaceIndex );
    }
    else
    if ( resourceType == D3DRTYPE_CUBETEXTURE )
    {
        D3DCUBEMAP_FACES FaceType = (D3DCUBEMAP_FACES)uiSurfaceIndex;
        ((IDirect3DCubeTexture9*)pD3DBaseTexture)->GetCubeMapSurface ( FaceType, 0, &pD3DSurface );
        if ( !pD3DSurface )
            return false;
    }
    else
    if ( resourceType == D3DRTYPE_TEXTURE )
    {
        ((IDirect3DTexture9*)pD3DBaseTexture)->GetSurfaceLevel ( 0, &pD3DSurface );
        if ( !pD3DSurface )
            return false;
    }


    bool bResult = false;
    D3DSURFACE_DESC Desc;
    pD3DSurface->GetDesc ( &Desc );

    if ( Desc.Usage == D3DUSAGE_RENDERTARGET )
    {
        // Get pixels onto offscreen surface
        IDirect3DSurface9* pLockableSurface = GetRTLockableSurface ( pD3DSurface );

        // Then
        bResult = SetSurfacePixels ( pLockableSurface, pixels, pRect );

        // And write back
        if ( bResult )
            if ( FAILED ( D3DXLoadSurfaceFromSurface ( pD3DSurface, NULL, NULL, pLockableSurface, NULL, NULL, D3DX_FILTER_NONE, 0 ) ) )
                return false;
    }
    else
    if ( Desc.Usage == 0 )
    {
        if ( Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8 || Desc.Format == D3DFMT_R5G6B5 )
        {
            // Direct reading will work here
            bResult = SetSurfacePixels ( pD3DSurface, pixels, pRect );
        }
        else
        {
            // If not a simple format, use D3DX to convert
            if ( !GetMatchingOffscreenSurface ( pD3DSurface, m_pTempOffscreenSurface, D3DFMT_A8R8G8B8 ) )
                return false;

            if ( FAILED ( D3DXLoadSurfaceFromSurface ( m_pTempOffscreenSurface, NULL, NULL, pD3DSurface, NULL, NULL, D3DX_FILTER_NONE, 0 ) ) )
                return false;

            // Then
            bResult = SetSurfacePixels ( m_pTempOffscreenSurface, pixels, pRect );

            // And write back
            if ( bResult )
                if ( FAILED ( D3DXLoadSurfaceFromSurface ( pD3DSurface, NULL, NULL, m_pTempOffscreenSurface, NULL, NULL, D3DX_FILTER_NONE, 0 ) ) )
                    return false;
        }
    }

    SAFE_RELEASE ( pD3DSurface );
    return bResult;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetSurfacePixels
//
// Here we have a surface which:
//          1 - is D3DFMT_R5G6B5 or D3DFMT_X8R8G8B8/D3DFMT_A8R8G8B8
//          2 - can be locked
//
// Returns PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetSurfacePixels ( IDirect3DSurface9* pD3DSurface, CPixels& outPixels, const RECT* pRect )
{
    if ( !pD3DSurface )
        return false;

    // Calc surface rect
    D3DSURFACE_DESC SurfDesc;
    pD3DSurface->GetDesc ( &SurfDesc );

    POINT SurfSize = { SurfDesc.Width, SurfDesc.Height };
    RECT SurfRect = { 0, 0, SurfDesc.Width, SurfDesc.Height };
    if ( pRect )
        SurfRect = *pRect;

    // Calc pixels rect
    uint uiPixelsWidth = GetRectWidth ( SurfRect );
    uint uiPixelsHeight = GetRectHeight ( SurfRect );

    POINT PixelsSize = { uiPixelsWidth, uiPixelsHeight };
    RECT PixelsRect = { 0, 0, uiPixelsWidth, uiPixelsHeight };

    // Validate rects
    if ( !ClipRects ( PixelsSize, PixelsRect, SurfSize, SurfRect ) )
        return false;

    // Check if pRect can be NULL
    pRect = &SurfRect;
    if ( SurfRect.left == 0 && SurfRect.top == 0 && SurfRect.right == SurfDesc.Width && SurfRect.bottom == SurfDesc.Height )
        pRect = NULL;        

    // Get pixels from pD3DSurface
    D3DLOCKED_RECT LockedRect;
    if ( FAILED ( LockSurfaceRect ( pD3DSurface, &LockedRect, pRect, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK ) ) )
        return false;

    // Get sizes
    uint SurfRectWidth = GetRectWidth ( SurfRect );
    uint SurfRectHeight = GetRectHeight ( SurfRect );

    uint PixelsRectWidth = GetRectWidth ( PixelsRect );
    uint PixelsRectHeight = GetRectHeight ( PixelsRect );

    uint CopyWidth = Min ( SurfRectWidth, PixelsRectWidth );
    uint CopyHeight = Min ( SurfRectHeight, PixelsRectHeight );

    // Prepare pixels
    uint ulPixelsPitch = uiPixelsWidth * XRGB_BYTES_PER_PIXEL;
    outPixels.SetSize ( ulPixelsPitch * uiPixelsHeight + SIZEOF_PLAIN_TAIL );
    memset ( outPixels.GetData (), 0x81,  outPixels.GetSize () );
    char* pPixelsData = outPixels.GetData ();
    pPixelsData += PixelsRect.left * XRGB_BYTES_PER_PIXEL;
    pPixelsData += PixelsRect.top * ulPixelsPitch;

    uint ulSurfPitch = LockedRect.Pitch;
    BYTE* pSurfBits = (BYTE*)LockedRect.pBits;

    if ( CRenderItemManager::GetBitsPerPixel ( SurfDesc.Format ) == 32 )
    {
        if (  SurfDesc.Format == D3DFMT_A8R8G8B8 )
        {
            uint uiCopyWidthBytes = CopyWidth * XRGB_BYTES_PER_PIXEL;
            // Copy lines into buffer
            for ( uint i = 0; i < CopyHeight; i++ )
            {
                memcpy ( pPixelsData + ulPixelsPitch * i, pSurfBits + i * ulSurfPitch, uiCopyWidthBytes );
            }
        }
        else
        {
            // Copy lines into buffer
            for ( uint i = 0; i < CopyHeight; i++ )
            {
                DWORD* pLinePixelsStart = (DWORD*)( pPixelsData + ulPixelsPitch * i );
                const DWORD* pLineSurfStart = (DWORD*)( pSurfBits + ulSurfPitch * i );
                for ( uint x = 0; x < CopyWidth; x++ )
                {
                    DWORD x8r8g8b8 = pLineSurfStart[x];
                    x8r8g8b8 |= 0xff000000;
                    pLinePixelsStart[x] = x8r8g8b8;
                }
            }
        }
    }
    else
    if ( CRenderItemManager::GetBitsPerPixel ( SurfDesc.Format ) == 16 )
    {
        // Copy lines into buffer
        for ( uint i = 0; i < CopyHeight; i++ )
        {
            DWORD* pLinePixelsStart = (DWORD*)( pPixelsData + ulPixelsPitch * i );
            const WORD* pLineSurfStart = (WORD*)( pSurfBits + ulSurfPitch * i );
            for ( uint x = 0; x < CopyWidth; x++ )
            {
                WORD r5g6b5 = pLineSurfStart[x];
                BYTE r = ( r5g6b5 & 0xF800 ) >> 11 << 3;
                BYTE g = ( r5g6b5 & 0x7E0 ) >> 5 << 2;
                BYTE b = ( r5g6b5 & 0x1F ) << 3;

                DWORD x8r8g8b8 = (r << 16) | (g << 8) | b;
                x8r8g8b8 |= 0xFF070307;
                pLinePixelsStart[x] = x8r8g8b8;
            }
        }
    }

    pD3DSurface->UnlockRect ();

    // Fixup plain format attributes
    return SetPlainDimensions ( outPixels, uiPixelsWidth, uiPixelsHeight );
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetSurfacePixels
//
// Here we have a surface which:
//          1 - is D3DFMT_R5G6B5 or D3DFMT_X8R8G8B8/D3DFMT_A8R8G8B8
//          2 - can be locked
//
// Requires PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetSurfacePixels ( IDirect3DSurface9* pD3DSurface, const CPixels& pixels, const RECT* pRect )
{
    if ( !pD3DSurface )
        return false;

    // Calc surface rect
    D3DSURFACE_DESC SurfDesc;
    pD3DSurface->GetDesc ( &SurfDesc );

    POINT SurfSize = { SurfDesc.Width, SurfDesc.Height };
    RECT SurfRect = { 0, 0, SurfDesc.Width, SurfDesc.Height };
    if ( pRect )
        SurfRect = *pRect;

    // Calc pixels rect
    uint uiPixelsWidth, uiPixelsHeight;
    if ( !GetPlainDimensions ( pixels, uiPixelsWidth, uiPixelsHeight ) )
        return false;

    POINT PixelsSize = { uiPixelsWidth, uiPixelsHeight };
    RECT PixelsRect = { 0, 0, uiPixelsWidth, uiPixelsHeight };

    // Validate rects
    if ( !ClipRects ( PixelsSize, PixelsRect, SurfSize, SurfRect ) )
        return false;

    // Check if pRect can be NULL
    pRect = &SurfRect;
    if ( SurfRect.left == 0 && SurfRect.top == 0 && SurfRect.right == SurfDesc.Width && SurfRect.bottom == SurfDesc.Height )
        pRect = NULL;        

    // Get pixels from pD3DSurface
    D3DLOCKED_RECT LockedRect;
    if ( FAILED ( LockSurfaceRect ( pD3DSurface, &LockedRect, pRect, D3DLOCK_NOSYSLOCK ) ) )
        return false;

    // Get sizes
    uint SurfRectWidth = GetRectWidth ( SurfRect );
    uint SurfRectHeight = GetRectHeight ( SurfRect );

    uint PixelsRectWidth = GetRectWidth ( PixelsRect );
    uint PixelsRectHeight = GetRectHeight ( PixelsRect );

    uint CopyWidth = Min ( SurfRectWidth, PixelsRectWidth );
    uint CopyHeight = Min ( SurfRectHeight, PixelsRectHeight );

    // Prepare pixels
    uint ulPixelsPitch = uiPixelsWidth * XRGB_BYTES_PER_PIXEL;
    const char* pPixelsData = pixels.GetData ();
    pPixelsData += PixelsRect.left * XRGB_BYTES_PER_PIXEL;
    pPixelsData += PixelsRect.top * ulPixelsPitch;

    uint ulSurfPitch = LockedRect.Pitch;
    BYTE* pSurfBits = (BYTE*)LockedRect.pBits;

    if ( CRenderItemManager::GetBitsPerPixel ( SurfDesc.Format ) == 32 )
    {
        if (  SurfDesc.Format == D3DFMT_A8R8G8B8 )
        {
            uint uiCopyWidthBytes = CopyWidth * XRGB_BYTES_PER_PIXEL;
            // Copy lines into surface
            for ( uint i = 0; i < CopyHeight; i++ )
            {
                memcpy ( pSurfBits + ulSurfPitch * i, pPixelsData + ulPixelsPitch * i, uiCopyWidthBytes );
            }
        }
        else
        {
            // Copy lines into surface
            for ( uint i = 0; i < CopyHeight; i++ )
            {
                const DWORD* pLinePixelsStart = (DWORD*)( pPixelsData + ulPixelsPitch * i );
                DWORD* pLineSurfStart = (DWORD*)( pSurfBits + ulSurfPitch * i );
                for ( uint x = 0; x < CopyWidth; x++ )
                {
                    DWORD x8r8g8b8 = pLinePixelsStart[x];
                    x8r8g8b8 |= 0xff000000;
                    pLineSurfStart[x] = x8r8g8b8;
                }
            }
        }
    }
    else
    if ( CRenderItemManager::GetBitsPerPixel ( SurfDesc.Format ) == 16 )
    {
        // Copy lines into surface
        for ( uint i = 0; i < CopyHeight; i++ )
        {
            const DWORD* pLinePixelsStart = (DWORD*)( pPixelsData + ulPixelsPitch * i );
            WORD* pLineSurfStart = (WORD*)( pSurfBits + ulSurfPitch * i );
            for ( uint x = 0; x < CopyWidth; x++ )
            {
                DWORD x8r8g8b8 = pLinePixelsStart[x];
                WORD r = ( x8r8g8b8 & 0xF80000 ) >> 8;
                WORD g = ( x8r8g8b8 & 0xFC00 ) >> 5;
                WORD b = ( x8r8g8b8 & 0xF8 ) >> 3;
                WORD r5g6b5 = r | g | b;
                pLineSurfStart[x] = r5g6b5;
            }
        }
    }

    pD3DSurface->UnlockRect ();
    return true;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetRTLockableSurface
//
// Get a surface containing the rendertarget data which is lockable
//
////////////////////////////////////////////////////////////////
IDirect3DSurface9* CPixelsManager::GetRTLockableSurface ( IDirect3DSurface9* pRTSurface )
{
    // Make sure we have an offscreen surface which is exactly the same as the render target
    if ( !GetMatchingOffscreenSurface ( pRTSurface, m_pTempOffscreenSurface ) )
        return NULL;

    // Copy render target to m_pTempOffscreenSurface
    if ( FAILED ( m_pDevice->GetRenderTargetData ( pRTSurface, m_pTempOffscreenSurface ) ) )
        return NULL;

    return m_pTempOffscreenSurface;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetMatchingOffscreenSurface
//
//
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetMatchingOffscreenSurface ( IDirect3DSurface9* pD3DSurface, IDirect3DSurface9*& pOffscreenSurface, D3DFORMAT ReqFormat )
{
    D3DSURFACE_DESC ReqDesc;
    pD3DSurface->GetDesc ( &ReqDesc );

    if ( ReqFormat == D3DFMT_UNKNOWN )
        ReqFormat = ReqDesc.Format;

    if ( pOffscreenSurface )
    {
        D3DSURFACE_DESC CurDesc;
        pOffscreenSurface->GetDesc ( &CurDesc );
        if ( CurDesc.Width != ReqDesc.Width || CurDesc.Height != ReqDesc.Height || CurDesc.Format != ReqFormat )
        {
            SAFE_RELEASE( pOffscreenSurface )
        }
    }

    if ( !pOffscreenSurface )
        if ( FAILED ( m_pDevice->CreateOffscreenPlainSurface ( ReqDesc.Width, ReqDesc.Height, ReqFormat, D3DPOOL_SYSTEMMEM, &pOffscreenSurface, NULL ) ) )
            return false;

    return true;
}



////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetPixelsSize
//
//
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetPixelsSize ( const CPixels& pixels, uint& uiOutWidth, uint& uiOutHeight )
{
    EPixelsFormatType format = GetPixelsFormat ( pixels );

    if ( format == EPixelsFormat::PLAIN )
    {
        return GetPlainDimensions ( pixels, uiOutWidth, uiOutHeight );
    }
    else
    if ( format == EPixelsFormat::PNG )
    {
        return PngGetDimensions ( pixels.GetData (), pixels.GetSize (), uiOutWidth, uiOutHeight );
    }
    else
    if ( format == EPixelsFormat::JPEG )
    {
        return JpegGetDimensions ( pixels.GetData (), pixels.GetSize (), uiOutWidth, uiOutHeight );
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetPixelsFormat
//
// Auto detect PNG, JPEG or PLAIN
//
////////////////////////////////////////////////////////////////
EPixelsFormatType CPixelsManager::GetPixelsFormat ( const CPixels& pixels )
{
    uint uiDataSize = pixels.GetSize ();
    const char* pData = pixels.GetData ();

    // Check if png
    if ( IsPng ( pData, uiDataSize ) )
        return EPixelsFormat::PNG;

    // Check if jpeg
    if ( IsJpeg ( pData, uiDataSize ) )
        return EPixelsFormat::JPEG;

    // Check if plain
    if ( uiDataSize >= 8 )
    {
        uint uiWidth, uiHeight;
        if ( GetPlainDimensions ( pixels, uiWidth, uiHeight ) )
            return EPixelsFormat::PLAIN;
    }

    return EPixelsFormat::UNKNOWN;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::IsPixels
//
// Returns true if content is pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::IsPixels ( const CPixels& pixels )
{
    return GetPixelsFormat ( pixels ) != EPixelsFormat::UNKNOWN;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetPlainDimensions
//
// Sets the width and height attributes
// Returns false if not possible
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetPlainDimensions ( CPixels& pixels, uint uiWidth, uint uiHeight)
{
    uint uiDataSize = pixels.GetSize ();
    const char* pData = pixels.GetData ();

    uint ReqSize = uiWidth * uiHeight * 4 + SIZEOF_PLAIN_TAIL;

    if ( ReqSize == uiDataSize )
    {
        // Fixup plain format tail
        WORD* pPlainTail = (WORD*)( pData + uiDataSize - SIZEOF_PLAIN_TAIL );
        pPlainTail[0] = uiWidth;
        pPlainTail[1] = uiHeight;
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetPlainDimensions
//
// Reads the width and height attributes
// Returns false if not possible
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetPlainDimensions ( const CPixels& pixels, uint& uiOutWidth, uint& uiOutHeight )
{
    uint uiDataSize = pixels.GetSize ();
    const char* pData = pixels.GetData ();

    // Check if plain
    if ( uiDataSize >= 8 )
    {
        const ushort* pPlainTail = (const ushort*)( pData + uiDataSize - SIZEOF_PLAIN_TAIL );
        uiOutWidth = pPlainTail[0];
        uiOutHeight = pPlainTail[1];
        uint uiPlainByteSize = uiOutWidth * uiOutHeight * 4 + SIZEOF_PLAIN_TAIL;
        if ( uiDataSize == uiPlainByteSize )
            return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::ChangePixelsFormat
//
// JPEG <-> PNG <-> PLAIN
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::ChangePixelsFormat ( const CPixels& oldPixels, CPixels& newPixels, EPixelsFormatType newFormat, int uiQuality )
{
    EPixelsFormatType oldFormat = GetPixelsFormat ( oldPixels );
    if ( oldFormat == EPixelsFormat::UNKNOWN || newFormat == EPixelsFormat::UNKNOWN )
        return false;

    if ( oldFormat == newFormat )
    {
        // No change
        newPixels = oldPixels;
        return true;
    }

    if ( oldFormat == EPixelsFormat::PLAIN )
    {
        // Encode
        uint uiWidth, uiHeight;
        if ( !GetPixelsSize ( oldPixels, uiWidth, uiHeight ) )
            return false;

        if ( newFormat == EPixelsFormat::JPEG )
            return JpegEncode ( uiWidth, uiHeight, uiQuality, oldPixels.GetData (), oldPixels.GetSize () - 4, newPixels.buffer );
        else
        if ( newFormat == EPixelsFormat::PNG )
            return PngEncode ( uiWidth, uiHeight, oldPixels.GetData (), oldPixels.GetSize () - 4, newPixels.buffer );
    }
    else
    if ( newFormat == EPixelsFormat::PLAIN )
    {
        // Decode
        if ( oldFormat == EPixelsFormat::JPEG )
        {
            uint uiWidth, uiHeight;
            if ( JpegDecode ( oldPixels.GetData (), oldPixels.GetSize (), newPixels.buffer, uiWidth, uiHeight ) )
            {
                newPixels.buffer.SetSize ( uiWidth * uiHeight * 4 + SIZEOF_PLAIN_TAIL );
                return SetPlainDimensions ( newPixels, uiWidth, uiHeight );
            }
        }
        else
        if ( oldFormat == EPixelsFormat::PNG )
        {
            uint uiWidth, uiHeight;
            if ( PngDecode ( oldPixels.GetData (), oldPixels.GetSize (), newPixels.buffer, uiWidth, uiHeight ) )
            {
                newPixels.buffer.SetSize ( uiWidth * uiHeight * 4 + SIZEOF_PLAIN_TAIL );
                return SetPlainDimensions ( newPixels, uiWidth, uiHeight );
            }
        }
    }
    else
    {
        // Recode
        CPixels tempPixels;
        if ( ChangePixelsFormat ( oldPixels, tempPixels, EPixelsFormat::PLAIN ) )
            return ChangePixelsFormat ( tempPixels, newPixels, newFormat, uiQuality );
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetPixelColor
//
// Only works on PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::GetPixelColor ( const CPixels& pixels, uint uiPosX, uint uiPosY, SColor& outColor )
{
    uint uiWidth, uiHeight;
    if ( !GetPlainDimensions ( pixels, uiWidth, uiHeight ) )
        return false;

    if ( uiPosX >= uiWidth || uiPosY >= uiHeight )
        return false;

    uint uiOffset = uiPosX + uiPosY * uiHeight;

    const SColor* pColorArray = (const SColor*)pixels.GetData ();
    outColor = pColorArray [ uiOffset ];

    return true;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::SetPixelColor
//
// Only works on PLAIN pixels
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::SetPixelColor ( CPixels& pixels, uint uiPosX, uint uiPosY, const SColor color )
{
    uint uiWidth, uiHeight;
    if ( !GetPlainDimensions ( pixels, uiWidth, uiHeight ) )
        return false;

    if ( uiPosX >= uiWidth || uiPosY >= uiHeight )
        return false;

    uint uiOffset = uiPosX + uiPosY * uiHeight;

    SColor* pColorArray = (SColor*)pixels.GetData ();
    pColorArray [ uiOffset ] = color;

    return true;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetRectWidth
//
// Static utility functions
//
////////////////////////////////////////////////////////////////
int CPixelsManager::GetRectWidth ( const RECT& rc )
{
    return rc.right - rc.left;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::GetRectHeight
//
// Static utility functions
//
////////////////////////////////////////////////////////////////
int CPixelsManager::GetRectHeight ( const RECT& rc )
{
    return rc.bottom - rc.top;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::ClipRects
//
// Static utility functions
//
////////////////////////////////////////////////////////////////
bool CPixelsManager::ClipRects ( const POINT& SrcSize, RECT& SrcRect, const POINT& DestSize, RECT& DestRect )
{
    // Left bounds
    int LowestLeft = Min ( SrcRect.left, DestRect.left );
    int MoveX = Max ( -LowestLeft, 0 );
    SrcRect.left += MoveX;
    DestRect.left += MoveX;

    // Top bounds
    int LowestTop = Min ( SrcRect.top, DestRect.top );
    int MoveY = Max ( -LowestTop, 0 );
    SrcRect.top += MoveY;
    DestRect.top += MoveY;

    // Right bounds
    SrcRect.right = Min ( SrcRect.right, SrcSize.x );
    DestRect.right = Min ( DestRect.right, DestSize.x );

    // Bottom bounds
    SrcRect.bottom = Min ( SrcRect.bottom, SrcSize.y );
    DestRect.bottom = Min ( DestRect.bottom, DestSize.y );

    if ( SrcRect.right - SrcRect.left > 0 && SrcRect.bottom - SrcRect.top > 0 &&
         DestRect.right - DestRect.left > 0 && DestRect.bottom - DestRect.top > 0 )
        return true;
    return false;
}


////////////////////////////////////////////////////////////////
//
// CPixelsManager::LockSurfaceRect
//
// Static utility functions
// Surface lock with bounds checking
//
////////////////////////////////////////////////////////////////
HRESULT CPixelsManager::LockSurfaceRect ( IDirect3DSurface9* pD3DSurface, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags )
{
    if ( pRect )
    {
        D3DSURFACE_DESC SurfDesc;
        pD3DSurface->GetDesc ( &SurfDesc );
        assert ( pRect->left >= 0 &&
                pRect->top >= 0 &&
                pRect->right <= (int)SurfDesc.Width &&
                pRect->bottom <= (int)SurfDesc.Height );
    }

    return pD3DSurface->LockRect ( pLockedRect, pRect, Flags );
}

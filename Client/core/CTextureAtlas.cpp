#include "StdInc.h"
#include <rectpack2D\src\finders_interface.h>
#include "CTextureAtlas.h"
#include <iostream>
#include <vector>
#include <algorithm> 

#include <wincodec.h>

#include "..\..\Client\sdk\core\CTextureAtlasInterface.h"

using namespace rectpack2D;

auto RwTextureCreate = (RwTexture*(__cdecl*)(RwRaster* raster))0x007F37C0;
auto RwRasterLock = (RwUInt8 *(__cdecl*)(RwRaster *raster, RwUInt8 level, RwInt32 lockMode))0x07FB2D0;
auto RwRasterUnlock = (RwRaster *(__cdecl*)(RwRaster *raster))0x7FAEC0;
auto RwD3D9RasterCreate = (RwRaster *(__cdecl*)(RwUInt32 width,RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags))0x4CD050;
auto WriteRaster = (bool(__cdecl*)(RwRaster *raster, char *filename))0x5A4150;

extern CCore* g_pCore;


CTextureAtlas::CTextureAtlas()
{
}

CTextureInfo* CTextureAtlas::GetTextureInfoByName(unsigned int uiTextureNameHash)
{
    for (auto& textureInfo : m_vecTexturesInfo)
    {
        if (uiTextureNameHash == textureInfo.GetNameHash())
        {
            return &textureInfo;
        }
    }
    return nullptr;
}

bool CTextureAtlas::RemoveTextureInfoTillSuccess(std::vector <RwTexture*>& vecTexturesRemoved)
{
    for (auto it = m_vecTexturesInfo.begin(); it != m_vecTexturesInfo.end();)
    {
        RwTexture* pTexture = it->GetTexture();
        vecTexturesRemoved.push_back(pTexture);

        eTextureAtlasErrorCodes textureReturn = CreateAtlas();
        if (textureReturn == TEX_ATLAS_SUCCESS)
        {
            break;
        }
        else if (textureReturn != TEX_ATLAS_CANT_FIT_INTO_ATLAS)
        {
            return false;
        }

        it = m_vecTexturesInfo.erase(it);
    }
    return true;
}

void CTextureAtlas::AddTextureInfo(RwTexture* pTexture, const float theWidth, const float theHeight)
{
    m_vecTexturesInfo.emplace_back(pTexture, theWidth, theHeight, this);
}

bool CTextureAtlas::CreateAtlasTextureResource(const float fWidth, const float fHeight)
{

    RwRaster* raster = RwD3D9RasterCreate(fWidth, fHeight, m_kTextureFormat,
        rwRASTERTYPETEXTURE | (m_kTextureFormat & 0x9000));
    if (!raster)
    {
        std::printf("CreateAtlasTextureResource: RwD3D9RasterCreate Failed\n");
        return false;
    }

    m_pAtlasTexture = RwTextureCreate(raster);
    if (!m_pAtlasTexture)
    {
        std::printf("CreateAtlasTextureResource: RwTextureCreate Failed\n");
        return false;
    }
    vecAtlasSize = CVector2D(fWidth, fHeight);
    return true;
}

///////////////////////////////////////////////////////////////////////////
// GTA SA doesn't use mipmaps, but it is supported.
// An option should be given to the client to allow generating mipmaps.
// The downside is that mipmaps will take extra memory.
///////////////////////////////////////////////////////////////////////////
bool CTextureAtlas::CopyTextureToAtlas(CTextureInfo& textureInfo)
{
    CRenderWare* pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();

    RwTexture* pTexture = textureInfo.GetTexture();
    RECT& texturePositionInAtlas = textureInfo.GetRegion();

    RwRaster* sourceRaster = textureInfo.GetTexture()->raster;
    RwRaster* destinationRaster = m_pAtlasTexture->raster;

    _rwD3D9RasterExt* destinationRasterExt = pRenderware->GetRasterExt(destinationRaster);

    RECT            dstRect;
    D3DLOCKED_RECT  dstLockedRect;

    const size_t totalMipMapLevels = 1;
    for (long mipLevel = 0; mipLevel < totalMipMapLevels; ++mipLevel)
    {
        long const div = static_cast<long>(pow(2L, mipLevel));
        DWORD textureWidth = texturePositionInAtlas.right - texturePositionInAtlas.left;
        DWORD textureHeight = texturePositionInAtlas.bottom - texturePositionInAtlas.top;
        std::printf("CopyTextureToAtlas: textureWidth: %d | textureHeight: %d\n", (int)textureWidth, (int)textureHeight);
        long const mipWidth = std::max(1UL, textureWidth / div);
        long const mipHeight = std::max(1UL, textureHeight / div);

        dstRect.left = std::max(0L, texturePositionInAtlas.left / div);
        dstRect.top = std::max(0L, texturePositionInAtlas.top / div);
        dstRect.right = dstRect.left + mipWidth;
        dstRect.bottom = dstRect.top + mipHeight;

        UCHAR *srcPtr = RwRasterLock(sourceRaster, mipLevel, rwRASTERLOCKREAD);
        if (!srcPtr)
        {
            std::printf("RwRasterLock: failed to lock sourceRaster\n");
            return false;
        }

        HRESULT hr = destinationRasterExt->texture->LockRect(mipLevel, &dstLockedRect, &dstRect, 0);
        if (hr != S_OK)
        {
            std::printf("atlas texture LockRect: failed to lock dstLockedRect error: %#.8x\n", hr);
            return false;
        }

        UCHAR *dstPtr = (UCHAR *)dstLockedRect.pBits; 

        int const kBytesPerRow = (mipWidth * SizeOfTexel(m_kTextureFormat)) / 8;
        int const kBlockFactor = (IsDXTnFormat(m_kTextureFormat) ? 4 : 1);

        for (int i = 0; i < mipHeight / kBlockFactor; ++i)
        {
            memcpy(dstPtr, srcPtr, kBlockFactor * kBytesPerRow);
            srcPtr += sourceRaster->stride;
            dstPtr += dstLockedRect.Pitch;
        }

        if (!RwRasterUnlock(sourceRaster))
        {
            std::printf("RwRasterUnlock: failed to unlock sourceRaster\n");
            return false;
        }

        hr = destinationRasterExt->texture->UnlockRect(mipLevel);
        if (hr != S_OK)
        {
            std::printf("atlas texture UnlockRect: failed to unlock error: %#.8x\n", hr);
            return false;
        }
    }
    WriteRaster(m_pAtlasTexture->raster, "C:\\Users\\danish\\Desktop\\atlasTXD\\textureAtlas.png");
    return true;
}

eTextureAtlasErrorCodes CTextureAtlas::CreateAtlas()
{
    constexpr bool allow_flip = false; // true;
    const auto runtime_flipping_mode = flipping_option::DISABLED; // ENABLED;
    using spaces_type = rectpack2D::empty_spaces<allow_flip, default_empty_spaces>;

    /*
    rect_xywh or rect_xywhf (see src/rect_structs.h),
    depending on the value of allow_flip.
    */

    using rect_type = output_rect_t<spaces_type>;
    auto report_successful = [](rect_type&) {
        return callback_result::CONTINUE_PACKING;
    };

    bool bFailedToPack = false;
    auto report_unsuccessful = [&](rect_type&) {
        bFailedToPack = true;
        std::printf("report_unsuccessful called\n");
        return callback_result::ABORT_PACKING;
    };

    /*
    Initial size for the bin, from which the search begins.
    The result can only be smaller - if it cannot, the algorithm will gracefully fail.
    */
    const auto maxAtlasSize = 16384;
    const auto discard_step = 1;

    /*
    Create some arbitrary rectangles.
    Every subsequent call to the packer library will only read the widths and heights that we now specify,
    and always overwrite the x and y coordinates with calculated results.
    */

    std::vector<rect_type> rectangles;

    for (size_t i = 0; i< GetTextureInfoCount(); i++)
    {
        CTextureInfo& textureInfo = GetTextureInfo(i);
        rectangles.emplace_back(rect_xywh(0, 0, textureInfo.GetWidth(), textureInfo.GetHeight()));
    }

    auto report_result = [&](const rect_wh& result_size) {
        std::cout << "Resultant bin: " << result_size.w << " " << result_size.h << std::endl;

        if (!CreateAtlasTextureResource(result_size.w, result_size.h))
        {
            return TEX_ATLAS_TEXTURE_RESOURCE_CREATE_FAILED;
        }

        for (DWORD i = 0; i < rectangles.size(); i++) 
        {
            rect_type& textureRectangle = rectangles[i];
            CTextureInfo& textureInfo = GetTextureInfo(i);
            float width = textureRectangle.w;
            float height = textureRectangle.h;

            /*if (r.flipped)
            {
            width = r.h;
            height = r.w;
            }*/

            char*textureName = textureInfo.GetTexture()->name;
            std::cout << textureName << " | " << 
                textureRectangle.x << " " << textureRectangle.y << " " << width << " " << height << std::endl;

            textureInfo.GetRegion() = { static_cast<LONG>(textureRectangle.x),static_cast<LONG>(textureRectangle.y),
                                            static_cast<LONG>(width + textureRectangle.x), static_cast<LONG>(height + textureRectangle.y) };

            if (IsTextureFormatDifferentFromAtlas(textureInfo))
            {
                RwTexture* pTexture = CreateTextureWithAtlasFormat(textureInfo);
                if (!pTexture)
                {
                    return TEX_ATLAS_DX_ERROR;
                }

                textureInfo.SetTexture(pTexture);
            }

            if (!CopyTextureToAtlas(textureInfo))
            {
                return TEX_ATLAS_COPY_TO_ATLAS_FAILED;
            }
        }

        //D3DXSaveTextureToFile("C:\\Users\\danish\\Desktop\\myLovelyAtlas.png", D3DXIFF_PNG, m_pAtlasTexture, NULL);
    };

    const auto result_size = find_best_packing<spaces_type>(
        rectangles,
        make_finder_input(
            maxAtlasSize,
            discard_step,
            report_successful,
            report_unsuccessful,
            runtime_flipping_mode
        )
        );

    if (bFailedToPack)
    {
        return TEX_ATLAS_CANT_FIT_INTO_ATLAS;
    }

    report_result(result_size);
    return TEX_ATLAS_SUCCESS;
}

void CTextureAtlas::GetRasterRect(RwRaster* raster, RECT& rect)
{
    rect.left = raster->nOffsetX;
    rect.top = raster->nOffsetY;
    rect.right = raster->nOffsetX + raster->width;
    rect.bottom = raster->nOffsetY + raster->height;
}

bool CTextureAtlas::IsTextureFormatDifferentFromAtlas(CTextureInfo &textureInfo)
{
    CRenderWare* pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();

    RwTexture* pTexture = textureInfo.GetTexture();
    RwRaster* raster = pTexture->raster;
    _rwD3D9RasterExt* rasterExt = pRenderware->GetRasterExt(raster);
    return rasterExt->d3dFormat != m_kTextureFormat;
}

///////////////////////////////////////////////////////////////////////////
// GTA SA doesn't use mipmaps, but it is supported.
// An option should be given to the client to allow generating mipmaps.
// The downside is that mipmaps will take extra memory.
///////////////////////////////////////////////////////////////////////////
RwTexture* CTextureAtlas::CreateTextureWithAtlasFormat(CTextureInfo &textureInfo)
{
    CRenderWare* pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();

    RwTexture* pTexture = textureInfo.GetTexture();
    RwRaster* raster = pTexture->raster;

    HRESULT hr = NULL;
 
    RwRaster* convertedRaster = RwD3D9RasterCreate(raster->width, raster->height, m_kTextureFormat,
        rwRASTERTYPETEXTURE | (m_kTextureFormat & 0x9000));
    if (!convertedRaster)
    {
        std::printf("RwD3D9RasterCreate: Failed\n");
        return nullptr;
    }

    _rwD3D9RasterExt* rasterExt = pRenderware->GetRasterExt(raster);
    _rwD3D9RasterExt* convertedRasterExt = pRenderware->GetRasterExt(convertedRaster);

    RECT sourceRect, destinationRect;
    GetRasterRect(raster, sourceRect);
    GetRasterRect(convertedRaster, destinationRect);

    IDirect3DSurface9 * sourceSurface;
    hr = rasterExt->texture->GetSurfaceLevel(0, &sourceSurface);
    if (hr != D3D_OK)
    {
        std::printf("Get surface level for sourceSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    IDirect3DSurface9 * destinationSurface;
    hr = convertedRasterExt->texture->GetSurfaceLevel(0, &destinationSurface);
    if (hr != D3D_OK)
    {
        std::printf("Get surface level for destinationSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    hr = D3DXLoadSurfaceFromSurface(
        destinationSurface, NULL, &destinationRect,
        sourceSurface, NULL, &sourceRect,
        D3DX_DEFAULT, 0); 
    if (FAILED(hr))
    {
        std::printf("D3DXLoadSurfaceFromSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    RwTexture* pConvertedTexture = RwTextureCreate(convertedRaster);
    memcpy (pConvertedTexture->name, pTexture->name, RW_TEXTURE_NAME_LENGTH);
    memcpy(pConvertedTexture->mask, pTexture->mask, RW_TEXTURE_NAME_LENGTH);

    std::printf("texture successfully converted to D3DFormat = %u FROM d3dFOrmat: %u\n", m_kTextureFormat, rasterExt->d3dFormat);

    SString theDirPath = "C:\\Users\\danish\\Desktop\\atlasTXD\\";
    theDirPath += pTexture->name;
    theDirPath += ".png";

    //"C:\\Users\\danish\\Desktop\\ConvertedRaster.png"
    if (!WriteRaster(convertedRaster, (char*)theDirPath.c_str()))
    {
        std::printf("WriteRaster failed\n");
    }

    return pConvertedTexture;
}

//-----------------------------------------------------------------------------
// Name: IsSupportedFormat()
// Desc: return true if the format is supported 
//       The list of supported formats are essentially all formats that we 
//       currently know how to deal with, ie they have a known size.  
//       It is a fail-safe mechansim to not have to deal w/ unknown 4CC codes.
//-----------------------------------------------------------------------------~
bool CTextureAtlas::IsSupportedFormat(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_R3G3B2:
    case D3DFMT_A8:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_X8B8G8R8:
    case D3DFMT_G16R16:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A16B16G16R16:
    case D3DFMT_A8P8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_L16:
    case D3DFMT_A8L8:
    case D3DFMT_A4L4:
    case D3DFMT_V8U8:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_Q16W16V16U16:
    case D3DFMT_CxV8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_G8R8_G8B8:
    case D3DFMT_R8G8_B8G8:
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
    case D3DFMT_R16F:
    case D3DFMT_G16R16F:
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_R32F:
    case D3DFMT_G32R32F:
    case D3DFMT_A32B32G32R32F:
        break;
    default:
        return false;
    }
    return true;
}

int CTextureAtlas::SizeOfTexel(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_R8G8B8:			return 3 * 8;
    case D3DFMT_A8R8G8B8:		return 4 * 8;
    case D3DFMT_X8R8G8B8:		return 4 * 8;
    case D3DFMT_R5G6B5:			return 2 * 8;
    case D3DFMT_X1R5G5B5:		return 2 * 8;
    case D3DFMT_A1R5G5B5:		return 2 * 8;
    case D3DFMT_A4R4G4B4:		return 2 * 8;
    case D3DFMT_R3G3B2:			return 8;
    case D3DFMT_A8:				return 8;
    case D3DFMT_A8R3G3B2:		return 2 * 8;
    case D3DFMT_X4R4G4B4:		return 2 * 8;
    case D3DFMT_A2B10G10R10:	return 4 * 8;
    case D3DFMT_A8B8G8R8:		return 4 * 8;
    case D3DFMT_X8B8G8R8:		return 4 * 8;
    case D3DFMT_G16R16:			return 4 * 8;
    case D3DFMT_A2R10G10B10:	return 4 * 8;
    case D3DFMT_A16B16G16R16:	return 8 * 8;
    case D3DFMT_A8P8:			return 8;
    case D3DFMT_P8:				return 8;
    case D3DFMT_L8:				return 8;
    case D3DFMT_L16:			return 2 * 8;
    case D3DFMT_A8L8:			return 2 * 8;
    case D3DFMT_A4L4:			return 8;
    case D3DFMT_V8U8:			return 2 * 8;
    case D3DFMT_Q8W8V8U8:		return 4 * 8;
    case D3DFMT_V16U16:			return 4 * 8;
    case D3DFMT_Q16W16V16U16:	return 8 * 8;
    case D3DFMT_CxV8U8:			return 2 * 8;
    case D3DFMT_L6V5U5:			return 2 * 8;
    case D3DFMT_X8L8V8U8:		return 4 * 8;
    case D3DFMT_A2W10V10U10:	return 4 * 8;
    case D3DFMT_G8R8_G8B8:		return 2 * 8;
    case D3DFMT_R8G8_B8G8:		return 2 * 8;
    case D3DFMT_DXT1:			return 4;
    case D3DFMT_DXT2:			return 8;
    case D3DFMT_DXT3:			return 8;
    case D3DFMT_DXT4:			return 8;
    case D3DFMT_DXT5:			return 8;
    case D3DFMT_UYVY:			return 2 * 8;
    case D3DFMT_YUY2:			return 2 * 8;
    case D3DFMT_R16F:			return 2 * 8;
    case D3DFMT_G16R16F:		return 4 * 8;
    case D3DFMT_A16B16G16R16F:	return 8 * 8;
    case D3DFMT_R32F:			return 4 * 8;
    case D3DFMT_G32R32F:		return 8 * 8;
    case D3DFMT_A32B32G32R32F:	return 16 * 8;
    default:
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Name: IsDXTnFormat()
// Desc: Returns true if the given format is a DXT format, false otherwise
//-----------------------------------------------------------------------------
bool CTextureAtlas::IsDXTnFormat(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
        return true;
    default:
        return false;
    }
}

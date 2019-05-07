#include "StdInc.h"
#include <rectpack2D\src\finders_interface.h>
#include "CTextureAtlas.h"
#include <iostream>
#include <vector>
#include <algorithm> 

using namespace rectpack2D;

/*
vecTexturesInfo.emplace_back(STextureInfo(256, 256)); //grass.png
vecTexturesInfo.emplace_back(STextureInfo(512, 512)); //nvidia_cloth.png
vecTexturesInfo.emplace_back(STextureInfo(128, 128)); //1d_debug.png
vecTexturesInfo.emplace_back(STextureInfo(64, 64)); //shine.png
vecTexturesInfo.emplace_back(STextureInfo(256, 265)); //braynzar.jpg
*/

extern CCore* g_pCore;

D3DFORMAT theFormatForAtlas;

CTextureAtlas::CTextureAtlas()
{
    Initialize();
}

CTextureAtlas::CTextureAtlas(std::vector <CTextureInfo>& vecTexturesInfo)
{
    Initialize();
    m_pVecTexturesInfo = &vecTexturesInfo;
    CreateAtlas();
}

void CTextureAtlas::Initialize()
{
    m_pVecTexturesInfo = nullptr;
    m_dwMaximumMipMapLevel = 1;
}

bool CTextureAtlas::CreateAtlas()
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

    auto report_unsuccessful = [](rect_type&) {
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

    for (auto& textureInfo : *m_pVecTexturesInfo) {
        rectangles.emplace_back(rect_xywh(0, 0, textureInfo.GetWidth(), textureInfo.GetHeight()));
        LoadTextureFromFile(textureInfo);

        DWORD dwMipMapLevel = textureInfo.GetTexture()->GetLevelCount();
        if (dwMipMapLevel > m_dwMaximumMipMapLevel)
        {
            m_dwMaximumMipMapLevel = dwMipMapLevel;
        }
    }

    auto report_result = [&](const rect_wh& result_size) {
        std::cout << "Resultant bin: " << result_size.w << " " << result_size.h << std::endl;

        CreateAtlasTextureResource(result_size.w, result_size.h);

        for (DWORD i = 0; i < rectangles.size(); i++) 
        {
            rect_type& textureRectangle = rectangles[i];
            CTextureInfo& textureInfo = m_pVecTexturesInfo->at(i);
            float width = textureRectangle.w;
            float height = textureRectangle.h;

            /*if (r.flipped)
            {
            width = r.h;
            height = r.w;
            }*/

            std::cout << textureRectangle.x << " " << textureRectangle.y << " " << width << " " << height << std::endl;

            textureInfo.GetRegion() = { static_cast<LONG>(textureRectangle.x),static_cast<LONG>(textureRectangle.y),
                                            static_cast<LONG>(width + textureRectangle.x), static_cast<LONG>(height + textureRectangle.y) };
            CopyTextureToAtlas(textureInfo);
        }

        D3DXSaveTextureToFile("C:\\Users\\danish\\Desktop\\myLovelyAtlas.dds", D3DXIFF_DDS, m_pAtlasTexture, NULL);
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

    report_result(result_size);
    return true;
}


bool CTextureAtlas::CreateAtlasTextureResource(float fWidth, float fHeight)
{
    IDirect3DDevice9* pDevice = CGraphics::GetSingleton().GetDevice();
    HRESULT hr = pDevice->CreateTexture(fWidth, fHeight, m_dwMaximumMipMapLevel,
                        D3DUSAGE_DYNAMIC, theFormatForAtlas, D3DPOOL_SYSTEMMEM, &m_pAtlasTexture, NULL);
    if (hr != D3D_OK)
    {
        std::printf("CreateAtlasTexture failed with error: %#.8x\n", hr);
        return false;
    }

    vecAtlasSize = CVector2D(fWidth, fHeight);
    return true;
}

bool CTextureAtlas::LoadTextureFromFile(CTextureInfo &textureInfo)
{
    IDirect3DDevice9* pDevice = CGraphics::GetSingleton().GetDevice();
    UINT const kMipLevels = D3DX_DEFAULT;
    HRESULT const hr = D3DXCreateTextureFromFileEx(pDevice, textureInfo.GetFilePath(),
        D3DX_DEFAULT, D3DX_DEFAULT, kMipLevels,
        0, D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
        &textureInfo.GetTexture());
    if (hr != D3D_OK)
    {
        std::printf("D3DXCreateTextureFromFileEx fail for '%s' with error: %#.8x\n",
            textureInfo.GetFilePath().c_str(), hr);
        return false;
    }

    D3DSURFACE_DESC desc;
    textureInfo.GetTexture()->GetLevelDesc(0, &desc);

    //remove this later
    theFormatForAtlas = desc.Format;

    // Can only pack 2D textures of known types
    if ((textureInfo.GetTexture()->GetType() != D3DRTYPE_TEXTURE) || (!IsSupportedFormat(desc.Format)))
    {
        fprintf(stderr, "*** Error: Texture %s has unsupported format.\n", textureInfo.GetFilePath().c_str());
        textureInfo.GetTexture()->Release();
        false;
    }
    std::printf("texture successfully loaded for '%s' | D3DFormat = %u\n", textureInfo.GetFilePath().c_str(), desc.Format);
    return true;
}

bool CTextureAtlas::CopyTextureToAtlas(CTextureInfo& textureInfo)
{
    IDirect3DTexture9* pTexture = textureInfo.GetTexture();
    RECT& texturePositionInAtlas = textureInfo.GetRegion();
    HRESULT         hr;
    RECT            srcRect, dstRect;
    D3DLOCKED_RECT  srcLockedRect, dstLockedRect;

    srcRect.left = srcRect.top = 0;

    // If -nomipmap was set then mpAtlas only has one mip-map and kNumMipMaps is 1.
    // If it wasn't then mpAtlas has more mip-maps then the texture and kNumMipMaps
    // is the same as there are mip-maps in pTexture.
    int const kNumMipMaps = std::min(m_pAtlasTexture->GetLevelCount(), pTexture->GetLevelCount());
    for (long mipLevel = 0; mipLevel < kNumMipMaps; ++mipLevel)
    {
        long const div = static_cast<long>(pow(2L, mipLevel));
        DWORD textureWidth = texturePositionInAtlas.right - texturePositionInAtlas.left;
        DWORD textureHeight = texturePositionInAtlas.bottom - texturePositionInAtlas.top;
        std::printf("textureWidth: %d | textureHeight: %d\n", (int)textureWidth, (int)textureHeight);
        long const mipWidth = std::max(1UL, textureWidth / div);
        long const mipHeight = std::max(1UL, textureHeight / div);

        srcRect.right = mipWidth;
        srcRect.bottom = mipHeight;

        dstRect.left = std::max(0L, texturePositionInAtlas.left / div);
        dstRect.top = std::max(0L, texturePositionInAtlas.top / div);
        dstRect.right = dstRect.left + mipWidth;
        dstRect.bottom = dstRect.top + mipHeight;

        // These calls to LockRect fail (generate errors:
        // Direct3D9: (ERROR) :Rects for DXT surfaces must be on 4x4 boundaries) 
        // in the 2003 Summer SDK Debug Runtime.  They work just fine 
        // (and as expected) when using the retail run-time: 
        // please make sure to use the retail run-time when running this!
        hr = pTexture->LockRect(mipLevel, &srcLockedRect, &srcRect, D3DLOCK_READONLY);
        if (hr != S_OK)
        {
            std::printf("pTexture->LockRect: failed to lock srcLockedRect\n");
            return false;
        }
        assert(hr == S_OK);
        hr = m_pAtlasTexture->LockRect(mipLevel, &dstLockedRect, &dstRect, 0);
        if (hr != S_OK)
        {
            std::printf("m_pAtlasTexture->LockRect: failed to lock dstLockedRect\n");
            return false;
        }
        assert(hr == S_OK);

        int const kBytesPerRow = (mipWidth * SizeOfTexel(theFormatForAtlas)) / 8;
        int const kBlockFactor = (IsDXTnFormat(theFormatForAtlas) ? 4 : 1);
        UCHAR *srcPtr = reinterpret_cast<UCHAR*>(srcLockedRect.pBits);
        UCHAR *dstPtr = reinterpret_cast<UCHAR*>(dstLockedRect.pBits);

        for (int i = 0; i < mipHeight / kBlockFactor; ++i)
        {
            memcpy(dstPtr, srcPtr, kBlockFactor * kBytesPerRow);
            srcPtr += srcLockedRect.Pitch;
            dstPtr += dstLockedRect.Pitch;
        }

        hr = pTexture->UnlockRect(mipLevel);
        assert(hr == S_OK);
        hr = m_pAtlasTexture->UnlockRect(mipLevel);
        assert(hr == S_OK);
    }
    return true;
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

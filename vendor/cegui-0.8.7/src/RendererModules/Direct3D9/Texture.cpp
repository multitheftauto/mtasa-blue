/***********************************************************************
    created:    Mon Feb 9 2009
    author:     Paul D Turner
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include <d3dx9.h>
#include "CEGUI/RendererModules/Direct3D9/Texture.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/ImageCodec.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
// helper to convert cegui pixel format enum to equivalent D3D format.
static D3DFORMAT toD3DPixelFormat(const Texture::PixelFormat fmt)
{
    switch (fmt)
    {
        case Texture::PF_RGBA:      return D3DFMT_A8R8G8B8;
        case Texture::PF_RGB:       return D3DFMT_X8R8G8B8;
        case Texture::PF_RGB_565:   return D3DFMT_R5G6B5;
        case Texture::PF_RGBA_4444: return D3DFMT_A4R4G4B4;
        case Texture::PF_RGBA_DXT1: return D3DFMT_DXT1;
        case Texture::PF_RGBA_DXT3: return D3DFMT_DXT3;
        case Texture::PF_RGBA_DXT5: return D3DFMT_DXT5;
        default:                    return D3DFMT_UNKNOWN;
    }
}

//----------------------------------------------------------------------------//
// helper function to return byte width of given pixel width in given format
static size_t calculateDataWidth(const size_t width, Texture::PixelFormat fmt)
{
    switch (fmt)
    {
    case Texture::PF_RGBA:
        return width * 4;

    case Texture::PF_RGB:
        return width * 3;

    case Texture::PF_RGB_565:
    case Texture::PF_RGBA_4444:
        return width * 2;

    case Texture::PF_RGBA_DXT1:
        return ((width + 3) / 4) * 8;

    case Texture::PF_RGBA_DXT3:
    case Texture::PF_RGBA_DXT5:
        return ((width + 3) / 4) * 16;

    default:
        return 0;
    }
}

//----------------------------------------------------------------------------//
// Helper utility function that copies an RGB buffer into a region of a second
// buffer as BGR data values (i.e swap red and blue)
static void blitRGBToBGRSurface(const uchar* src, uchar* dst, const Sizef& sz)
{
    for (uint i = 0; i < sz.d_height; ++i)
    {
        for (uint j = 0; j < sz.d_width; ++j)
        {
            *dst++ = src[2];
            *dst++ = src[1];
            *dst++ = src[0];
            src += 3;
        }
    }
}

//----------------------------------------------------------------------------//
// Helper utility function that copies an RGBA buffer into a region of a second
// buffer as D3DCOLOR data values
static void blitRGBAToD3DCOLORSurface(const uint32* src, uint32* dst,
                                      const Sizef& sz, size_t dest_pitch)
{
    for (uint i = 0; i < sz.d_height; ++i)
    {
        for (uint j = 0; j < sz.d_width; ++j)
        {
            const uint32 pixel = src[j];
            const uint32 tmp = pixel & 0x00FF00FF;
            dst[j] = (pixel & 0xFF00FF00) | (tmp << 16) | (tmp >> 16);
        }

        dst += dest_pitch / sizeof(uint32);
        src += static_cast<uint32>(sz.d_width);
    }
}

//----------------------------------------------------------------------------//
// Helper utility function that copies a region of a buffer containing D3DCOLOR
// values into a second buffer as RGBA values.
static void blitD3DCOLORSurfaceToRGBA(const uint32* src, uint32* dst,
                                      const Sizef& sz, size_t source_pitch)
{
    for (uint i = 0; i < sz.d_height; ++i)
    {
        for (uint j = 0; j < sz.d_width; ++j)
        {
            const uint32 pixel = src[j];
            const uint32 tmp = pixel & 0x00FF00FF;
            dst[j] = (pixel & 0xFF00FF00) | (tmp << 16) | (tmp >> 16);
        }

        src += source_pitch / sizeof(uint32);
        dst += static_cast<uint32>(sz.d_width);
    }
}

//----------------------------------------------------------------------------//
// Helper class to wrap an image pixel buffer.  The main purpose of this is
// to prevent the need to have lots of conditionals and raw pointers that need
// managing in places where exceptions are used (i.e this prevents leaks while
// keeping code clean).
class PixelBuffer
{
    const uchar* d_sourceBuffer;
    uchar* d_workBuffer;
    size_t d_pitch;

public:
    //------------------------------------------------------------------------//
    PixelBuffer(const void* data, const Sizef& size, Texture::PixelFormat format) :
        d_sourceBuffer(static_cast<const uchar*>(data)),
        d_workBuffer(0),
        d_pitch(calculateDataWidth(static_cast<size_t>(size.d_width), format))
    {
        if (format != Texture::PF_RGBA && format != Texture::PF_RGB)
            return;

        d_workBuffer = new uchar[d_pitch * static_cast<size_t>(size.d_height)];

        if (format == Texture::PF_RGBA)
            blitRGBAToD3DCOLORSurface(reinterpret_cast<const uint32*>(d_sourceBuffer),
                                      reinterpret_cast<uint32*>(d_workBuffer),
                                      size, d_pitch);
        else
            blitRGBToBGRSurface(d_sourceBuffer, d_workBuffer, size);
    }

    //------------------------------------------------------------------------//
    ~PixelBuffer()
    {
        delete[] d_workBuffer;
    }

    //------------------------------------------------------------------------//
    size_t getPitch() const
    {
        return d_pitch;
    }

    //------------------------------------------------------------------------//
    const uchar* getPixelDataPtr() const
    {
        return d_workBuffer ? d_workBuffer : d_sourceBuffer;
    }

    //------------------------------------------------------------------------//
};

//----------------------------------------------------------------------------//
// Internal helper class to wrap the internal handling needed to copy data to
// and from a D3D9 texture.
class D3DSurfaceBlitter
{
public:
    //------------------------------------------------------------------------//
    D3DSurfaceBlitter(LPDIRECT3DDEVICE9 d3d_device, LPDIRECT3DTEXTURE9 tex) :
        d_device(d3d_device),
        d_texture(tex),
        d_renderTarget(0),
        d_offscreen(0)
    {
        populateTextureSurfaceDescription();
    }

    //------------------------------------------------------------------------//
    ~D3DSurfaceBlitter()
    {
        cleanupSurfaces();
    }

    //------------------------------------------------------------------------//
    void blitFromMemory(const uint32* source, const Rectf& area)
    {
        if (!d_texture)
            return;

        RECT target_rect = {static_cast<LONG>(area.left()), static_cast<LONG>(area.top()),
                            static_cast<LONG>(area.right()), static_cast<LONG>(area.bottom())};

        lockRect(&target_rect);

        blitRGBAToD3DCOLORSurface(
            source, static_cast<uint32*>(d_lockedRect.pBits),
            area.getSize(), d_lockedRect.Pitch);

        unlockRect(&target_rect, true);
    }

    //------------------------------------------------------------------------//
    void blitToMemory(uint32* dest)
    {
        if (!d_texture)
            return;

        lockRect(0, true);

        blitD3DCOLORSurfaceToRGBA(
            static_cast<uint32*>(d_lockedRect.pBits), dest,
            Sizef(static_cast<float>(d_surfDesc.Width), static_cast<float>(d_surfDesc.Height)),
            d_lockedRect.Pitch);

        unlockRect();
    }

    //------------------------------------------------------------------------//
    bool isRenderTarget() const
    {
        return d_surfDesc.Usage == D3DUSAGE_RENDERTARGET;
    }

    //------------------------------------------------------------------------//

private:
    LPDIRECT3DDEVICE9 d_device;
    LPDIRECT3DTEXTURE9 d_texture;
    LPDIRECT3DSURFACE9 d_renderTarget;
    LPDIRECT3DSURFACE9 d_offscreen;
    D3DSURFACE_DESC d_surfDesc;
    D3DLOCKED_RECT d_lockedRect;
    RECT d_fullArea;

    //------------------------------------------------------------------------//
    void initialiseSurfaces()
    {
        if (!d_offscreen)
            createOffscreenSurface();

        if (!d_renderTarget)
            getTextureSurface();
    }

    //------------------------------------------------------------------------//
    void cleanupSurfaces()
    {
        if (d_renderTarget)
        {
            d_renderTarget->Release();
            d_renderTarget = 0;
        }

        if (d_offscreen)
        {
            d_offscreen->Release();
            d_offscreen = 0;
        }
    }

    //------------------------------------------------------------------------//
    void lockRect(RECT* area = 0, bool for_reading = false)
    {
        if (isRenderTarget())
        {
            initialiseSurfaces();

            if (for_reading)
                getRenderTargetData();

            lockSurfaceRect(area);
        }
        else
        {
            lockTextureRect(area);
        }
    }

    //------------------------------------------------------------------------//
    void unlockRect(RECT* area = 0, bool needs_update = false)
    {
        if (isRenderTarget())
        {
            d_offscreen->UnlockRect();

            if (needs_update)
                updateRenderTarget(area);
        }
        else
            d_texture->UnlockRect(0);
    }

    //------------------------------------------------------------------------//
    void populateTextureSurfaceDescription()
    {
        if (FAILED(d_texture->GetLevelDesc(0, &d_surfDesc)))
            CEGUI_THROW(RendererException(
                "IDirect3DTexture9::GetLevelDesc failed."));

        d_fullArea.left = 0;
        d_fullArea.top = 0;
        d_fullArea.right = d_surfDesc.Width;
        d_fullArea.bottom = d_surfDesc.Height;
    }

    //------------------------------------------------------------------------//
    void createOffscreenSurface()
    {
        if (FAILED(d_device->CreateOffscreenPlainSurface(
            d_surfDesc.Width, d_surfDesc.Height, d_surfDesc.Format,
            D3DPOOL_SYSTEMMEM, &d_offscreen, 0)))
        {
            CEGUI_THROW(RendererException(
                "IDirect3DDevice9::CreateOffscreenPlainSurface failed."));
        }
    }

    //------------------------------------------------------------------------//
    void getTextureSurface()
    {
        if (FAILED(d_texture->GetSurfaceLevel(0, &d_renderTarget)))
            CEGUI_THROW(RendererException(
                "IDirect3DTexture9::GetSurfaceLevel failed."));
    }

    //------------------------------------------------------------------------//
    void lockSurfaceRect(RECT* area)
    {
        if (FAILED(d_offscreen->LockRect(&d_lockedRect, area, 0)))
            CEGUI_THROW(RendererException(
                "IDirect3DSurface9::LockRect failed."));
    }

    //------------------------------------------------------------------------//
    void lockTextureRect(RECT* area)
    {
        if (FAILED(d_texture->LockRect(0, &d_lockedRect, area, 0)))
            CEGUI_THROW(RendererException(
                "IDirect3DTexture9::LockRect failed."));
    }

    //------------------------------------------------------------------------//
    void updateRenderTarget(RECT* area)
    {
        POINT pt = {area ? area->left : 0, area ? area->top : 0};
        if (FAILED(d_device->UpdateSurface(d_offscreen,
                                           area ? area : &d_fullArea,
                                           d_renderTarget, &pt)))
        {
            CEGUI_THROW(RendererException(
                "IDirect3DDevice9::UpdateSurface failed."));
        }
    }

    //------------------------------------------------------------------------//
    void getRenderTargetData()
    {
        if (FAILED(d_device->GetRenderTargetData(d_renderTarget, d_offscreen)))
            CEGUI_THROW(RendererException(
                "IDirect3DDevice9::GetRenderTargetData failed."));
    }

    //------------------------------------------------------------------------//
};

//----------------------------------------------------------------------------//
Direct3D9Texture::Direct3D9Texture(Direct3D9Renderer& owner,
                                   const String& name) :
    d_owner(owner),
    d_texture(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_savedSurfaceDescValid(false),
    d_name(name)
{
}

//----------------------------------------------------------------------------//
Direct3D9Texture::Direct3D9Texture(Direct3D9Renderer& owner,
                                   const String& name,
                                   const String& filename,
                                   const String& resourceGroup) :
    d_owner(owner),
    d_texture(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_savedSurfaceDescValid(false),
    d_name(name)
{
    loadFromFile(filename, resourceGroup);
}

//----------------------------------------------------------------------------//
Direct3D9Texture::Direct3D9Texture(Direct3D9Renderer& owner,
                                   const String& name, const Sizef& sz) :
    d_owner(owner),
    d_texture(0),
    d_size(0, 0),
    d_dataSize(sz),
    d_texelScaling(0, 0),
    d_savedSurfaceDescValid(false),
    d_name(name)
{
    createDirect3D9Texture(sz, D3DFMT_A8R8G8B8);
}

//----------------------------------------------------------------------------//
Direct3D9Texture::Direct3D9Texture(Direct3D9Renderer& owner,
                                   const String& name,
                                   LPDIRECT3DTEXTURE9 tex) :
    d_owner(owner),
    d_texture(0),
    d_size(0, 0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0),
    d_savedSurfaceDescValid(false),
    d_name(name)
{
    setDirect3D9Texture(tex);
}

//----------------------------------------------------------------------------//
Direct3D9Texture::~Direct3D9Texture()
{
    cleanupDirect3D9Texture();
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::setDirect3D9Texture(LPDIRECT3DTEXTURE9 tex)
{
    if (d_texture != tex)
    {
        cleanupDirect3D9Texture();
        d_dataSize.d_width = d_dataSize.d_height = 0;

        d_texture = tex;
        if (d_texture)
            d_texture->AddRef();
    }

    updateTextureSize();
    d_dataSize = d_size;
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
LPDIRECT3DTEXTURE9 Direct3D9Texture::getDirect3D9Texture() const
{
    return d_texture;
}

//----------------------------------------------------------------------------//
const String& Direct3D9Texture::getName() const
{
    return d_name;
}

//----------------------------------------------------------------------------//
const Sizef& Direct3D9Texture::getSize() const
{
    return d_size;
}

//----------------------------------------------------------------------------//
const Sizef& Direct3D9Texture::getOriginalDataSize() const
{
    return d_dataSize;
}

//----------------------------------------------------------------------------//
const Vector2f& Direct3D9Texture::getTexelScaling() const
{
    return d_texelScaling;
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::loadFromFile(const String& filename,
                                    const String& resourceGroup)
{
    // get and check existence of CEGUI::System object
    System* sys = System::getSingletonPtr();
    if (!sys)
        CEGUI_THROW(RendererException(
            "CEGUI::System object has not been created!"));

    // load file to memory via resource provider
    RawDataContainer texFile;
    sys->getResourceProvider()->loadRawDataContainer(filename, texFile,
            resourceGroup);

    Texture* res = sys->getImageCodec().load(texFile, this);

    // unload file data buffer
    sys->getResourceProvider()->unloadRawDataContainer(texFile);

    if (!res)
        // It's an error
        CEGUI_THROW(RendererException(
            sys->getImageCodec().getIdentifierString() +
            " failed to load image '" + filename + "'."));
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::loadFromMemory(const void* buffer,
                                      const Sizef& buffer_size,
                                      PixelFormat pixel_format)
{
    if (!isPixelFormatSupported(pixel_format))
        CEGUI_THROW(InvalidRequestException(
            "Data was supplied in an unsupported pixel format."));

    const D3DFORMAT pixfmt = toD3DPixelFormat(pixel_format);
    createDirect3D9Texture(buffer_size, pixfmt);

    LPDIRECT3DSURFACE9 surface = getTextureSurface();
    const PixelBuffer pixel_buffer(buffer, buffer_size, pixel_format);

    const RECT src_rect = { 0, 0,
        static_cast<LONG>(buffer_size.d_width),
        static_cast<LONG>(buffer_size.d_height) };

    HRESULT hr = D3DXLoadSurfaceFromMemory(
            surface, 0, 0, pixel_buffer.getPixelDataPtr(),
            pixfmt == D3DFMT_X8R8G8B8 ? D3DFMT_R8G8B8 : pixfmt,
            pixel_buffer.getPitch(), 0, &src_rect, D3DX_FILTER_NONE, 0);

    surface->Release();

    if (FAILED(hr))
        CEGUI_THROW(RendererException(
            "D3DXLoadSurfaceFromMemory failed."));
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::createDirect3D9Texture(const Sizef sz, D3DFORMAT format)
{
    cleanupDirect3D9Texture();

    const Sizef tex_sz(d_owner.getAdjustedSize(sz));

    HRESULT hr = D3DXCreateTexture(d_owner.getDevice(),
                                   static_cast<UINT>(tex_sz.d_width),
                                   static_cast<UINT>(tex_sz.d_height),
                                   1, 0, format, D3DPOOL_MANAGED, &d_texture);

    if (FAILED(hr))
        CEGUI_THROW(RendererException("D3DXCreateTexture failed."));

    d_dataSize = sz;
    updateTextureSize();
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
IDirect3DSurface9* Direct3D9Texture::getTextureSurface() const
{
    LPDIRECT3DSURFACE9 surface;
    HRESULT hr = d_texture->GetSurfaceLevel(0, &surface);

    if (FAILED(hr))
        CEGUI_THROW(RendererException(
            "IDirect3DTexture9::GetSurfaceLevel failed."));

    return surface;
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::blitFromMemory(const void* sourceData, const Rectf& area)
{
    D3DSurfaceBlitter blitter(d_owner.getDevice(), d_texture);
    blitter.blitFromMemory(static_cast<const uint32*>(sourceData), area);
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::blitToMemory(void* targetData)
{
    D3DSurfaceBlitter blitter(d_owner.getDevice(), d_texture);
    blitter.blitToMemory(static_cast<uint32*>(targetData));
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::cleanupDirect3D9Texture()
{
    if (d_texture)
    {
        d_texture->Release();
        d_texture = 0;
    }
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::updateCachedScaleValues()
{
    //
    // calculate what to use for x scale
    //
    const float orgW = d_dataSize.d_width;
    const float texW = d_size.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = d_dataSize.d_height;
    const float texH = d_size.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::updateTextureSize()
{
    D3DSURFACE_DESC surfDesc;

    // obtain details of the size of the texture
    if (d_texture && SUCCEEDED(d_texture->GetLevelDesc(0, &surfDesc)))
    {
        d_size.d_width  = static_cast<float>(surfDesc.Width);
        d_size.d_height = static_cast<float>(surfDesc.Height);
    }
    // use the original size if query failed.
    // NB: This should probably be an exception.
    else
        d_size = d_dataSize;
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::setOriginalDataSize(const Sizef& sz)
{
    d_dataSize = sz;
    updateCachedScaleValues();
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::preD3DReset()
{
    // if already saved surface info, or we have no texture, do nothing
    if (d_savedSurfaceDescValid || !d_texture)
        return;

    // get info about our texture
    d_texture->GetLevelDesc(0, &d_savedSurfaceDesc);

    // if texture is managed, we have nothing more to do
    if (d_savedSurfaceDesc.Pool == D3DPOOL_MANAGED)
        return;

    // otherwise release texture.
    d_texture->Release();
    d_texture = 0;
    d_savedSurfaceDescValid = true;
}

//----------------------------------------------------------------------------//
void Direct3D9Texture::postD3DReset()
{
    // if texture has no saved surface info, we do nothing.
    if (!d_savedSurfaceDescValid)
        return;

    // otherwise, create a new texture using saved details.
    d_owner.getDevice()->
        CreateTexture(d_savedSurfaceDesc.Width,
                      d_savedSurfaceDesc.Height,
                      1, d_savedSurfaceDesc.Usage, d_savedSurfaceDesc.Format,
                      d_savedSurfaceDesc.Pool, &d_texture, 0);

    d_savedSurfaceDescValid = false;
}

//----------------------------------------------------------------------------//
bool Direct3D9Texture::isPixelFormatSupported(const PixelFormat fmt) const
{
    D3DDEVICE_CREATION_PARAMETERS dev_params;
    d_owner.getDevice()->GetCreationParameters(&dev_params);

    LPDIRECT3D9 d3d;
    d_owner.getDevice()->GetDirect3D(&d3d);

    D3DDISPLAYMODE dmode;
    d3d->GetAdapterDisplayMode(dev_params.AdapterOrdinal, &dmode);

    const D3DFORMAT d3d_format = toD3DPixelFormat(fmt);

    if (d3d_format == D3DFMT_UNKNOWN)
        return false;

    return SUCCEEDED(d3d->CheckDeviceFormat(
        dev_params.AdapterOrdinal, dev_params.DeviceType,
        dmode.Format, 0, D3DRTYPE_TEXTURE, d3d_format));
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

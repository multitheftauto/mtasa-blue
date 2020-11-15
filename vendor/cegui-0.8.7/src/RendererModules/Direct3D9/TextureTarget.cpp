/***********************************************************************
    created:    Fri Feb 13 2009
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
#include "CEGUI/RendererModules/Direct3D9/TextureTarget.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/RenderQueue.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/RendererModules/Direct3D9/Renderer.h"
#include "CEGUI/RendererModules/Direct3D9/Texture.h"
#include "CEGUI/PropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const float Direct3D9TextureTarget::DEFAULT_SIZE = 128.0f;
uint Direct3D9TextureTarget::s_textureNumber = 0;

//----------------------------------------------------------------------------//
Direct3D9TextureTarget::Direct3D9TextureTarget(Direct3D9Renderer& owner) :
    Direct3D9RenderTarget<TextureTarget>(owner),
    d_texture(0),
    d_surface(0)
{
    // this essentially creates a 'null' CEGUI::Texture
    d_CEGUITexture = &static_cast<Direct3D9Texture&>(
        d_owner.createTexture(generateTextureName(), 0));

    // setup area and cause the initial texture to be generated.
    declareRenderSize(Sizef(DEFAULT_SIZE, DEFAULT_SIZE));
}

//----------------------------------------------------------------------------//
Direct3D9TextureTarget::~Direct3D9TextureTarget()
{
    cleanupRenderTexture();
    d_owner.destroyTexture(*d_CEGUITexture);
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::declareRenderSize(const Sizef& sz)
{
    // exit if current size is enough
    if ((d_area.getWidth() >= sz.d_width) && (d_area.getHeight() >=sz.d_height))
        return;

    setArea(Rectf(d_area.getPosition(), sz));
    resizeRenderTexture();
    clear();
}

//----------------------------------------------------------------------------//
bool Direct3D9TextureTarget::isImageryCache() const
{
    return true;
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::activate()
{
    enableRenderTexture();
    Direct3D9RenderTarget::activate();
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::deactivate()
{
    Direct3D9RenderTarget::deactivate();
    disableRenderTexture();
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::clear()
{
    // switch to targetting our texture
    enableRenderTexture();
    // Clear it.
    d_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    // switch back to rendering to previous target
    disableRenderTexture();
}

//----------------------------------------------------------------------------//
Texture& Direct3D9TextureTarget::getTexture() const
{
    return *d_CEGUITexture;
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::initialiseRenderTexture()
{
    Sizef tex_sz(d_owner.getAdjustedSize(d_area.getSize()));

    d_device->CreateTexture(static_cast<UINT>(tex_sz.d_width),
                            static_cast<UINT>(tex_sz.d_height),
                            1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                            D3DPOOL_DEFAULT, &d_texture, 0);

    d_texture->GetSurfaceLevel(0, &d_surface);

    // wrap the created texture with the CEGUI::Texture
    d_CEGUITexture->setDirect3D9Texture(d_texture);
    d_CEGUITexture->setOriginalDataSize(d_area.getSize());
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::resizeRenderTexture()
{
    cleanupRenderTexture();
    initialiseRenderTexture();
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::cleanupRenderTexture()
{
    if (d_surface)
    {
        d_surface->Release();
        d_surface = 0;
    }
    if (d_texture)
    {
        d_CEGUITexture->setDirect3D9Texture(0);
        d_texture->Release();
        d_texture = 0;
    }
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::enableRenderTexture()
{
    LPDIRECT3DSURFACE9 oldSurface = 0;
    d_device->GetRenderTarget(0, &oldSurface);

    if (oldSurface && oldSurface != d_surface)
    {
        d_prevColourSurface = oldSurface;
        d_device->SetRenderTarget(0, d_surface);
    }
    else if (oldSurface)
        oldSurface->Release();
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::disableRenderTexture()
{
    d_device->SetRenderTarget(0, d_prevColourSurface);

    if (d_prevColourSurface)
    {
        d_prevColourSurface->Release();
        d_prevColourSurface = 0;
    }
}

//----------------------------------------------------------------------------//
bool Direct3D9TextureTarget::isRenderingInverted() const
{
    return false;
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::preD3DReset()
{
    if (d_surface)
    {
        d_surface->Release();
        d_surface = 0;
    }

    if (d_CEGUITexture)
        d_CEGUITexture->preD3DReset();

    if (d_texture)
    {
        d_texture->Release();
        d_texture = 0;
    }
}

//----------------------------------------------------------------------------//
void Direct3D9TextureTarget::postD3DReset()
{
    if (!d_CEGUITexture)
        return;

    // this will recreate the texture
    d_CEGUITexture->postD3DReset();
    // we now obtain a reference to that created texture
    d_texture = d_CEGUITexture->getDirect3D9Texture();
    if (d_texture)
    {
        d_texture->AddRef();
        // now obtain the surface
        d_texture->GetSurfaceLevel(0, &d_surface);
    }
}

//----------------------------------------------------------------------------//
String Direct3D9TextureTarget::generateTextureName()
{
    String tmp("_d3d9_tt_tex_");
    tmp.append(PropertyHelper<uint>::toString(s_textureNumber++));

    return tmp;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

//----------------------------------------------------------------------------//
// Implementation of template base class
#include "./RenderTarget.inl"


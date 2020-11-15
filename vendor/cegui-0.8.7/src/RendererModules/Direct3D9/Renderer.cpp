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
#include "CEGUI/RendererModules/Direct3D9/Renderer.h"
#include "CEGUI/RendererModules/Direct3D9/Texture.h"
#include "CEGUI/RendererModules/Direct3D9/GeometryBuffer.h"
#include "CEGUI/RendererModules/Direct3D9/RenderTarget.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/RendererModules/Direct3D9/ViewportTarget.h"
#include "CEGUI/RendererModules/Direct3D9/TextureTarget.h"
#include "CEGUI/System.h"
#include "CEGUI/DefaultResourceProvider.h"
#include "CEGUI/Logger.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
String Direct3D9Renderer::d_rendererID(
"CEGUI::Direct3D9Renderer - Official Direct3D 9 based 2nd generation renderer "
"module.");

//----------------------------------------------------------------------------//
static const D3DMATRIX s_identityMatrix =
{
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

//----------------------------------------------------------------------------//
Direct3D9Renderer& Direct3D9Renderer::bootstrapSystem(LPDIRECT3DDEVICE9 device,
                                                      const int abi)
{
    System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

    if (System::getSingletonPtr())
        CEGUI_THROW(InvalidRequestException(
            "CEGUI::System object is already initialised."));

    Direct3D9Renderer& renderer(create(device));
    DefaultResourceProvider* rp = new CEGUI::DefaultResourceProvider();
    System::create(renderer, rp);

    return renderer;
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroySystem()
{
    System* sys;
    if (!(sys = System::getSingletonPtr()))
        CEGUI_THROW(InvalidRequestException(
            "CEGUI::System object is not created or was already destroyed."));

    Direct3D9Renderer* renderer =
        static_cast<Direct3D9Renderer*>(sys->getRenderer());
    DefaultResourceProvider* rp =
        static_cast<DefaultResourceProvider*>(sys->getResourceProvider());

    System::destroy();
    delete rp;
    destroy(*renderer);
}

//----------------------------------------------------------------------------//
Direct3D9Renderer& Direct3D9Renderer::create(LPDIRECT3DDEVICE9 device,
                                             const int abi)
{
    System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

    return *new Direct3D9Renderer(device);
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroy(Direct3D9Renderer& renderer)
{
    delete &renderer;
}

//----------------------------------------------------------------------------//
RenderTarget& Direct3D9Renderer::getDefaultRenderTarget()
{
    return *d_defaultTarget;
}

//----------------------------------------------------------------------------//
GeometryBuffer& Direct3D9Renderer::createGeometryBuffer()
{
    Direct3D9GeometryBuffer* b = new Direct3D9GeometryBuffer(*this, d_device);
    d_geometryBuffers.push_back(b);
    return *b;
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyGeometryBuffer(const GeometryBuffer& buffer)
{
    GeometryBufferList::iterator i = std::find(d_geometryBuffers.begin(),
                                     d_geometryBuffers.end(),
                                     &buffer);

    if (d_geometryBuffers.end() != i)
    {
        d_geometryBuffers.erase(i);
        delete &buffer;
    }
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyAllGeometryBuffers()
{
    while (!d_geometryBuffers.empty())
        destroyGeometryBuffer(**d_geometryBuffers.begin());
}

//----------------------------------------------------------------------------//
TextureTarget* Direct3D9Renderer::createTextureTarget()
{
    TextureTarget* t = new Direct3D9TextureTarget(*this);
    d_textureTargets.push_back(t);
    return t;
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyTextureTarget(TextureTarget* target)
{
    TextureTargetList::iterator i = std::find(d_textureTargets.begin(),
                                    d_textureTargets.end(),
                                    target);

    if (d_textureTargets.end() != i)
    {
        d_textureTargets.erase(i);
        delete target;
    }
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyAllTextureTargets()
{
    while (!d_textureTargets.empty())
        destroyTextureTarget(*d_textureTargets.begin());
}

//----------------------------------------------------------------------------//
Texture& Direct3D9Renderer::createTexture(const String& name)
{
    throwIfNameExists(name);

    Direct3D9Texture* tex = new Direct3D9Texture(*this, name);
    d_textures[name] = tex;

    logTextureCreation(name);

    return *tex;
}

//----------------------------------------------------------------------------//
Texture& Direct3D9Renderer::createTexture(const String& name,
                                          const String& filename,
                                          const String& resourceGroup)
{
    throwIfNameExists(name);

    Direct3D9Texture* tex =
        new Direct3D9Texture(*this, name, filename, resourceGroup);
    d_textures[name] = tex;

    logTextureCreation(name);

    return *tex;
}

//----------------------------------------------------------------------------//
Texture& Direct3D9Renderer::createTexture(const String& name, const Sizef& size)
{
    throwIfNameExists(name);

    Direct3D9Texture* tex = new Direct3D9Texture(*this, name, size);
    d_textures[name] = tex;

    logTextureCreation(name);

    return *tex;
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::throwIfNameExists(const String& name) const
{
    if (d_textures.find(name) != d_textures.end())
        CEGUI_THROW(AlreadyExistsException(
            "[Direct3D9Renderer] Texture already exists: " + name));
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::logTextureCreation(const String& name)
{
    Logger* logger = Logger::getSingletonPtr();
    if (logger)
        logger->logEvent("[Direct3D9Renderer] Created texture: " + name);
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyTexture(Texture& texture)
{
    destroyTexture(texture.getName());
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyTexture(const String& name)
{
    TextureMap::iterator i = d_textures.find(name);

    if (d_textures.end() != i)
    {
        logTextureDestruction(name);
        delete i->second;
        d_textures.erase(i);
    }
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::logTextureDestruction(const String& name)
{
    Logger* logger = Logger::getSingletonPtr();
    if (logger)
        logger->logEvent("[Direct3D9Renderer] Destroyed texture: " + name);
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::destroyAllTextures()
{
    while (!d_textures.empty())
        destroyTexture(d_textures.begin()->first);
}

//----------------------------------------------------------------------------//
Texture& Direct3D9Renderer::getTexture(const String& name) const
{
    TextureMap::const_iterator i = d_textures.find(name);
    
    if (i == d_textures.end())
        CEGUI_THROW(UnknownObjectException(
            "[Direct3D9Renderer] Texture does not exist: " + name));

    return *i->second;
}

//----------------------------------------------------------------------------//
bool Direct3D9Renderer::isTextureDefined(const String& name) const
{
    return d_textures.find(name) != d_textures.end();
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::beginRendering()
{
    d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

    // no shaders initially
    d_device->SetVertexShader(0);
    d_device->SetPixelShader(0);

    // set device states
    d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
    d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    // setup texture addressing settings
    d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // setup colour calculations
    d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    // setup alpha calculations
    d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    // setup filtering
    d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // disable texture stages we do not need.
    d_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    // setup scene alpha blending
    d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    // put alpha blend operations into a known state
    setupRenderingBlendMode(BM_NORMAL, true);

    // set view matrix back to identity.
    d_device->SetTransform(D3DTS_VIEW, &s_identityMatrix);
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::endRendering()
{
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::setDisplaySize(const Sizef& sz)
{
    if (sz != d_displaySize)
    {
        d_displaySize = sz;

        // FIXME: This is probably not the right thing to do in all cases.
        Rectf area(d_defaultTarget->getArea());
        area.setSize(sz);
        d_defaultTarget->setArea(area);
    }
}

//----------------------------------------------------------------------------//
const Sizef& Direct3D9Renderer::getDisplaySize() const
{
    return d_displaySize;
}

//----------------------------------------------------------------------------//
const Vector2f& Direct3D9Renderer::getDisplayDPI() const
{
    return d_displayDPI;
}

//----------------------------------------------------------------------------//
uint Direct3D9Renderer::getMaxTextureSize() const
{
    return d_maxTextureSize;
}

//----------------------------------------------------------------------------//
const String& Direct3D9Renderer::getIdentifierString() const
{
    return d_rendererID;
}

//----------------------------------------------------------------------------//
Direct3D9Renderer::Direct3D9Renderer(LPDIRECT3DDEVICE9 device) :
    d_device(device),
    d_displaySize(getViewportSize()),
    d_displayDPI(96, 96),
    d_defaultTarget(0)
{
    D3DCAPS9 caps;
    device->GetDeviceCaps(&caps);

    if (!caps.RasterCaps && D3DPRASTERCAPS_SCISSORTEST)
        CEGUI_THROW(RendererException(
            "Hardware does not support D3DPRASTERCAPS_SCISSORTEST. "
            "Unable to proceed."));

    d_maxTextureSize = ceguimin(caps.MaxTextureHeight, caps.MaxTextureWidth);

    d_supportNonSquareTex = !(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY);

    d_supportNPOTTex = !(caps.TextureCaps & D3DPTEXTURECAPS_POW2) ||
                       (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);

    d_defaultTarget = new Direct3D9ViewportTarget(*this);
}

//----------------------------------------------------------------------------//
Direct3D9Renderer::~Direct3D9Renderer()
{
    destroyAllGeometryBuffers();
    destroyAllTextureTargets();
    destroyAllTextures();

    delete d_defaultTarget;
}

//----------------------------------------------------------------------------//
Sizef Direct3D9Renderer::getViewportSize()
{
    D3DVIEWPORT9 vp;

    if (FAILED(d_device->GetViewport(&vp)))
        CEGUI_THROW(RendererException(
            "Unable to access required view port information from "
            "Direct3DDevice9."));
    else
        return Sizef(static_cast<float>(vp.Width),
                      static_cast<float>(vp.Height));
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::preD3DReset()
{
    // perform pre-reset on texture targets
    TextureTargetList::iterator target_iterator = d_textureTargets.begin();
    for (; target_iterator != d_textureTargets.end(); ++target_iterator)
        static_cast<Direct3D9TextureTarget*>(*target_iterator)->preD3DReset();

    // perform pre-reset on textures
    TextureMap::iterator texture_iterator = d_textures.begin();
    for (; texture_iterator != d_textures.end(); ++texture_iterator)
        texture_iterator->second->preD3DReset();
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::postD3DReset()
{
    // perform post-reset on textures
    TextureMap::iterator texture_iterator = d_textures.begin();
    for (; texture_iterator != d_textures.end(); ++texture_iterator)
        texture_iterator->second->postD3DReset();

    // perform post-reset on texture targets
    TextureTargetList::iterator target_iterator = d_textureTargets.begin();
    for (; target_iterator != d_textureTargets.end(); ++target_iterator)
        static_cast<Direct3D9TextureTarget*>(*target_iterator)->postD3DReset();

    // notify system about the (possibly) new viewport size.
    System::getSingleton().notifyDisplaySizeChanged(getViewportSize());
}

//----------------------------------------------------------------------------//
LPDIRECT3DDEVICE9 Direct3D9Renderer::getDevice() const
{
    return d_device;
}

//----------------------------------------------------------------------------//
Texture& Direct3D9Renderer::createTexture(const String& name,
                                          LPDIRECT3DTEXTURE9 texture)
{
    throwIfNameExists(name);

    Direct3D9Texture* tex = new Direct3D9Texture(*this, name, texture);
    d_textures[name] = tex;

    logTextureCreation(name);

    return *tex;
}

//----------------------------------------------------------------------------//
bool Direct3D9Renderer::supportsNonSquareTexture()
{
    return d_supportNonSquareTex;
}

//----------------------------------------------------------------------------//
bool Direct3D9Renderer::supportsNPOTTextures()
{
    return d_supportNPOTTex;
}

//----------------------------------------------------------------------------//
Sizef Direct3D9Renderer::getAdjustedSize(const Sizef& sz)
{
    Sizef s(sz);

    if (!d_supportNPOTTex)
    {
        s.d_width  = getSizeNextPOT(sz.d_width);
        s.d_height = getSizeNextPOT(sz.d_height);
    }
    if (!d_supportNonSquareTex)
        s.d_width = s.d_height =
                        ceguimax(s.d_width, s.d_height);

    return s;
}

//----------------------------------------------------------------------------//
float Direct3D9Renderer::getSizeNextPOT(float sz) const
{
    uint size = static_cast<uint>(sz);

    // if not power of 2
    if ((size & (size - 1)) || !size)
    {
        int log = 0;

        // get integer log of 'size' to base 2
        while (size >>= 1)
            ++log;

        // use log to calculate value to use as size.
        size = (2 << log);
    }

    return static_cast<float>(size);
}

//----------------------------------------------------------------------------//
void Direct3D9Renderer::setupRenderingBlendMode(const BlendMode mode,
                                                const bool force)
{
    // exit if no change (and setup not forced)
    if ((d_activeBlendMode == mode) && !force)
        return;

    d_activeBlendMode = mode;

    if (d_activeBlendMode == BM_RTT_PREMULTIPLIED)
    {
        d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
        d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    }
    else
    {
        d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
        d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        d_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
        d_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
    }
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

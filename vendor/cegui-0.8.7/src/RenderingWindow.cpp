/***********************************************************************
    created:    Mon Jan 12 2009
    author:     Paul D Turner
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/RenderingWindow.h"
#include "CEGUI/TextureTarget.h"
#include "CEGUI/System.h"
#include "CEGUI/Renderer.h"
#include "CEGUI/Vertex.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/Texture.h"
#include "CEGUI/RenderEffect.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
RenderingWindow::RenderingWindow(TextureTarget& target, RenderingSurface& owner) :
    RenderingSurface(target),
    d_renderer(*System::getSingleton().getRenderer()),
    d_textarget(target),
    d_owner(&owner),
    d_geometry(&d_renderer.createGeometryBuffer()),
    d_geometryValid(false),
    d_position(0, 0),
    d_size(0, 0),
    d_rotation(Quaternion::IDENTITY)
{
    d_geometry->setBlendMode(BM_RTT_PREMULTIPLIED);
}

//----------------------------------------------------------------------------//
RenderingWindow::~RenderingWindow()
{
    d_renderer.destroyGeometryBuffer(*d_geometry);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setClippingRegion(const Rectf& region)
{
    Rectf final_region(region);

    // clip region position must be offset according to our owner position, if
    // that is a RenderingWindow.
    if (d_owner->isRenderingWindow())
    {
        final_region.offset(
            Vector2f(-static_cast<RenderingWindow*>(d_owner)->d_position.d_x,
                      -static_cast<RenderingWindow*>(d_owner)->d_position.d_y));
    }

    d_geometry->setClippingRegion(final_region);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setPosition(const Vector2f& position)
{
    d_position = position;

    Vector3f trans(d_position.d_x, d_position.d_y, 0.0f);
    // geometry position must be offset according to our owner position, if
    // that is a RenderingWindow.
    if (d_owner->isRenderingWindow())
    {
        trans.d_x -= static_cast<RenderingWindow*>(d_owner)->d_position.d_x;
        trans.d_y -= static_cast<RenderingWindow*>(d_owner)->d_position.d_y;
    }

    d_geometry->setTranslation(trans);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setSize(const Sizef& size)
{
    // URGENT FIXME: Isn't this in the hands of the user?
    /*d_size.d_width = PixelAligned(size.d_width);
    d_size.d_height = PixelAligned(size.d_height);*/
    d_size = size;
    d_geometryValid = false;

    d_textarget.declareRenderSize(d_size);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setRotation(const Quaternion& rotation)
{
    d_rotation = rotation;
    d_geometry->setRotation(d_rotation);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setPivot(const Vector3f& pivot)
{
    d_pivot = pivot;
    d_geometry->setPivot(d_pivot);
}

//----------------------------------------------------------------------------//
const Vector2f& RenderingWindow::getPosition() const
{
    return d_position;
}

//----------------------------------------------------------------------------//
const Sizef& RenderingWindow::getSize() const
{
    return d_size;
}

//----------------------------------------------------------------------------//
const Quaternion& RenderingWindow::getRotation() const
{
    return d_rotation;
}

//----------------------------------------------------------------------------//
const Vector3f& RenderingWindow::getPivot() const
{
    return d_pivot;
}

//----------------------------------------------------------------------------//
const TextureTarget& RenderingWindow::getTextureTarget() const
{
    return d_textarget;
}

//----------------------------------------------------------------------------//
TextureTarget& RenderingWindow::getTextureTarget()
{
    return const_cast<TextureTarget&>(
        static_cast<const RenderingWindow*>(this)->getTextureTarget());
}

//----------------------------------------------------------------------------//
void RenderingWindow::update(const float elapsed)
{
    RenderEffect* effect = d_geometry->getRenderEffect();

    if (effect)
        d_geometryValid &= effect->update(elapsed, *this);
}

//----------------------------------------------------------------------------//
void RenderingWindow::setRenderEffect(RenderEffect* effect)
{
    d_geometry->setRenderEffect(effect);
}

//----------------------------------------------------------------------------//
RenderEffect* RenderingWindow::getRenderEffect()
{
    return d_geometry->getRenderEffect();
}

//----------------------------------------------------------------------------//
const RenderingSurface& RenderingWindow::getOwner() const
{
    return *d_owner;
}

//----------------------------------------------------------------------------//
RenderingSurface& RenderingWindow::getOwner()
{
    return const_cast<RenderingSurface&>(
        static_cast<const RenderingWindow*>(this)->getOwner());
}

//----------------------------------------------------------------------------//
void RenderingWindow::setOwner(RenderingSurface& owner)
{
    d_owner = &owner;
}

//----------------------------------------------------------------------------//
void RenderingWindow::draw()
{
    // update geometry if needed.
    if (!d_geometryValid)
        realiseGeometry();

    if (d_invalidated)
    {
        // base class will render out queues for us
        RenderingSurface::draw();
        // mark as no longer invalidated
        d_invalidated = false;
    }

    // add our geometry to our owner for rendering
    d_owner->addGeometryBuffer(RQ_BASE, *d_geometry);
}

//----------------------------------------------------------------------------//
void RenderingWindow::invalidate()
{
    // this override is potentially expensive, so only do the main work when we
    // have to.
    if (!d_invalidated)
    {
        RenderingSurface::invalidate();
        d_textarget.clear();
    }

    // also invalidate what we render back to.
    d_owner->invalidate();
}

//----------------------------------------------------------------------------//
bool RenderingWindow::isRenderingWindow() const
{
    return true;
}

//----------------------------------------------------------------------------//
void RenderingWindow::realiseGeometry()
{
    if (d_geometryValid)
        return;

    d_geometry->reset();

    RenderEffect* effect = d_geometry->getRenderEffect();

    if (!effect || effect->realiseGeometry(*this, *d_geometry))
        realiseGeometry_impl();

    d_geometryValid = true;
 }

//----------------------------------------------------------------------------//
void RenderingWindow::realiseGeometry_impl()
{
   Texture& tex = d_textarget.getTexture();

    const float tu = d_size.d_width * tex.getTexelScaling().d_x;
    const float tv = d_size.d_height * tex.getTexelScaling().d_y;
    const Rectf tex_rect(d_textarget.isRenderingInverted() ?
                          Rectf(0, 1, tu, 1 - tv) :
                          Rectf(0, 0, tu, tv));

    const Rectf area(0, 0, d_size.d_width, d_size.d_height);
    const Colour c(1, 1, 1, 1);
    Vertex vbuffer[6];

    // vertex 0
    vbuffer[0].position   = Vector3f(area.d_min.d_x, area.d_min.d_y, 0.0f);
    vbuffer[0].colour_val = c;
    vbuffer[0].tex_coords = Vector2f(tex_rect.d_min.d_x, tex_rect.d_min.d_y);

    // vertex 1
    vbuffer[1].position   = Vector3f(area.d_min.d_x, area.d_max.d_y, 0.0f);
    vbuffer[1].colour_val = c;
    vbuffer[1].tex_coords = Vector2f(tex_rect.d_min.d_x, tex_rect.d_max.d_y);

    // vertex 2
    vbuffer[2].position   = Vector3f(area.d_max.d_x, area.d_max.d_y, 0.0f);
    vbuffer[2].colour_val = c;
    vbuffer[2].tex_coords = Vector2f(tex_rect.d_max.d_x, tex_rect.d_max.d_y);

    // vertex 3
    vbuffer[3].position   = Vector3f(area.d_max.d_x, area.d_min.d_y, 0.0f);
    vbuffer[3].colour_val = c;
    vbuffer[3].tex_coords = Vector2f(tex_rect.d_max.d_x, tex_rect.d_min.d_y);

    // vertex 4
    vbuffer[4].position   = Vector3f(area.d_min.d_x, area.d_min.d_y, 0.0f);
    vbuffer[4].colour_val = c;
    vbuffer[4].tex_coords = Vector2f(tex_rect.d_min.d_x, tex_rect.d_min.d_y);

    // vertex 5
    vbuffer[5].position   = Vector3f(area.d_max.d_x, area.d_max.d_y, 0.0f);
    vbuffer[5].colour_val = c;
    vbuffer[5].tex_coords = Vector2f(tex_rect.d_max.d_x, tex_rect.d_max.d_y);

    d_geometry->setActiveTexture(&tex);
    d_geometry->appendGeometry(vbuffer, 6);
}

//----------------------------------------------------------------------------//
void RenderingWindow::unprojectPoint(const Vector2f& p_in, Vector2f& p_out)
{
    // quick test for rotations to save us a lot of work in the unrotated case
    if ((d_rotation == Quaternion::IDENTITY))
    {
        p_out = p_in;
        return;
    }

    Vector2f in(p_in);

    // localise point for cases where owner is also a RenderingWindow
    if (d_owner->isRenderingWindow())
        in -= static_cast<RenderingWindow*>(d_owner)->getPosition();

    d_owner->getRenderTarget().unprojectPoint(*d_geometry, in, p_out);
    p_out.d_x += d_position.d_x;
    p_out.d_y += d_position.d_y;
}

//----------------------------------------------------------------------------//
void RenderingWindow::invalidateGeometry()
{
    d_geometryValid = false;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

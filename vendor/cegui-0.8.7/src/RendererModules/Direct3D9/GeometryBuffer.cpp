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
#include "CEGUI/RendererModules/Direct3D9/GeometryBuffer.h"
#include "CEGUI/RendererModules/Direct3D9/Texture.h"
#include "CEGUI/RenderEffect.h"
#include "CEGUI/Vertex.h"
#include <d3d9.h>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
Direct3D9GeometryBuffer::Direct3D9GeometryBuffer(Direct3D9Renderer& owner,
                                                 LPDIRECT3DDEVICE9 device) :
    d_owner(owner),
    d_activeTexture(0),
    d_clipRect(0, 0, 0, 0),
    d_clippingActive(true),
    d_translation(0, 0, 0),
    d_rotation(0, 0, 0),
    d_pivot(0, 0, 0),
    d_effect(0),
    d_device(device),
    d_matrixValid(false)
{
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::draw() const
{
    RECT saved_clip;
    d_device->GetScissorRect(&saved_clip);

    // setup clip region
    RECT clip;
    clip.left   = static_cast<LONG>(d_clipRect.left());
    clip.top    = static_cast<LONG>(d_clipRect.top());
    clip.right  = static_cast<LONG>(d_clipRect.right());
    clip.bottom = static_cast<LONG>(d_clipRect.bottom());

    // apply the transformations we need to use.
    if (!d_matrixValid)
        updateMatrix();

    d_device->SetTransform(D3DTS_WORLD, &d_matrix);

    d_owner.setupRenderingBlendMode(d_blendMode);

    const int pass_count = d_effect ? d_effect->getPassCount() : 1;
    for (int pass = 0; pass < pass_count; ++pass)
    {
        // set up RenderEffect
        if (d_effect)
            d_effect->performPreRenderFunctions(pass);

        // draw the batches
        size_t pos = 0;
        BatchList::const_iterator i = d_batches.begin();
        for ( ; i != d_batches.end(); ++i)
        {
            if (i->clip)
                d_device->SetScissorRect(&clip);

            d_device->SetTexture(0, i->texture);
            d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, i->vertexCount / 3,
                                    &d_vertices[pos], sizeof(D3DVertex));
            pos += i->vertexCount;

            if (i->clip)
                d_device->SetScissorRect(&saved_clip);
        }
    }

    // clean up RenderEffect
    if (d_effect)
        d_effect->performPostRenderFunctions();
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setTranslation(const Vector3f& t)
{
    d_translation = t;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setRotation(const Quaternion& r)
{
    d_rotation = r;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setPivot(const Vector3f& p)
{
    d_pivot = p;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setClippingRegion(const Rectf& region)
{
    d_clipRect.top(ceguimax(0.0f, region.top()));
    d_clipRect.bottom(ceguimax(0.0f, region.bottom()));
    d_clipRect.left(ceguimax(0.0f, region.left()));
    d_clipRect.right(ceguimax(0.0f, region.right()));
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::appendVertex(const Vertex& vertex)
{
    appendGeometry(&vertex, 1);
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::appendGeometry(const Vertex* const vbuff,
                                             uint vertex_count)
{
    performBatchManagement();

    // update size of current batch
    d_batches.back().vertexCount += vertex_count;

    // buffer these vertices
    D3DVertex vd;
    const Vertex* vs = vbuff;
    for (uint i = 0; i < vertex_count; ++i, ++vs)
    {
        // copy vertex info the buffer, converting from CEGUI::Vertex to
        // something directly usable by D3D as needed.
        vd.x       = vs->position.d_x - 0.5f;
        vd.y       = vs->position.d_y - 0.5f;
        vd.z       = vs->position.d_z;
        vd.diffuse = vs->colour_val.getARGB();
        vd.tu      = vs->tex_coords.d_x;
        vd.tv      = vs->tex_coords.d_y;
        d_vertices.push_back(vd);
    }
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setActiveTexture(Texture* texture)
{
    d_activeTexture = static_cast<Direct3D9Texture*>(texture);
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::reset()
{
    d_batches.clear();
    d_vertices.clear();
    d_activeTexture = 0;
}

//----------------------------------------------------------------------------//
Texture* Direct3D9GeometryBuffer::getActiveTexture() const
{
    return d_activeTexture;
}

//----------------------------------------------------------------------------//
uint Direct3D9GeometryBuffer::getVertexCount() const
{
    return d_vertices.size();
}

//----------------------------------------------------------------------------//
uint Direct3D9GeometryBuffer::getBatchCount() const
{
    return d_batches.size();
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setRenderEffect(RenderEffect* effect)
{
    d_effect = effect;
}

//----------------------------------------------------------------------------//
RenderEffect* Direct3D9GeometryBuffer::getRenderEffect()
{
    return d_effect;
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::performBatchManagement()
{
    const LPDIRECT3DTEXTURE9 t = d_activeTexture ?
                                 d_activeTexture->getDirect3D9Texture() : 0;

    // create a new batch if there are no batches yet, or if the active texture
    // differs from that used by the current batch.
    if (d_batches.empty() ||
        t != d_batches.back().texture ||
        d_clippingActive != d_batches.back().clip)
    {
        BatchInfo batch = {t, 0, d_clippingActive};
        d_batches.push_back(batch);
    }
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::updateMatrix() const
{
    const D3DXVECTOR3 p(d_pivot.d_x, d_pivot.d_y, d_pivot.d_z);
    const D3DXVECTOR3 t(d_translation.d_x, d_translation.d_y, d_translation.d_z);

    D3DXQUATERNION r;
    r.x = d_rotation.d_x;
    r.y = d_rotation.d_y;
    r.z = d_rotation.d_z;
    r.w = d_rotation.d_w;

    D3DXMatrixTransformation(&d_matrix, 0, 0, 0, &p, &r, &t);

    d_matrixValid = true;
}

//----------------------------------------------------------------------------//
const D3DXMATRIX* Direct3D9GeometryBuffer::getMatrix() const
{
    if (!d_matrixValid)
        updateMatrix();

    return &d_matrix;
}

//----------------------------------------------------------------------------//
void Direct3D9GeometryBuffer::setClippingActive(const bool active)
{
    d_clippingActive = active;
}

//----------------------------------------------------------------------------//
bool Direct3D9GeometryBuffer::isClippingActive() const
{
    return d_clippingActive;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

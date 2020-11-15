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
#include "CEGUI/RendererModules/Direct3D9/ViewportTarget.h"
#include "CEGUI/RenderQueue.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/Exceptions.h"
#include <d3d9.h>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
Direct3D9ViewportTarget::Direct3D9ViewportTarget(Direct3D9Renderer& owner) :
    Direct3D9RenderTarget<>(owner)
{
    // initialise renderer size
    D3DVIEWPORT9 vp;
    if (FAILED(d_owner.getDevice()->GetViewport(&vp)))
        CEGUI_THROW(RendererException("Unable to access "
            "required view port information from Direct3DDevice9."));

    Rectf area(
        Vector2f(static_cast<float>(vp.X), static_cast<float>(vp.Y)),
        Sizef(static_cast<float>(vp.Width), static_cast<float>(vp.Height))
    );

    setArea(area);
}

//----------------------------------------------------------------------------//
Direct3D9ViewportTarget::Direct3D9ViewportTarget(Direct3D9Renderer& owner,
    const Rectf& area) :
        Direct3D9RenderTarget<>(owner)
{
    setArea(area);
}

//----------------------------------------------------------------------------//
bool Direct3D9ViewportTarget::isImageryCache() const
{
    return false;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

//----------------------------------------------------------------------------//
// Implementation of template base class
#include "./RenderTarget.inl"


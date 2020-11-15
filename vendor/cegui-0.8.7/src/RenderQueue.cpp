/***********************************************************************
    created:    Fri Jan 9 2009
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
#include "CEGUI/RenderQueue.h"
#include "CEGUI/GeometryBuffer.h"
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
void RenderQueue::draw() const
{
    // draw the buffers
    BufferList::const_iterator i = d_buffers.begin();
    for ( ; i != d_buffers.end(); ++i)
        (*i)->draw();
}

//----------------------------------------------------------------------------//
void RenderQueue::addGeometryBuffer(const GeometryBuffer& buffer)
{
    d_buffers.push_back(&buffer);
}

//----------------------------------------------------------------------------//
void RenderQueue::removeGeometryBuffer(const GeometryBuffer& buffer)
{
    BufferList::iterator i = std::find(d_buffers.begin(), d_buffers.end(),
                                       &buffer);
    if (i != d_buffers.end())
        d_buffers.erase(i);
}

//----------------------------------------------------------------------------//
void RenderQueue::reset()
{
    d_buffers.clear();
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

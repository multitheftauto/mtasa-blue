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
#include "CEGUI/RenderingSurface.h"
#include "CEGUI/RenderTarget.h"
#include "CEGUI/RenderingWindow.h"
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
// RenderQueueEventArgs
//----------------------------------------------------------------------------//
RenderQueueEventArgs::RenderQueueEventArgs(const RenderQueueID id) :
    queueID(id)
{}

//----------------------------------------------------------------------------//
// RenderingSurface
//----------------------------------------------------------------------------//
// Namespace for global events
const String RenderingSurface::EventNamespace("RenderingSurface");
// Event that signals the start of rendering for a queue.
const String RenderingSurface::EventRenderQueueStarted("RenderQueueStarted");
// Event that signals the end of rendering for a queue.
const String RenderingSurface::EventRenderQueueEnded("RenderQueueEnded");

//----------------------------------------------------------------------------//
RenderingSurface::RenderingSurface(RenderTarget& target) :
    d_target(&target),
    d_invalidated(true)
{
}

//----------------------------------------------------------------------------//
RenderingSurface::~RenderingSurface()
{
    // destroy all the RenderingWindow objects attached to this surface
    const size_t count = d_windows.size();
    for (size_t i = 0; i < count; ++i)
        CEGUI_DELETE_AO d_windows[i];
}

//----------------------------------------------------------------------------//
void RenderingSurface::addGeometryBuffer(const RenderQueueID queue,
    const GeometryBuffer& buffer)
{
    d_queues[queue].addGeometryBuffer(buffer);
}

//----------------------------------------------------------------------------//
void RenderingSurface::removeGeometryBuffer(const RenderQueueID queue,
    const GeometryBuffer& buffer)
{
    d_queues[queue].removeGeometryBuffer(buffer);
}

//----------------------------------------------------------------------------//
void RenderingSurface::clearGeometry(const RenderQueueID queue)
{
    d_queues[queue].reset();
}

//----------------------------------------------------------------------------//
void RenderingSurface::clearGeometry()
{
    RenderQueueList::iterator i = d_queues.begin();

    for ( ; d_queues.end() != i; ++i)
        i->second.reset();
}

//----------------------------------------------------------------------------//
void RenderingSurface::draw()
{
    d_target->activate();

    drawContent();

    d_target->deactivate();
}

//----------------------------------------------------------------------------//
void RenderingSurface::drawContent()
{
    RenderQueueEventArgs evt_args(RQ_USER_0);

    for (RenderQueueList::iterator i = d_queues.begin();
         d_queues.end() != i;
         ++i)
    {
        evt_args.handled = 0;
        evt_args.queueID = i->first;
        draw(i->second, evt_args);
    }
}

//----------------------------------------------------------------------------//
void RenderingSurface::draw(const RenderQueue& queue,
    RenderQueueEventArgs& args)
{
    fireEvent(EventRenderQueueStarted, args, EventNamespace);

    d_target->draw(queue);

    args.handled = 0;
    fireEvent(EventRenderQueueEnded, args, EventNamespace);
}

//----------------------------------------------------------------------------//
void RenderingSurface::invalidate()
{
    d_invalidated = true;
}

//----------------------------------------------------------------------------//
bool RenderingSurface::isInvalidated() const
{
    return d_invalidated || !d_target->isImageryCache();
}

//----------------------------------------------------------------------------//
bool RenderingSurface::isRenderingWindow() const
{
    return false;
}

//----------------------------------------------------------------------------//
RenderingWindow& RenderingSurface::createRenderingWindow(TextureTarget& target)
{
    RenderingWindow* w = CEGUI_NEW_AO RenderingWindow(target, *this);
    attachWindow(*w);

    return *w;
}

//----------------------------------------------------------------------------//
void RenderingSurface::destroyRenderingWindow(RenderingWindow& window)
{
    if (&window.getOwner() == this)
    {
        detatchWindow(window);
        CEGUI_DELETE_AO &window;
    }
}

//----------------------------------------------------------------------------//
void RenderingSurface::transferRenderingWindow(RenderingWindow& window)
{
    if (&window.getOwner() != this)
    {
        // detach window from it's current owner
        window.getOwner().detatchWindow(window);
        // add window to this surface.
        attachWindow(window);

        window.setOwner(*this);
    }
}

//----------------------------------------------------------------------------//
void RenderingSurface::detatchWindow(RenderingWindow& w)
{
    RenderingWindowList::iterator i =
        std::find(d_windows.begin(), d_windows.end(), &w);

    if (i != d_windows.end())
    {
        d_windows.erase(i);
        invalidate();
    }
}

//----------------------------------------------------------------------------//
void RenderingSurface::attachWindow(RenderingWindow& w)
{
    d_windows.push_back(&w);
    invalidate();
}

//----------------------------------------------------------------------------//
const RenderTarget& RenderingSurface::getRenderTarget() const
{
    return *d_target;
}

//----------------------------------------------------------------------------//
RenderTarget& RenderingSurface::getRenderTarget()
{
    return const_cast<RenderTarget&>(
        static_cast<const RenderingSurface*>(this)->getRenderTarget());
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

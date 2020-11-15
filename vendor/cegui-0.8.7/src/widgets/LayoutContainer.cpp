/***********************************************************************
    created:    29/7/2010
    author:     Martin Preisler

    purpose:    Implements the Layout Container base class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/widgets/LayoutContainer.h"
#include "CEGUI/RenderingSurface.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{

const String LayoutContainer::EventNamespace("LayoutContainer");

//----------------------------------------------------------------------------//
LayoutContainer::LayoutContainer(const String& type, const String& name):
    Window(type, name),

    d_needsLayouting(false),
    d_clientChildContentArea(this, static_cast<Element::CachedRectf::DataGenerator>(&LayoutContainer::getClientChildContentArea_impl))
{
    // layout should take the whole window by default I think
    setSize(USize(cegui_reldim(1), cegui_reldim(1)));

    subscribeEvent(Window::EventChildAdded,
                   Event::Subscriber(&LayoutContainer::handleChildAdded, this));
    subscribeEvent(Window::EventChildRemoved,
                   Event::Subscriber(&LayoutContainer::handleChildRemoved, this));
}

//----------------------------------------------------------------------------//
LayoutContainer::~LayoutContainer(void)
{}

//----------------------------------------------------------------------------//
void LayoutContainer::markNeedsLayouting()
{
    d_needsLayouting = true;

    //invalidate();
}

//----------------------------------------------------------------------------//
bool LayoutContainer::needsLayouting() const
{
    return d_needsLayouting;
}

//----------------------------------------------------------------------------//
void LayoutContainer::layoutIfNecessary()
{
    if (d_needsLayouting)
    {
        layout();

        d_needsLayouting = false;
    }
}

//----------------------------------------------------------------------------//
void LayoutContainer::update(float elapsed)
{
    Window::update(elapsed);

    layoutIfNecessary();
}

//----------------------------------------------------------------------------//
const Element::CachedRectf& LayoutContainer::getClientChildContentArea() const
{
    return d_clientChildContentArea;
}

//----------------------------------------------------------------------------//
void LayoutContainer::notifyScreenAreaChanged(bool recursive)
{
    Window::notifyScreenAreaChanged(recursive);
    
    d_clientChildContentArea.invalidateCache();
}

//----------------------------------------------------------------------------//
Rectf LayoutContainer::getUnclippedInnerRect_impl(bool skipAllPixelAlignment) const
{
    return d_parent ?
           (skipAllPixelAlignment ? d_parent->getUnclippedInnerRect().getFresh(true) : d_parent->getUnclippedInnerRect().get()) :
           Window::getUnclippedInnerRect_impl(skipAllPixelAlignment);
}

//----------------------------------------------------------------------------//
Rectf LayoutContainer::getClientChildContentArea_impl(bool skipAllPixelAlignment) const
{
    if (!d_parent)
    {
        return skipAllPixelAlignment ? Window::getClientChildContentArea().getFresh(true) : Window::getClientChildContentArea().get();
    }
    else
    {
        return skipAllPixelAlignment ?
            Rectf(getUnclippedOuterRect().getFresh(true).getPosition(), d_parent->getUnclippedInnerRect().getFresh(true).getSize()) :
            Rectf(getUnclippedOuterRect().get().getPosition(), d_parent->getUnclippedInnerRect().get().getSize());
    }
}

//----------------------------------------------------------------------------//
size_t LayoutContainer::getIdxOfChild(Window* wnd) const
{
    for (size_t i = 0; i < getChildCount(); ++i)
    {
        if (getChildAtIdx(i) == wnd)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

//----------------------------------------------------------------------------//
void LayoutContainer::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
        CEGUI_THROW(InvalidRequestException(
            "LayoutContainer can only have Elements of type Window added as "
            "children (Window path: " + getNamePath() + ")."));
    
    Window::addChild_impl(wnd);

    // we have to subscribe to the EventSized for layout updates
    d_eventConnections.insert(std::make_pair(wnd,
        wnd->subscribeEvent(Window::EventSized,
            Event::Subscriber(&LayoutContainer::handleChildSized, this))));
    d_eventConnections.insert(std::make_pair(wnd,
        wnd->subscribeEvent(Window::EventMarginChanged,
            Event::Subscriber(&LayoutContainer::handleChildMarginChanged, this))));
}

//----------------------------------------------------------------------------//
void LayoutContainer::removeChild_impl(Element* element)
{
    Window* wnd = static_cast<Window*>(element);
    
    // we want to get rid of the subscription, because the child window could
    // get removed and added somewhere else, we would be wastefully updating
    // layouts if it was sized inside other Window
    ConnectionTracker::iterator conn;

    while ((conn = d_eventConnections.find(wnd)) != d_eventConnections.end())
    {
        conn->second->disconnect();
        d_eventConnections.erase(conn);
    }

    Window::removeChild_impl(wnd);
}

//----------------------------------------------------------------------------//
bool LayoutContainer::handleChildSized(const EventArgs&)
{
    markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
bool LayoutContainer::handleChildMarginChanged(const EventArgs&)
{
    markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
bool LayoutContainer::handleChildAdded(const EventArgs&)
{
    markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
bool LayoutContainer::handleChildRemoved(const EventArgs&)
{
    markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
UVector2 LayoutContainer::getOffsetForWindow(Window* window) const
{
    const UBox& margin = window->getMargin();

    return UVector2(
               margin.d_left,
               margin.d_top
           );
}

//----------------------------------------------------------------------------//
UVector2 LayoutContainer::getBoundingSizeForWindow(Window* window) const
{
    const Sizef& pixelSize = window->getPixelSize();

    // we rely on pixelSize rather than mixed absolute and relative getSize
    // this seems to solve problems when windows overlap because their size
    // is constrained by min size
    const UVector2 size(UDim(0, pixelSize.d_width), UDim(0, pixelSize.d_height));
    // todo: we still do mixed absolute/relative margin, should we convert the
    //       value to absolute?
    const UBox& margin = window->getMargin();

    return UVector2(
               margin.d_left + size.d_x + margin.d_right,
               margin.d_top + size.d_y + margin.d_bottom
           );
}

//----------------------------------------------------------------------------//
void LayoutContainer::onParentSized(ElementEventArgs& e)
{
    // This is intentionally not Window::onParentSized.
    Element::onParentSized(e);

    // force update of child positioning.
    notifyScreenAreaChanged(true);
    performChildWindowLayout(true, true);
}

//----------------------------------------------------------------------------//

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

} // End of  CEGUI namespace section


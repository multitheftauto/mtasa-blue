/***********************************************************************
    created:    22/2/2011
    author:     Martin Preisler

    purpose:    Implements the LayoutCell class
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/widgets/LayoutCell.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{

const String LayoutCell::EventNamespace("LayoutCell");
const String LayoutCell::WidgetTypeName("LayoutCell");

//----------------------------------------------------------------------------//
LayoutCell::LayoutCell(const String& type, const String& name):
        Window(type, name),
        
        d_clientChildContentArea(this, static_cast<Element::CachedRectf::DataGenerator>(&LayoutCell::getClientChildContentArea_impl))
{
    // cell should take the whole window by default I think
    setSize(USize(cegui_reldim(1), cegui_reldim(1)));

    subscribeEvent(Window::EventChildAdded,
                   Event::Subscriber(&LayoutCell::handleChildAdded, this));
    subscribeEvent(Window::EventChildRemoved,
                   Event::Subscriber(&LayoutCell::handleChildRemoved, this));
}

//----------------------------------------------------------------------------//
LayoutCell::~LayoutCell(void)
{}

const Element::CachedRectf& LayoutCell::getClientChildContentArea() const
{
    if (!d_parent)
    {
        return Window::getClientChildContentArea();
    }
    else
    {
        return d_clientChildContentArea;
    }
}

//----------------------------------------------------------------------------//
void LayoutCell::notifyScreenAreaChanged(bool recursive)
{
    d_clientChildContentArea.invalidateCache();
    
    Window::notifyScreenAreaChanged(recursive);
}

//----------------------------------------------------------------------------//
Rectf LayoutCell::getUnclippedInnerRect_impl(bool skipAllPixelAlignment) const
{
    return d_parent ?
           (skipAllPixelAlignment ? d_parent->getUnclippedInnerRect().getFresh(true) : d_parent->getUnclippedInnerRect().get()) :
           Window::getUnclippedInnerRect_impl(skipAllPixelAlignment);
}

//----------------------------------------------------------------------------//
Rectf LayoutCell::getClientChildContentArea_impl(bool skipAllPixelAlignment) const
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
void LayoutCell::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
        CEGUI_THROW(InvalidRequestException(
            "LayoutCell can only have Elements of type Window added as children "
            "(Window path: " + getNamePath() + ")."));
    
    Window::addChild_impl(wnd);

    // we have to subscribe to the EventSized for layout updates
    d_eventConnections.insert(std::make_pair(wnd,
        wnd->subscribeEvent(Window::EventSized,
            Event::Subscriber(&LayoutCell::handleChildSized, this))));
}

//----------------------------------------------------------------------------//
void LayoutCell::removeChild_impl(Element* element)
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
bool LayoutCell::handleChildSized(const EventArgs&)
{
    //markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
bool LayoutCell::handleChildAdded(const EventArgs&)
{
    if (getChildCount() > 0)
    {
        CEGUI_THROW(InvalidRequestException(
            "You can't add more than one widget to a layout cell!"));
    }

    //markNeedsLayouting();

    return true;
}

//----------------------------------------------------------------------------//
bool LayoutCell::handleChildRemoved(const EventArgs&)
{
    //markNeedsLayouting();

    return true;
}

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


} // End of  CEGUI namespace section

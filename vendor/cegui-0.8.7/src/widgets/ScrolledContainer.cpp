/***********************************************************************
    created:    1/3/2005
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
#include "CEGUI/widgets/ScrolledContainer.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/RenderingSurface.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String ScrolledContainer::WidgetTypeName("ScrolledContainer");
const String ScrolledContainer::EventNamespace("ScrolledContainer");
const String ScrolledContainer::EventContentChanged("ContentChanged");
const String ScrolledContainer::EventAutoSizeSettingChanged("AutoSizeSettingChanged");

//----------------------------------------------------------------------------//
ScrolledContainer::ScrolledContainer(const String& type, const String& name) :
    Window(type, name),
    d_contentArea(0, 0, 0, 0),
    d_autosizePane(true),
    
    d_clientChildContentArea(this, static_cast<Element::CachedRectf::DataGenerator>(&ScrolledContainer::getClientChildContentArea_impl))
{
    addScrolledContainerProperties();
    setMouseInputPropagationEnabled(true);
}

//----------------------------------------------------------------------------//
ScrolledContainer::~ScrolledContainer(void)
{
}

//----------------------------------------------------------------------------//
bool ScrolledContainer::isContentPaneAutoSized(void) const
{
    return d_autosizePane;
}

//----------------------------------------------------------------------------//
void ScrolledContainer::setContentPaneAutoSized(bool setting)
{
    if (d_autosizePane != setting)
    {
        d_autosizePane = setting;

        // Fire events
        WindowEventArgs args1(this);
        onAutoSizeSettingChanged(args1);
    }
}

//----------------------------------------------------------------------------//
const Rectf& ScrolledContainer::getContentArea(void) const
{
    return d_contentArea;
}

//----------------------------------------------------------------------------//
void ScrolledContainer::setContentArea(const Rectf& area)
{
    if (!d_autosizePane)
    {
        d_contentArea = area;
        
        // Fire event
        WindowEventArgs args(this);
        onContentChanged(args);
   }

}

//----------------------------------------------------------------------------//
const Element::CachedRectf& ScrolledContainer::getClientChildContentArea() const
{
    return d_clientChildContentArea;
}

//----------------------------------------------------------------------------//
const Element::CachedRectf& ScrolledContainer::getNonClientChildContentArea() const
{
    return d_clientChildContentArea;
}

//----------------------------------------------------------------------------//
void ScrolledContainer::notifyScreenAreaChanged(bool recursive)
{
    d_clientChildContentArea.invalidateCache();

    Window::notifyScreenAreaChanged(recursive);
}

//----------------------------------------------------------------------------//
Rectf ScrolledContainer::getChildExtentsArea(void) const
{
    Rectf extents(0, 0, 0, 0);

    const size_t childCount = getChildCount();
    if (childCount == 0)
        return extents;

    for (size_t i = 0; i < childCount; ++i)
    {
        const Window* const wnd = getChildAtIdx(i);
        Rectf area(
            CoordConverter::asAbsolute(wnd->getPosition(), d_pixelSize),
            wnd->getPixelSize());

        if (wnd->getHorizontalAlignment() == HA_CENTRE)
            area.setPosition(area.getPosition() - CEGUI::Vector2f(area.getWidth() * 0.5f - d_pixelSize.d_width * 0.5f, 0.0f));
        if (wnd->getVerticalAlignment() == VA_CENTRE)
            area.setPosition(area.getPosition() - CEGUI::Vector2f(0.0f, area.getHeight() * 0.5f - d_pixelSize.d_height * 0.5f));

        if (area.d_min.d_x < extents.d_min.d_x)
            extents.d_min.d_x = area.d_min.d_x;

        if (area.d_min.d_y < extents.d_min.d_y)
            extents.d_min.d_y = area.d_min.d_y;

        if (area.d_max.d_x > extents.d_max.d_x)
            extents.d_max.d_x = area.d_max.d_x;

        if (area.d_max.d_y > extents.d_max.d_y)
            extents.d_max.d_y = area.d_max.d_y;
    }

    return extents;
}

//----------------------------------------------------------------------------//
void ScrolledContainer::onContentChanged(WindowEventArgs& e)
{
    if (d_autosizePane)
    {
        d_contentArea = getChildExtentsArea();
    }

    fireEvent(EventContentChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void ScrolledContainer::onAutoSizeSettingChanged(WindowEventArgs& e)
{
    fireEvent(EventAutoSizeSettingChanged, e, EventNamespace);

    if (d_autosizePane)
    {
        WindowEventArgs args(this);
        onContentChanged(args);
    }
}

//----------------------------------------------------------------------------//
bool ScrolledContainer::handleChildSized(const EventArgs&)
{
    // Fire event that notifies that a child's area has changed.
    WindowEventArgs args(this);
    onContentChanged(args);
    return true;
}

//----------------------------------------------------------------------------//
bool ScrolledContainer::handleChildMoved(const EventArgs&)
{
    // Fire event that notifies that a child's area has changed.
    WindowEventArgs args(this);
    onContentChanged(args);
    return true;
}

//----------------------------------------------------------------------------//
Rectf ScrolledContainer::getUnclippedInnerRect_impl(bool skipAllPixelAlignment) const
{
    return d_parent ?
        (skipAllPixelAlignment ? d_parent->getUnclippedInnerRect().getFresh(true) : d_parent->getUnclippedInnerRect().get()) :
        Window::getUnclippedInnerRect_impl(skipAllPixelAlignment);
}

//----------------------------------------------------------------------------//
Rectf ScrolledContainer::getInnerRectClipper_impl() const
{
    return d_parent ?
        getParent()->getInnerRectClipper() :
        Window::getInnerRectClipper_impl();
}

//----------------------------------------------------------------------------//
Rectf ScrolledContainer::getHitTestRect_impl() const
{
    return d_parent ? getParent()->getHitTestRect() :
                      Window::getHitTestRect_impl();
}

//----------------------------------------------------------------------------//
Rectf ScrolledContainer::getClientChildContentArea_impl(bool skipAllPixelAlignment) const
{
    if (!d_parent)
    {
        return skipAllPixelAlignment ? Window::getUnclippedInnerRect().getFresh(true) : Window::getUnclippedInnerRect().get();
    }
    else
    {
        if (skipAllPixelAlignment)
        {
            return Rectf(getUnclippedOuterRect().getFresh(true).getPosition(),
                         getParent()->getUnclippedInnerRect().getFresh(true).getSize());
        }
        else
        {
            return Rectf(getUnclippedOuterRect().get().getPosition(),
                         getParent()->getUnclippedInnerRect().get().getSize());
        }
    }
}

//----------------------------------------------------------------------------//
void ScrolledContainer::onChildAdded(ElementEventArgs& e)
{
    Window::onChildAdded(e);

    // subscribe to some events on this child
    d_eventConnections.insert(std::make_pair(static_cast<Window*>(e.element),
        static_cast<Window*>(e.element)->subscribeEvent(Window::EventSized,
            Event::Subscriber(&ScrolledContainer::handleChildSized, this))));
    d_eventConnections.insert(std::make_pair(static_cast<Window*>(e.element),
        static_cast<Window*>(e.element)->subscribeEvent(Window::EventMoved,
            Event::Subscriber(&ScrolledContainer::handleChildMoved, this))));

    // force window to update what it thinks it's screen / pixel areas are.
    static_cast<Window*>(e.element)->notifyScreenAreaChanged(false);

    // perform notification.
    WindowEventArgs args(this);
    onContentChanged(args);
}

//----------------------------------------------------------------------------//
void ScrolledContainer::onChildRemoved(ElementEventArgs& e)
{
    Window::onChildRemoved(e);

    // disconnect from events for this window.
    ConnectionTracker::iterator conn;
    while ((conn = d_eventConnections.find(static_cast<Window*>(e.element))) != d_eventConnections.end())
    {
        conn->second->disconnect();
        d_eventConnections.erase(conn);
    }

    // perform notification only if we're not currently being destroyed
    if (!d_destructionStarted)
    {
        WindowEventArgs args(this);
        onContentChanged(args);
    }
}

//----------------------------------------------------------------------------//
void ScrolledContainer::onParentSized(ElementEventArgs& e)
{
    Window::onParentSized(e);

    // perform notification.
    WindowEventArgs args(this);
    onContentChanged(args);
}

//----------------------------------------------------------------------------//
void ScrolledContainer::addScrolledContainerProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(ScrolledContainer, bool,
        "ContentPaneAutoSized", "Property to get/set the setting which controls whether the content pane will auto-size itself."
        "  Value is either \"true\" or \"false\".",
        &ScrolledContainer::setContentPaneAutoSized, &ScrolledContainer::isContentPaneAutoSized, true
    );
    
    CEGUI_DEFINE_PROPERTY(ScrolledContainer, Rectf,
        "ContentArea", "Property to get/set the current content area rectangle of the content pane."
        "  Value is \"l:[float] t:[float] r:[float] b:[float]\" (where l is left, t is top, r is right, and b is bottom).",
        &ScrolledContainer::setContentArea, &ScrolledContainer::getContentArea, Rectf::zero()
    );
    
    CEGUI_DEFINE_PROPERTY(ScrolledContainer, Rectf,
        "ChildExtentsArea", "Property to get the current content extents rectangle."
        "  Value is \"l:[float] t:[float] r:[float] b:[float]\" (where l is left, t is top, r is right, and b is bottom).",
        0, &ScrolledContainer::getChildExtentsArea, Rectf::zero()
    );
}

//----------------------------------------------------------------------------//
void ScrolledContainer::setArea_impl(const UVector2& pos, const USize& size,
                                     bool topLeftSizing, bool fireEvents)
{
    d_clientChildContentArea.invalidateCache();
    Window::setArea_impl(pos, size, topLeftSizing, fireEvents);
}

//----------------------------------------------------------------------------//

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

} // End of  CEGUI namespace section

/************************************************************************
    created:    Sat Oct 29 2005
    author:     Tomas Lindquist Olsen
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
#include "CEGUI/widgets/ScrolledItemListBase.h"
#include "CEGUI/widgets/ClippedContainer.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Logger.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/CoordConverter.h"

// begin CEGUI namespace
namespace CEGUI
{

/*************************************************************************
    Constants
*************************************************************************/
// event strings
const String ScrolledItemListBase::EventNamespace("ScrolledItemListBase");
const String ScrolledItemListBase::EventVertScrollbarModeChanged("VertScrollbarModeChanged");
const String ScrolledItemListBase::EventHorzScrollbarModeChanged("HorzScrollbarModeChanged");

// component widget name suffixes
const String ScrolledItemListBase::VertScrollbarName( "__auto_vscrollbar__" );
const String ScrolledItemListBase::HorzScrollbarName( "__auto_hscrollbar__" );
const String ScrolledItemListBase::ContentPaneName("__auto_content_pane__");

/************************************************************************
    Constructor
************************************************************************/
ScrolledItemListBase::ScrolledItemListBase(const String& type, const String& name) :
    ItemListBase(type, name),
    d_forceVScroll(false),
    d_forceHScroll(false)
{
    d_pane = WindowManager::getSingletonPtr()->
        createWindow("ClippedContainer", ContentPaneName);

    d_pane->setAutoWindow(true);
    static_cast<ClippedContainer*>(d_pane)->setClipperWindow(this);
    d_pane->setMouseInputPropagationEnabled(true);

    addChild(d_pane);

    // add properties for this class
    addScrolledItemListBaseProperties();
}

/************************************************************************
    Destructor
************************************************************************/
ScrolledItemListBase::~ScrolledItemListBase()
{
    if (d_pane && d_pane != this)
        WindowManager::getSingletonPtr()->destroyWindow(d_pane);
}

/************************************************************************
    Initialise
************************************************************************/
void ScrolledItemListBase::initialiseComponents()
{
    // base class handling
    ItemListBase::initialiseComponents();

    // init scrollbars
    Scrollbar* v = getVertScrollbar();
    Scrollbar* h = getHorzScrollbar();

    v->setAlwaysOnTop(true);
    h->setAlwaysOnTop(true);

    v->subscribeEvent(Scrollbar::EventScrollPositionChanged,
        Event::Subscriber(&ScrolledItemListBase::handle_VScroll,this));
    h->subscribeEvent(Scrollbar::EventScrollPositionChanged,
        Event::Subscriber(&ScrolledItemListBase::handle_HScroll,this));

    v->hide();
    h->hide();
}

/************************************************************************
    Get vertical scroll bar
************************************************************************/
Scrollbar* ScrolledItemListBase::getVertScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(VertScrollbarName));
}

/************************************************************************
    Get horizontal scroll bar
************************************************************************/
Scrollbar* ScrolledItemListBase::getHorzScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(HorzScrollbarName));
}

/************************************************************************
    Configure scroll bars
************************************************************************/
void ScrolledItemListBase::configureScrollbars(const Sizef& doc_size)
{
    Scrollbar* v = getVertScrollbar();
    Scrollbar* h = getHorzScrollbar();

    const bool old_vert_visible = v->isVisible();
    const bool old_horz_visible = h->isVisible();

    Sizef render_area_size = getItemRenderArea().getSize();

    // setup the pane size
    float pane_size_w = ceguimax(doc_size.d_width, render_area_size.d_width);
    USize pane_size(cegui_absdim(pane_size_w), cegui_absdim(doc_size.d_height));

    d_pane->setMinSize(pane_size);
    d_pane->setMaxSize(pane_size);
    //d_pane->setWindowSize(pane_size);

    // "fix" scrollbar visibility
    if (d_forceVScroll || doc_size.d_height > render_area_size.d_height)
    {
        v->show();
    }
    else
    {
        v->hide();
    }

    //render_area_size = getItemRenderArea().getSize();

    if (d_forceHScroll || doc_size.d_width > render_area_size.d_width)
    {
        h->show();
    }
    else
    {
        h->hide();
    }

    // if some change occurred, invalidate the inner rect area caches.
    if ((old_vert_visible != v->isVisible()) ||
        (old_horz_visible != h->isVisible()))
    {
        d_unclippedInnerRect.invalidateCache();
        d_innerRectClipperValid = false;
    }

    // get a fresh item render area
    Rectf render_area = getItemRenderArea();
    render_area_size = render_area.getSize();

    // update the pane clipper area
    static_cast<ClippedContainer*>(d_pane)->setClipArea(render_area);

    // setup vertical scrollbar
    v->setDocumentSize(doc_size.d_height);
    v->setPageSize(render_area_size.d_height);
    v->setStepSize(ceguimax(1.0f, render_area_size.d_height / 10.0f));
    v->setScrollPosition(v->getScrollPosition());

    // setup horizontal scrollbar
    h->setDocumentSize(doc_size.d_width);
    h->setPageSize(render_area_size.d_width);
    h->setStepSize(ceguimax(1.0f, render_area_size.d_width / 10.0f));
    h->setScrollPosition(h->getScrollPosition());
}

/************************************************************************
    Handle mouse wheel event
************************************************************************/
void ScrolledItemListBase::onMouseWheel(MouseEventArgs& e)
{
    ItemListBase::onMouseWheel(e);

    size_t count = getItemCount();
    Scrollbar* v = getVertScrollbar();

    // dont do anything if we are no using scrollbars
    // or have'nt got any items
    if (!v->isVisible() || !count)
    {
        return;
    }

    const float pixH = d_pane->getUnclippedOuterRect().get().getHeight();
    const float delta = (pixH/float(count)) * -e.wheelChange;
    v->setScrollPosition(v->getScrollPosition() + delta);
    ++e.handled;
}

/************************************************************************
    Event subscribers for scrolling
************************************************************************/
bool ScrolledItemListBase::handle_VScroll(const EventArgs& e)
{
    const WindowEventArgs& we = static_cast<const WindowEventArgs&>(e);
    Scrollbar* v = static_cast<Scrollbar*>(we.window);
    float newpos = -v->getScrollPosition();
    d_pane->setYPosition(cegui_absdim(newpos));
    return true;
}

bool ScrolledItemListBase::handle_HScroll(const EventArgs& e)
{
    const WindowEventArgs& we = static_cast<const WindowEventArgs&>(e);
    Scrollbar* h = static_cast<Scrollbar*>(we.window);
    float newpos = -h->getScrollPosition();
    d_pane->setXPosition(cegui_absdim(newpos));
    return true;
}

/************************************************************************
    Set vertical scrollbar mode
************************************************************************/
void ScrolledItemListBase::setShowVertScrollbar(bool mode)
{
    if (mode != d_forceVScroll)
    {
        d_forceVScroll = mode;
        WindowEventArgs e(this);
        onVertScrollbarModeChanged(e);
    }
}

/************************************************************************
    Set horizontal scrollbar mode
************************************************************************/
void ScrolledItemListBase::setShowHorzScrollbar(bool mode)
{
    if (mode != d_forceHScroll)
    {
        d_forceHScroll = mode;
        WindowEventArgs e(this);
        onHorzScrollbarModeChanged(e);
    }
}

/************************************************************************
    Handle vertical scrollbar mode change
************************************************************************/
void ScrolledItemListBase::onVertScrollbarModeChanged(WindowEventArgs& e)
{
    fireEvent(EventVertScrollbarModeChanged, e);
}

/************************************************************************
    Handle horizontal scrollbar mode change
************************************************************************/
void ScrolledItemListBase::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
    fireEvent(EventHorzScrollbarModeChanged, e);
}

/*************************************************************************
    Add ItemListbox specific properties
*************************************************************************/
void ScrolledItemListBase::addScrolledItemListBaseProperties()
{
    const String propertyOrigin = "CEGUI/ScrolledItemListBase";

    CEGUI_DEFINE_PROPERTY(ScrolledItemListBase, bool,
        "ForceVertScrollbar", "Property to get/set the state of the force vertical scrollbar setting for the ScrolledItemListBase.  Value is either \"true\" or \"false\".",
        &ScrolledItemListBase::setShowVertScrollbar, &ScrolledItemListBase::isVertScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ScrolledItemListBase, bool,
        "ForceHorzScrollbar", "Property to get/set the state of the force horizontal scrollbar setting for the ScrolledItemListBase.  Value is either \"true\" or \"false\".",
        &ScrolledItemListBase::setShowHorzScrollbar, &ScrolledItemListBase::isHorzScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
}

//----------------------------------------------------------------------------//
void ScrolledItemListBase::ensureItemIsVisibleVert(const ItemEntry& item)
{
    const Rectf render_area = getItemRenderArea();
    Scrollbar* const v = getVertScrollbar();
    const float currPos = v->getScrollPosition();

    const float top =
        CoordConverter::asAbsolute(item.getYPosition(), getPixelSize().d_height) - currPos;
    const float bottom = top + item.getItemPixelSize().d_height;

    // if top is above the view area, or if item is too big, scroll item to top
    if ((top < render_area.d_min.d_y) || ((bottom - top) > render_area.getHeight()))
        v->setScrollPosition(currPos + top);
    // if bottom is below the view area, scroll item to bottom of list
    else if (bottom >= render_area.d_max.d_y)
        v->setScrollPosition(currPos + bottom - render_area.getHeight());
}

//----------------------------------------------------------------------------//
void ScrolledItemListBase::ensureItemIsVisibleHorz(const ItemEntry& item)
{
    const Rectf render_area = getItemRenderArea();
    Scrollbar* const h = getHorzScrollbar();
    const float currPos = h->getScrollPosition();

    const float left =
        CoordConverter::asAbsolute(item.getXPosition(), getPixelSize().d_width) - currPos;
    const float right = left + item.getItemPixelSize().d_width;

    // if left is left of the view area, or if item too big, scroll item to left
    if ((left < render_area.d_min.d_x) || ((right - left) > render_area.getWidth()))
        h->setScrollPosition(currPos + left);
    // if right is right of the view area, scroll item to right of list
    else if (right >= render_area.d_max.d_x)
        h->setScrollPosition(currPos + right - render_area.getWidth());
}

//----------------------------------------------------------------------------//

} // end CEGUI namespace

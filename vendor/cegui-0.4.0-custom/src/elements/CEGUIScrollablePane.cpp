/************************************************************************
	filename: 	CEGUIScrollablePane.cpp
	created:	1/3/2005
	author:		Paul D Turner
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "elements/CEGUIScrollablePane.h"
#include "elements/CEGUIScrolledContainer.h"
#include "elements/CEGUIScrollbar.h"
#include "CEGUIWindowManager.h"
#include <math.h>

// Start of CEGUI namespace section
namespace CEGUI
{
    //////////////////////////////////////////////////////////////////////////
    // Event name constants
    const String ScrollablePane::EventNamespace("ScrollablePane");
    const String ScrollablePane::EventContentPaneChanged("ContentPaneChanged");
    const String ScrollablePane::EventVertScrollbarModeChanged("VertScrollbarModeChanged");
    const String ScrollablePane::EventHorzScrollbarModeChanged("HorzScrollbarModeChanged");
    const String ScrollablePane::EventAutoSizeSettingChanged("AutoSizeSettingChanged");
    const String ScrollablePane::EventContentPaneScrolled("ContentPaneScrolled");
    // Property objects
    ScrollablePaneProperties::ForceHorzScrollbar   ScrollablePane::d_horzScrollbarProperty;
    ScrollablePaneProperties::ForceVertScrollbar   ScrollablePane::d_vertScrollbarProperty;
    ScrollablePaneProperties::ContentPaneAutoSized ScrollablePane::d_autoSizedProperty;
    ScrollablePaneProperties::ContentArea          ScrollablePane::d_contentAreaProperty;
    ScrollablePaneProperties::HorzStepSize         ScrollablePane::d_horzStepProperty;
    ScrollablePaneProperties::HorzOverlapSize      ScrollablePane::d_horzOverlapProperty;
    ScrollablePaneProperties::HorzScrollPosition   ScrollablePane::d_horzScrollPositionProperty;
    ScrollablePaneProperties::VertStepSize         ScrollablePane::d_vertStepProperty;
    ScrollablePaneProperties::VertOverlapSize      ScrollablePane::d_vertOverlapProperty;
    ScrollablePaneProperties::VertScrollPosition   ScrollablePane::d_vertScrollPositionProperty;
    //////////////////////////////////////////////////////////////////////////

    ScrollablePane::ScrollablePane(const String& type, const String& name) :
        Window(type, name),
        d_forceVertScroll(false),
        d_forceHorzScroll(false),
        d_contentRect(0, 0, 0, 0),
        d_vertStep(0.1f),
        d_vertOverlap(0.01f),
        d_horzStep(0.1f),
        d_horzOverlap(0.01f),
        d_vertScrollbar(0),
        d_horzScrollbar(0),
        d_container(0)
    {
        addScrollablePaneEvents();
        addScrollablePaneProperties();
    }

    ScrollablePane::~ScrollablePane(void)
    {
    }

    const ScrolledContainer* ScrollablePane::getContentPane(void) const
    {
        assert (d_container != 0);
        return d_container;
    }

    bool ScrollablePane::isVertScrollbarAlwaysShown(void) const
    {
        return d_forceVertScroll;
    }

    void ScrollablePane::setShowVertScrollbar(bool setting)
    {
        if (d_forceVertScroll != setting)
        {
            d_forceVertScroll = setting;

            configureScrollbars();
            WindowEventArgs args(this);
            onVertScrollbarModeChanged(args);
        }
    }

    bool ScrollablePane::isHorzScrollbarAlwaysShown(void) const
    {
        return d_forceHorzScroll;
    }

    void ScrollablePane::setShowHorzScrollbar(bool setting)
    {
        if (d_forceHorzScroll != setting)
        {
            d_forceHorzScroll = setting;

            configureScrollbars();
            WindowEventArgs args(this);
            onHorzScrollbarModeChanged(args);
        }
    }

    bool ScrollablePane::isContentPaneAutoSized(void) const
    {
        assert(d_container != 0);
        return d_container->isContentPaneAutoSized();
    }

    void ScrollablePane::setContentPaneAutoSized(bool setting)
    {
        assert(d_container != 0);
        d_container->setContentPaneAutoSized(setting);
    }

    const Rect& ScrollablePane::getContentPaneArea(void) const
    {
        assert(d_container != 0);
        return d_container->getContentArea();
    }

    void ScrollablePane::setContentPaneArea(const Rect& area)
    {
        assert(d_container != 0);
        d_container->setContentArea(area);
    }

    float ScrollablePane::getHorizontalStepSize(void) const
    {
        return d_horzStep;
    }

    void ScrollablePane::setHorizontalStepSize(float step)
    {
        d_horzStep = step;
        configureScrollbars();
    }

    float ScrollablePane::getHorizontalOverlapSize(void) const
    {
        return d_horzOverlap;
    }

    void ScrollablePane::setHorizontalOverlapSize(float overlap)
    {
        d_horzOverlap = overlap;
        configureScrollbars();
    }

    float ScrollablePane::getHorizontalScrollPosition(void) const
    {
        assert(d_horzScrollbar != 0);
        float docSz = d_horzScrollbar->getDocumentSize();
        return (docSz != 0) ? d_horzScrollbar->getScrollPosition() / docSz : 0.0f;
    }

    void ScrollablePane::setHorizontalScrollPosition(float position)
    {
        assert(d_horzScrollbar != 0);
        d_horzScrollbar->setScrollPosition(d_horzScrollbar->getDocumentSize() * position);
    }

    float ScrollablePane::getVerticalStepSize(void) const
    {
        return d_vertStep;
    }

    void ScrollablePane::setVerticalStepSize(float step)
    {
        d_vertStep = step;
        configureScrollbars();
    }

    float ScrollablePane::getVerticalOverlapSize(void) const
    {
        return d_vertOverlap;
    }

    void ScrollablePane::setVerticalOverlapSize(float overlap)
    {
        d_vertOverlap = overlap;
        configureScrollbars();
    }

    float ScrollablePane::getVerticalScrollPosition(void) const
    {
        assert(d_vertScrollbar != 0);
        float docSz = d_vertScrollbar->getDocumentSize();
        return (docSz != 0) ? d_vertScrollbar->getScrollPosition() / docSz : 0.0f;
    }

    void ScrollablePane::setVerticalScrollPosition(float position)
    {
        assert(d_vertScrollbar != 0);
        d_vertScrollbar->setScrollPosition(d_vertScrollbar->getDocumentSize() * position);
    }

    void ScrollablePane::initialise(void)
    {
        String widgetName;
        // create horizontal scrollbar
        widgetName = d_name + "__auto_hscrollbar__";
        d_horzScrollbar = createHorizontalScrollbar(widgetName);
        // perform consistency checks on what we got back
        assert(d_horzScrollbar != 0);
        assert(d_horzScrollbar->getName() == widgetName);

        // create vertical scrollbar
        widgetName = d_name + "__auto_vscrollbar__";
        d_vertScrollbar = createVerticalScrollbar(widgetName);
        // perform consistency checks on what we got back
        assert(d_vertScrollbar != 0);
        assert(d_vertScrollbar->getName() == widgetName);

        // create scrolled container widget
        d_container = 
            static_cast<ScrolledContainer*>(WindowManager::getSingleton().createWindow(
                ScrolledContainer::WidgetTypeName, d_name + "__auto_container__"));

        // add child controls
        addChildWindow(d_horzScrollbar);
        addChildWindow(d_vertScrollbar);
        addChildWindow(d_container);

        // do a bit of initialisation
        d_horzScrollbar->setAlwaysOnTop(true);
        d_vertScrollbar->setAlwaysOnTop(true);
        // container pane is always same size as this parent pane,
        // scrolling is actually implemented via positioning and clipping tricks.
        d_container->setSize(Relative, Size(1.0f, 1.0f));

        // subscribe to events we need to hear about
        d_vertScrollbar->subscribeEvent(
            Scrollbar::EventScrollPositionChanged,
            Event::Subscriber(&ScrollablePane::handleScrollChange, this));
        d_horzScrollbar->subscribeEvent(
            Scrollbar::EventScrollPositionChanged,
            Event::Subscriber(&ScrollablePane::handleScrollChange, this));
        d_container->subscribeEvent(
            ScrolledContainer::EventContentChanged,
            Event::Subscriber(&ScrollablePane::handleContentAreaChange, this));
        d_container->subscribeEvent(
            ScrolledContainer::EventAutoSizeSettingChanged,
            Event::Subscriber(&ScrollablePane::handleAutoSizePaneChanged, this));

        // finalise setup
        configureScrollbars();
    }

    void ScrollablePane::addScrollablePaneEvents(bool bCommon)
    {
        if ( bCommon == false )	addEvent(EventAutoSizeSettingChanged);
        if ( bCommon == false )	addEvent(EventContentPaneChanged);
        if ( bCommon == false )	addEvent(EventHorzScrollbarModeChanged);
        if ( bCommon == false )	addEvent(EventVertScrollbarModeChanged);
        if ( bCommon == false )	addEvent(EventContentPaneScrolled);
    }

    void ScrollablePane::configureScrollbars(void)
    {
        // controls should all be valid by this stage
        assert(d_container != 0);
        assert(d_vertScrollbar != 0);
        assert(d_horzScrollbar != 0);

        // enable required scrollbars
        d_vertScrollbar->setVisible(isVertScrollbarNeeded());
        d_horzScrollbar->setVisible(isHorzScrollbarNeeded());

        // Check if the addition of the horizontal scrollbar means we
        // now also need the vertical bar.
        if (d_horzScrollbar->isVisible())
        {
            d_vertScrollbar->setVisible(isVertScrollbarNeeded());
        }

        performChildWindowLayout();

        // get viewable area
        Rect viewableArea(getViewableArea());

        // set up vertical scroll bar values
        d_vertScrollbar->setDocumentSize(fabsf(d_contentRect.getHeight()));
        d_vertScrollbar->setPageSize(viewableArea.getHeight());
        d_vertScrollbar->setStepSize(ceguimax(1.0f, viewableArea.getHeight() * d_vertStep));
        d_vertScrollbar->setOverlapSize(ceguimax(1.0f, viewableArea.getHeight() * d_vertOverlap));
        d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition());

        // set up horizontal scroll bar values
        d_horzScrollbar->setDocumentSize(fabsf(d_contentRect.getWidth()));
        d_horzScrollbar->setPageSize(viewableArea.getWidth());
        d_horzScrollbar->setStepSize(ceguimax(1.0f, viewableArea.getWidth() * d_horzStep));
        d_horzScrollbar->setOverlapSize(ceguimax(1.0f, viewableArea.getWidth() * d_horzOverlap));
        d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition());
    }

    bool ScrollablePane::isHorzScrollbarNeeded(void) const
    {
        assert(d_container != 0);

        return ((fabs(d_contentRect.getWidth()) > getViewableArea().getWidth()) || d_forceHorzScroll);
    }

    bool ScrollablePane::isVertScrollbarNeeded(void) const
    {
        assert(d_container != 0);

        return ((fabs(d_contentRect.getHeight()) > getViewableArea().getHeight()) || d_forceVertScroll);
    }

    void ScrollablePane::updateContainerPosition(void)
    {
        assert(d_container != 0);
        assert(d_horzScrollbar != 0);
        assert(d_vertScrollbar != 0);

        // basePos is the position represented by the scrollbars
        // (these are negated so pane is scrolled in the correct directions)
        Point basePos(-d_horzScrollbar->getScrollPosition(), -d_vertScrollbar->getScrollPosition());

        // this bias is the absolute position that 0 on the scrollbars represent.
        // effectively removes un-used empty space from the pane.
        Point bias(d_contentRect.d_left, d_contentRect.d_top);

        // set the new container pane position to be what the scrollbars request
        // minus any bias generated by the location of the content.
        d_container->setPosition(Absolute, basePos - bias);
    }

    void ScrollablePane::onContentPaneChanged(WindowEventArgs& e)
    {
        fireEvent(EventContentPaneChanged, e, EventNamespace);
    }

    void ScrollablePane::onVertScrollbarModeChanged(WindowEventArgs& e)
    {
        fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
    }

    void ScrollablePane::onHorzScrollbarModeChanged(WindowEventArgs& e)
    {
        fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
    }

    void ScrollablePane::onAutoSizeSettingChanged(WindowEventArgs& e)
    {
        fireEvent(EventAutoSizeSettingChanged, e, EventNamespace);
    }

    void ScrollablePane::onContentPaneScrolled(WindowEventArgs& e)
    {
        updateContainerPosition();
        fireEvent(EventContentPaneScrolled, e, EventNamespace);
    }

    bool ScrollablePane::handleScrollChange(const EventArgs& e)
    {
        WindowEventArgs args(this);
        onContentPaneScrolled(args);
        return true;
    }

    bool ScrollablePane::handleContentAreaChange(const EventArgs& e)
    {
        assert(d_container != 0);
        assert(d_horzScrollbar != 0);
        assert(d_vertScrollbar != 0);

        // get updated extents of the content
        Rect contentArea(d_container->getContentArea());

        // calculate any change on the top and left edges.
        float xChange = contentArea.d_left - d_contentRect.d_left;
        float yChange = contentArea.d_top - d_contentRect.d_top;

        // store new content extents information
        d_contentRect = contentArea;

        configureScrollbars();

        // update scrollbar positions (which causes container pane to be moved as needed).
        d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() - xChange);
        d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() - yChange);

        // this call may already have been made if the scroll positions changed.  The call
        // is required here for cases where the top/left 'bias' has changed; in which
        // case the scroll position notification may or may not have been fired.
        if (xChange || yChange)
        {
            updateContainerPosition();
        }
    
        // fire event
        WindowEventArgs args(this);
        onContentPaneChanged(args);

        return true;
    }

    bool ScrollablePane::handleAutoSizePaneChanged(const EventArgs& e)
    {
        // just forward event to client.
        WindowEventArgs args(this);
        fireEvent(EventAutoSizeSettingChanged, args, EventNamespace);
        return args.handled;
    }

    void ScrollablePane::addChild_impl(Window* wnd)
    {
        // null is not a valid window pointer!
        assert(wnd != 0);

        // See if this is an internally generated window (will have "__auto_" in the name)
        if (wnd->getName().find("__auto_") != String::npos)
        {
            // This is an internal widget, so should be added normally.
            Window::addChild_impl(wnd);
        }
        // this is a client window/widget, so should be added to the pane container.
        else
        {
            // container should always be valid by the time we're adding client controls
            assert(d_container != 0);
            d_container->addChildWindow(wnd);
        }
    }

    void ScrollablePane::removeChild_impl(Window* wnd)
    {
        // null is not a valid window pointer!
        assert(wnd != 0);

        // See if this is an internally generated window (will have "__auto_" in the name)
        if (wnd->getName().find("__auto_") != String::npos)
        {
            // This is an internal widget, so should be removed normally.
            Window::removeChild_impl(wnd);
        }
        // this is a client window/widget, so should be removed from the pane container.
        else
        {
            // container should always be valid by the time we're handling client controls
            assert(d_container != 0);
            d_container->removeChildWindow(wnd);
        }
    }

    void ScrollablePane::onSized(WindowEventArgs& e)
    {
        Window::onSized(e);
        configureScrollbars();
        updateContainerPosition();

        e.handled = true;
    }

    void ScrollablePane::onMouseWheel(MouseEventArgs& e)
    {
        // base class processing.
        Window::onMouseWheel(e);

        if (d_vertScrollbar->isVisible() && (d_vertScrollbar->getDocumentSize() > d_vertScrollbar->getPageSize()))
        {
            d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + d_vertScrollbar->getStepSize() * -e.wheelChange);
        }
        else if (d_horzScrollbar->isVisible() && (d_horzScrollbar->getDocumentSize() > d_horzScrollbar->getPageSize()))
        {
            d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + d_horzScrollbar->getStepSize() * -e.wheelChange);
        }

        e.handled = true;
    }

    void ScrollablePane::addScrollablePaneProperties( bool bCommon )
    {
        if ( bCommon == false )   addProperty(&d_horzScrollbarProperty);
        if ( bCommon == false )   addProperty(&d_vertScrollbarProperty);
        if ( bCommon == true )   addProperty(&d_autoSizedProperty);
        if ( bCommon == true )   addProperty(&d_contentAreaProperty);
        if ( bCommon == false )   addProperty(&d_horzStepProperty);
        if ( bCommon == false )   addProperty(&d_horzOverlapProperty);
        if ( bCommon == false )   addProperty(&d_horzScrollPositionProperty);
        if ( bCommon == true )   addProperty(&d_vertStepProperty);
        if ( bCommon == false )   addProperty(&d_vertOverlapProperty);
        if ( bCommon == false )   addProperty(&d_vertScrollPositionProperty);
    }

} // End of  CEGUI namespace section

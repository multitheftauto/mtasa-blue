/************************************************************************
	filename: 	CEGUIScrolledContainer.cpp
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
#include "elements/CEGUIScrolledContainer.h"

// Start of CEGUI namespace section
namespace CEGUI
{   //////////////////////////////////////////////////////////////////////////
    // static data definitions
    //////////////////////////////////////////////////////////////////////////
    // Widget type name string
    const String ScrolledContainer::WidgetTypeName("ScrolledContainer");
    // Event string constants
    const String ScrolledContainer::EventNamespace("ScrolledContainer");
    const String ScrolledContainer::EventContentChanged("ContentChanged");
    const String ScrolledContainer::EventAutoSizeSettingChanged("AutoSizeSettingChanged");
    // properties
    ScrolledContainerProperties::ContentPaneAutoSized ScrolledContainer::d_autoSizedProperty;
    ScrolledContainerProperties::ContentArea          ScrolledContainer::d_contentAreaProperty;
    ScrolledContainerProperties::ChildExtentsArea     ScrolledContainer::d_childExtentsAreaProperty;
    //////////////////////////////////////////////////////////////////////////
    
    ScrolledContainer::ScrolledContainer(const String& type, const String& name) :
        Window(type, name),
        d_contentArea(0, 0, 0, 0),
        d_autosizePane(true)
    {
        addScrolledContainerEvents();
        addScrolledContainerProperties();
    }

    ScrolledContainer::~ScrolledContainer(void)
    {
    }

    bool ScrolledContainer::isContentPaneAutoSized(void) const
    {
        return d_autosizePane;
    }

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

    const Rect& ScrolledContainer::getContentArea(void) const
    {
        return d_contentArea;
    }

    void ScrolledContainer::setContentArea(const Rect& area)
    {
        if (!d_autosizePane)
        {
            d_contentArea = area;
            
            // Fire event
            WindowEventArgs args(this);
            onContentChanged(args);
       }

    }

    void ScrolledContainer::addScrolledContainerEvents(bool bCommon)
    {
        if ( bCommon == true )
        {
            addEvent(EventContentChanged);
            addEvent(EventAutoSizeSettingChanged);
        }
    }

    Rect ScrolledContainer::getChildExtentsArea(void) const
    {
        uint childCount = getChildCount();

        // set up initial content area to match first child.
        if (childCount != 0)
        {
            Window* wnd = getChildAtIdx(0);
            Rect extents(wnd->getAbsoluteRect());

            // control var starts at 1 since we already dealt with 0 above
            for (uint i = 1; i < childCount; ++i)
            {
                wnd = getChildAtIdx(i);
                Rect area(wnd->getAbsoluteRect());

                if (area.d_left < extents.d_left)
                    extents.d_left = area.d_left;

                if (area.d_top < extents.d_top)
                    extents.d_top = area.d_top;

                if (area.d_right > extents.d_right)
                    extents.d_right = area.d_right;

                if (area.d_bottom > extents.d_bottom)
                    extents.d_bottom = area.d_bottom;
            }

            return extents;
        }
        else
        {
            return Rect(0, 0, 0, 0);
        }
    }

    void ScrolledContainer::onContentChanged(WindowEventArgs& e)
    {
        if (d_autosizePane)
        {
            d_contentArea = getChildExtentsArea();
        }

        fireEvent(EventContentChanged, e, EventNamespace);
    }

    void ScrolledContainer::onAutoSizeSettingChanged(WindowEventArgs& e)
    {
        fireEvent(EventAutoSizeSettingChanged, e, EventNamespace);

        if (d_autosizePane)
        {
            WindowEventArgs args(this);
            onContentChanged(args);
        }
    }

    bool ScrolledContainer::handleChildSized(const EventArgs& e)
    {
        // Fire event that notifies that a child's area has changed.
        WindowEventArgs args(this);
        onContentChanged(args);
        return true;
    }

    bool ScrolledContainer::handleChildMoved(const EventArgs& e)
    {
        // Fire event that notifies that a child's area has changed.
        WindowEventArgs args(this);
        onContentChanged(args);
        return true;
    }

    Rect ScrolledContainer::getUnclippedInnerRect(void) const
    {
        // return inner rect of our parent when possible,
        // or of the screen when not.
        return d_parent ?
            d_parent->getUnclippedInnerRect() :
            System::getSingleton().getRenderer()->getRect();
    }

    void ScrolledContainer::onChildAdded(WindowEventArgs& e)
    {
        Window::onChildAdded(e);

        // subscribe to some events on this child
        d_eventConnections.insert(std::make_pair(e.window,
            e.window->subscribeEvent(Window::EventSized,
                Event::Subscriber(&ScrolledContainer::handleChildSized, this))));
        d_eventConnections.insert(std::make_pair(e.window,
            e.window->subscribeEvent(Window::EventMoved,
                Event::Subscriber(&ScrolledContainer::handleChildMoved, this))));

        // perform notification.
        WindowEventArgs args(this);
        onContentChanged(args);
    }

    void ScrolledContainer::onChildRemoved(WindowEventArgs& e)
    {
        Window::onChildRemoved(e);

        // disconnect from events for this window.
        ConnectionTracker::iterator conn;
        while ((conn = d_eventConnections.find(e.window)) != d_eventConnections.end())
        {
            conn->second->disconnect();
            d_eventConnections.erase(conn);
        }

        // perform notification.
        WindowEventArgs args(this);
        onContentChanged(args);
    }

    void ScrolledContainer::onParentSized(WindowEventArgs& e)
    {
        Window::onParentSized(e);

        // perform notification.
        WindowEventArgs args(this);
        onContentChanged(args);
    }

    void ScrolledContainer::addScrolledContainerProperties( bool bCommon )
    {
        if ( bCommon == false )
        {
            addProperty(&d_autoSizedProperty);
            addProperty(&d_contentAreaProperty);
            addProperty(&d_childExtentsAreaProperty);
        }
    }

} // End of  CEGUI namespace section

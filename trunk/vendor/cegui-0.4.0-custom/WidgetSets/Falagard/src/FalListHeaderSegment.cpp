/************************************************************************
    filename:   FalListHeaderSegment.cpp
    created:    Tue Jul 5 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "FalListHeaderSegment.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardListHeaderSegment::WidgetTypeName[] = "Falagard/ListHeaderSegment";

    // properties
    FalagardListHeaderSegmentProperties::SizingCursorImage   FalagardListHeaderSegment::d_sizingCursorProperty;
    FalagardListHeaderSegmentProperties::MovingCursorImage   FalagardListHeaderSegment::d_movingCursorProperty;


    FalagardListHeaderSegment::FalagardListHeaderSegment(const String& type, const String& name) :
        ListHeaderSegment(type, name)
    {
        addProperty(&d_sizingCursorProperty);
        addProperty(&d_movingCursorProperty);
    }

    FalagardListHeaderSegment::~FalagardListHeaderSegment()
    {
    }

    const Image* FalagardListHeaderSegment::getSizingCursorImage() const
    {
        return d_sizingMouseCursor;
    }

    void FalagardListHeaderSegment::setSizingCursorImage(const Image* image)
    {
        d_sizingMouseCursor = image;
    }

    void FalagardListHeaderSegment::setSizingCursorImage(const String& imageset, const String& image)
    {
        d_sizingMouseCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
    }

    const Image* FalagardListHeaderSegment::getMovingCursorImage() const
    {
        return d_movingMouseCursor;
    }

    void FalagardListHeaderSegment::setMovingCursorImage(const Image* image)
    {
        d_movingMouseCursor = image;
    }

    void FalagardListHeaderSegment::setMovingCursorImage(const String& imageset, const String& image)
    {
        d_movingMouseCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
    }

    void FalagardListHeaderSegment::populateRenderCache()
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);

        const StateImagery* imagery;

        // get imagery for main state.
        if (isDisabled())
        {
            imagery = &wlf.getStateImagery("Disabled");
        }
        else if ((d_segmentHover != d_segmentPushed) && !d_splitterHover && isClickable())
        {
            imagery = &wlf.getStateImagery("Hover");
        }
        else if (d_splitterHover)
        {
            imagery = &wlf.getStateImagery("SplitterHover");
        }
        else
        {
            imagery = &wlf.getStateImagery("Normal");
        }

        // do main rendering
        imagery->render(*this);

        // Render sorting icon as needed
        if (d_sortDir == Ascending)
        {
            imagery = &wlf.getStateImagery("AscendingSortIcon");
            imagery->render(*this);
        }
        else if (d_sortDir == Descending)
        {
            imagery = &wlf.getStateImagery("DescendingSortIcon");
            imagery->render(*this);
        }

        // draw ghost copy if the segment is being dragged.
        if (d_dragMoving)
        {
            Rect targetArea(0, 0, getAbsoluteWidth(), getAbsoluteHeight());
            targetArea.offset(d_dragPosition);
            imagery = &wlf.getStateImagery("DragGhost");
            imagery->render(*this, targetArea);

            // Render sorting icon as needed
            if (d_sortDir == Ascending)
            {
                imagery = &wlf.getStateImagery("GhostAscendingSortIcon");
                imagery->render(*this, targetArea);
            }
            else if (d_sortDir == Descending)
            {
                imagery = &wlf.getStateImagery("GhostDescendingSortIcon");
                imagery->render(*this, targetArea);
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardListHeaderSegmentFactory::createWindow(const String& name)
    {
        return new FalagardListHeaderSegment(d_type, name);
    }

    void FalagardListHeaderSegmentFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section

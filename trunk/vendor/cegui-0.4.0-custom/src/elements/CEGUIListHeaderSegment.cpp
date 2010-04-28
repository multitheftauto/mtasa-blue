/************************************************************************
	filename: 	CEGUIListHeaderSegment.cpp
	created:	15/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of List header segment widget.
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
#include "elements/CEGUIListHeaderSegment.h"
#include "CEGUIMouseCursor.h"


// Start of CEGUI namespace section
namespace CEGUI
{
const String ListHeaderSegment::EventNamespace("ListHeaderSegment");

/*************************************************************************
	Properties for this class
*************************************************************************/
ListHeaderSegmentProperties::Clickable		ListHeaderSegment::d_clickableProperty;
ListHeaderSegmentProperties::Dragable		ListHeaderSegment::d_dragableProperty;
ListHeaderSegmentProperties::Sizable		ListHeaderSegment::d_sizableProperty;
ListHeaderSegmentProperties::SortDirection	ListHeaderSegment::d_sortDirectionProperty;


/*************************************************************************
	Constants
*************************************************************************/
// Event names
const String ListHeaderSegment::EventSegmentClicked( (utf8*)"SegmentClicked" );
const String ListHeaderSegment::EventSplitterDoubleClicked( (utf8*)"SplitterDoubleClicked" );
const String ListHeaderSegment::EventSizingSettingChanged( (utf8*)"SizingSettingChanged" );
const String ListHeaderSegment::EventSortDirectionChanged( (utf8*)"SortDirectionChanged" );
const String ListHeaderSegment::EventMovableSettingChanged( (utf8*)"MovableSettingChanged" );
const String ListHeaderSegment::EventSegmentDragStart( (utf8*)"SegmentDragStart" );
const String ListHeaderSegment::EventSegmentDragStop( (utf8*)"SegmentDragStop" );
const String ListHeaderSegment::EventSegmentDragPositionChanged( (utf8*)"SegmentDragPositionChanged" );
const String ListHeaderSegment::EventSegmentSized( (utf8*)"SegmentSized" );
const String ListHeaderSegment::EventClickableSettingChanged( (utf8*)"ClickableSettingChanged" );

// Defaults
const float	ListHeaderSegment::DefaultSizingArea	= 8.0f;
const float	ListHeaderSegment::SegmentMoveThreshold	= 12.0f;


/*************************************************************************
	Constructor for list header segment base class	
*************************************************************************/
ListHeaderSegment::ListHeaderSegment(const String& type, const String& name) :
	Window(type, name),
	d_sizingMouseCursor(NULL),
	d_movingMouseCursor(NULL),
	d_splitterSize(DefaultSizingArea),
	d_splitterHover(false),
	d_dragSizing(false),
	d_sortDir(None),
	d_segmentHover(false),
	d_segmentPushed(false),
	d_sizingEnabled(true),
	d_movingEnabled(true),
	d_dragMoving(false),
	d_allowClicks(true)
{
	addHeaderSegmentEvents();
	addHeaderSegmentProperties();
}


/*************************************************************************
	Destructor for list header segment base class.	
*************************************************************************/
ListHeaderSegment::~ListHeaderSegment(void)
{
}


/*************************************************************************
	Set whether this segment can be sized.
*************************************************************************/
void ListHeaderSegment::setSizingEnabled(bool setting)
{
	if (d_sizingEnabled != setting)
	{
		d_sizingEnabled = setting;

		// if sizing is now disabled, ensure sizing operation is cancelled
		if (!d_sizingEnabled && d_dragSizing)
		{
			releaseInput();
		}

		WindowEventArgs args(this);
		onSizingSettingChanged(args);
	}

}


/*************************************************************************
	Set the current sort direction set for this segment.	
*************************************************************************/
void ListHeaderSegment::setSortDirection(SortDirection sort_dir)
{
	if (d_sortDir != sort_dir)
	{
		d_sortDir = sort_dir;

		WindowEventArgs args(this);
		onSortDirectionChanged(args);

		requestRedraw();
	}

}


/*************************************************************************
	Set whether drag moving is allowed for this segment.	
*************************************************************************/
void ListHeaderSegment::setDragMovingEnabled(bool setting)
{
	if (d_movingEnabled != setting)
	{
		d_movingEnabled = setting;

		WindowEventArgs args(this);
		onMovableSettingChanged(args);
	}

}


/*************************************************************************
	Set whether the segment is clickable.
*************************************************************************/
void ListHeaderSegment::setClickable(bool setting)
{
	if (d_allowClicks != setting)
	{
		d_allowClicks = setting;

		WindowEventArgs args(this);
		onClickableSettingChanged(args);
	}

}


/*************************************************************************
	Add list header segment specific events
*************************************************************************/
void ListHeaderSegment::addHeaderSegmentEvents(bool bCommon)
{
    if ( bCommon == false )
    {
        addEvent(EventSizingSettingChanged);
        addEvent(EventSegmentDragStart);
        addEvent(EventMovableSettingChanged);
        addEvent(EventClickableSettingChanged);
    }
    else
    {
        addEvent(EventSortDirectionChanged);
        addEvent(EventSegmentSized);
        addEvent(EventSegmentDragStop);
        addEvent(EventSegmentDragPositionChanged);
        addEvent(EventSplitterDoubleClicked);
        addEvent(EventSegmentClicked);
    }
}



/*************************************************************************
	Handler called when segment is clicked.	
*************************************************************************/
void ListHeaderSegment::onSegmentClicked(WindowEventArgs& e)
{
	fireEvent(EventSegmentClicked, e, EventNamespace);
}


/*************************************************************************
	Handler called when the sizer/splitter is double-clicked.	
*************************************************************************/
void ListHeaderSegment::onSplitterDoubleClicked(WindowEventArgs& e)
{
	fireEvent(EventSplitterDoubleClicked, e, EventNamespace);
}


/*************************************************************************
	Handler called when sizing setting changes.
*************************************************************************/
void ListHeaderSegment::onSizingSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventSizingSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the sort direction value changes.
*************************************************************************/
void ListHeaderSegment::onSortDirectionChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventSortDirectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the drag-movable setting is changed.
*************************************************************************/
void ListHeaderSegment::onMovableSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventMovableSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the user starts dragging the segment.
*************************************************************************/
void ListHeaderSegment::onSegmentDragStart(WindowEventArgs& e)
{
	fireEvent(EventSegmentDragStart, e, EventNamespace);
}


/*************************************************************************
	Handler called when the user stops dragging the segment
	(releases mouse button)
*************************************************************************/
void ListHeaderSegment::onSegmentDragStop(WindowEventArgs& e)
{
	fireEvent(EventSegmentDragStop, e, EventNamespace);
}


/*************************************************************************
	Handler called when the drag position changes.
*************************************************************************/
void ListHeaderSegment::onSegmentDragPositionChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventSegmentDragPositionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the segment is sized.
*************************************************************************/
void ListHeaderSegment::onSegmentSized(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventSegmentSized, e, EventNamespace);
}


/*************************************************************************
	Handler called when the clickable setting for the segment changes
*************************************************************************/
void ListHeaderSegment::onClickableSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventClickableSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Processing for drag-sizing the segment
*************************************************************************/
void ListHeaderSegment::doDragSizing(const Point& local_mouse)
{
    float delta = local_mouse.d_x - d_dragPoint.d_x;

    // store this so we can work out how much size actually changed
    float orgWidth = getAbsoluteWidth();

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxWidth(d_maxSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float minWidth(d_minSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float newWidth = orgWidth + delta;

    if (newWidth > maxWidth)
        delta = maxWidth - orgWidth;
    else if (newWidth < minWidth)
        delta = minWidth - orgWidth;
    
    // update segment area rect
    URect area(d_area.d_min.d_x, d_area.d_min.d_y, d_area.d_max.d_x + UDim(0,PixelAligned(delta)), d_area.d_max.d_y);
    setWindowArea_impl(area.d_min, area.getSize());

    // move the dragging point so mouse remains 'attached' to edge of segment
    d_dragPoint.d_x += getAbsoluteWidth() - orgWidth;

    WindowEventArgs args(this);
    onSegmentSized(args);
}


/*************************************************************************
	Processing for drag-moving the segment
*************************************************************************/
void ListHeaderSegment::doDragMoving(const Point& local_mouse)
{
	// calculate movement deltas.
	float	deltaX = local_mouse.d_x - d_dragPoint.d_x;
	float	deltaY = local_mouse.d_y - d_dragPoint.d_y;

	// update 'ghost' position
	d_dragPosition.d_x += deltaX;
	d_dragPosition.d_y += deltaY;

	// update drag point.
	d_dragPoint.d_x += deltaX;
	d_dragPoint.d_y += deltaY;

	WindowEventArgs args(this);
	onSegmentDragPositionChanged(args);
}


/*************************************************************************
	Initialise and enter the drag moving state.
*************************************************************************/
void ListHeaderSegment::initDragMoving(void)
{
	if (d_movingEnabled)
	{
		// initialise drag moving state
		d_dragMoving = true;
		d_segmentPushed = false;
		d_segmentHover = false;
		d_dragPosition.d_x = 0.0f;
		d_dragPosition.d_y = 0.0f;

		// setup new cursor
		MouseCursor::getSingleton().setImage(d_movingMouseCursor);

		// Trigger the event
		WindowEventArgs args(this);
		onSegmentDragStart(args);
	}

}


/*************************************************************************
	Initialise the state for hovering over sizing area.
*************************************************************************/
void ListHeaderSegment::initSizingHoverState(void)
{
	// only react if settings are changing.
	if (!d_splitterHover  && !d_segmentPushed)
	{
		d_splitterHover = true;

		// change the mouse cursor.
		MouseCursor::getSingleton().setImage(d_sizingMouseCursor);

		// trigger redraw so 'sizing' area can be highlighted if needed.
		requestRedraw();
	}

	// reset segment hover as needed.
	if (d_segmentHover)
	{	
		d_segmentHover = false;
		requestRedraw();
	}

}


/*************************************************************************
	Initialise the state for hovering over main segment area
*************************************************************************/
void ListHeaderSegment::initSegmentHoverState(void)
{
	// reset sizing area hover state if needed.
	if (d_splitterHover)
	{
		d_splitterHover = false;
		MouseCursor::getSingleton().setImage(getMouseCursor());
		requestRedraw();
	}

	// set segment hover state if not already set.
	if ((!d_segmentHover) && isClickable())
	{
		d_segmentHover = true;
		requestRedraw();
	}
}


/*************************************************************************
	Return true if move threshold for initiating drag-moving has been
	exceeded.
*************************************************************************/
bool ListHeaderSegment::isDragMoveThresholdExceeded(const Point& local_mouse)
{
	// see if mouse has moved far enough to start move operation
	// calculate movement deltas.
	float	deltaX = local_mouse.d_x - d_dragPoint.d_x;
	float	deltaY = local_mouse.d_y - d_dragPoint.d_y;

	if ((deltaX > SegmentMoveThreshold) || (deltaX < -SegmentMoveThreshold) ||
		(deltaY > SegmentMoveThreshold) || (deltaY < -SegmentMoveThreshold))
	{
		return true;
	}
	else
	{
		return false;
	}

}


/*************************************************************************
	Handler for when mouse position changes in widget area (or captured)
*************************************************************************/
void ListHeaderSegment::onMouseMove(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseMove(e);

	//
	// convert mouse position to something local
	//
	Point localMousePos(screenToWindow(e.position));

	if (getMetricsMode() == Relative)
	{
		localMousePos = relativeToAbsolute(localMousePos);
	}

	// handle drag sizing
	if (d_dragSizing)
	{
		doDragSizing(localMousePos);
	}
	// handle drag moving
	else if (d_dragMoving)
	{
		doDragMoving(localMousePos);
	}
	// not sizing, is mouse in the widget area?
	else if (isHit(e.position))
	{
		// mouse in sizing area & sizing is enabled
		if ((localMousePos.d_x > (getAbsoluteWidth() - d_splitterSize)) && d_sizingEnabled)
		{
			initSizingHoverState();
		}
		// mouse not in sizing area and/or sizing not enabled
		else
		{
			initSegmentHoverState();

			// if we are pushed but not yet drag moving
			if (d_segmentPushed && !d_dragMoving)
			{
				if (isDragMoveThresholdExceeded(localMousePos))
				{
					initDragMoving();
				}

			}

		}

	}
	// mouse is no longer within the widget area...
	else
	{
		// only change settings if change is required
		if (d_splitterHover)
		{
			d_splitterHover = false;
			MouseCursor::getSingleton().setImage(getMouseCursor());
			requestRedraw();
		}

		// reset segment hover state if not already done.
		if (d_segmentHover)
		{	
			d_segmentHover = false;
			requestRedraw();
		}

	}

	e.handled = true;
}


/*************************************************************************
	Handler for when mouse buttons are pushed
*************************************************************************/
void ListHeaderSegment::onMouseButtonDown(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		// ensure all inputs come to us for now
		if (captureInput())
		{
			// get position of mouse as co-ordinates local to this window.
			Point localPos(screenToWindow(e.position));

			if (getMetricsMode() == Relative)
			{
				localPos = relativeToAbsolute(localPos);
			}

			// store drag point for possible sizing or moving operation.
			d_dragPoint = localPos;

			// if the mouse is in the sizing area
			if (d_splitterHover)
			{
				if (isSizingEnabled())
				{
					// setup the 'dragging' state variables
					d_dragSizing = true;
				}

			}
			else
			{
				d_segmentPushed = true;
			}

		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for when mouse buttons area released
*************************************************************************/
void ListHeaderSegment::onMouseButtonUp(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		// if we were pushed and mouse was released within our segment area
		if (d_segmentPushed && d_segmentHover)
		{
			WindowEventArgs args(this);
			onSegmentClicked(args);
		}
		else if (d_dragMoving)
		{
			MouseCursor::getSingleton().setImage(getMouseCursor());
			
			WindowEventArgs args(this);
			onSegmentDragStop(args);
		}

		// release our capture on the input data
		releaseInput();
		e.handled = true;
	}

}


/*************************************************************************
	Handler for when a mouse button is double-clicked
*************************************************************************/
void ListHeaderSegment::onMouseDoubleClicked(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseDoubleClicked(e);

	// if double-clicked on splitter / sizing area
	if ((e.button == LeftButton) && d_splitterHover)
	{
		WindowEventArgs args(this);
		onSplitterDoubleClicked(args);

		e.handled = true;
	}

}


/*************************************************************************
	Handler for when mouse leaves the widget area (uncaptured)
*************************************************************************/
void ListHeaderSegment::onMouseLeaves(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseLeaves(e);

	d_splitterHover = false;
	d_dragSizing = false;
	d_segmentHover = false;
	requestRedraw();
}


/*************************************************************************
	Handler for when mouse input capture is lost
*************************************************************************/
void ListHeaderSegment::onCaptureLost(WindowEventArgs& e)
{
	// base class processing
	Window::onCaptureLost(e);

	// reset segment state
	d_dragSizing = false;
	d_segmentPushed = false;
	d_dragMoving = false;

	e.handled = true;
}

/*************************************************************************
	adds properties for the class
*************************************************************************/
void ListHeaderSegment::addHeaderSegmentProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_clickableProperty);
        addProperty(&d_sizableProperty);
        addProperty(&d_dragableProperty);
        addProperty(&d_sortDirectionProperty);
    }
}



} // End of  CEGUI namespace section

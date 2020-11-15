/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of ListHeader widget base class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/widgets/ListHeader.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/CoordConverter.h"
#include <sstream>


// Start of CEGUI namespace section
namespace CEGUI
{
const String ListHeader::EventNamespace("ListHeader");
const String ListHeader::WidgetTypeName("CEGUI/ListHeader");

/*************************************************************************
    ListHeaderWindowRenderer constructor
*************************************************************************/
ListHeaderWindowRenderer::ListHeaderWindowRenderer(const String& name) :
    WindowRenderer(name, "ListHeader")
{
}
/*************************************************************************
	Constants
*************************************************************************/
// Event names
const String ListHeader::EventSortColumnChanged( "SortColumnChanged" );
const String ListHeader::EventSortDirectionChanged( "SortDirectionChanged" );
const String ListHeader::EventSegmentSized( "SegmentSized" );
const String ListHeader::EventSegmentClicked( "SegmentClicked" );
const String ListHeader::EventSplitterDoubleClicked( "SplitterDoubleClicked" );
const String ListHeader::EventSegmentSequenceChanged( "SegmentSequenceChanged" );
const String ListHeader::EventSegmentAdded( "SegmentAdded" );
const String ListHeader::EventSegmentRemoved( "SegmentRemoved" );
const String ListHeader::EventSortSettingChanged( "SortSettingChanged" );
const String ListHeader::EventDragMoveSettingChanged( "DragMoveSettingChanged" );
const String ListHeader::EventDragSizeSettingChanged( "DragSizeSettingChanged" );
const String ListHeader::EventSegmentRenderOffsetChanged( "SegmentRenderOffsetChanged" );

// values
const float	ListHeader::ScrollSpeed	= 8.0f;
const float	ListHeader::MinimumSegmentPixelWidth	= 20.0f;

/*************************************************************************
    Child Widget name suffix constants
*************************************************************************/
const String ListHeader::SegmentNameSuffix("__auto_seg_");



/*************************************************************************
	Constructor for the list header base class.
*************************************************************************/
ListHeader::ListHeader(const String& type, const String& name) :
	Window(type, name),
	d_sortSegment(0),
	d_sizingEnabled(true),
	d_sortingEnabled(true),
	d_movingEnabled(true),
	d_uniqueIDNumber(0),
	d_segmentOffset(0.0f),
	d_sortDir(ListHeaderSegment::None)
{
	addHeaderProperties();
}


/*************************************************************************
	Destructor for the list header base class.
*************************************************************************/
ListHeader::~ListHeader(void)
{
}


/*************************************************************************
	Return the current number of segments in the header.
*************************************************************************/
uint ListHeader::getColumnCount(void) const
{
	return (uint)d_segments.size();
}


/*************************************************************************
	Given a zero based column index, return the ListHeaderSegment.
*************************************************************************/
ListHeaderSegment& ListHeader::getSegmentFromColumn(uint column) const
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "requested column index is out of range for this ListHeader."));
	}
	else
	{
		return *d_segments[column];
	}
}

/*************************************************************************
	Return the ListHeaderSegment with the requested ID.
*************************************************************************/
ListHeaderSegment& ListHeader::getSegmentFromID(uint id) const
{
	for (uint i = 0; i < getColumnCount(); ++i)
	{
		if (d_segments[i]->getID() == id)
		{
			return *d_segments[i];
		}

	}

	// No such segment found, throw exception
	CEGUI_THROW(InvalidRequestException(
        "no segment with the requested ID is attached to this ListHeader."));
}


/*************************************************************************
	Return the sort-key segment.
*************************************************************************/
ListHeaderSegment& ListHeader::getSortSegment(void) const
{
	if (!d_sortSegment)
	{
		CEGUI_THROW(InvalidRequestException(
            "Sort segment was invalid!  (No segments are attached to the ListHeader?)"));
	}
	else
	{
		return *d_sortSegment;
	}

}
/*************************************************************************
	Return the sort-key segment.
*************************************************************************/
uint ListHeader::getSortSegmentID(void) const
{
	return getSortSegment().getID();
}

/*************************************************************************
	Given a segment, return it's zero based column index.
*************************************************************************/
uint ListHeader::getColumnFromSegment(const ListHeaderSegment& segment) const
{
	for (uint i = 0; i < getColumnCount(); ++i)
	{
		if (d_segments[i] == &segment)
		{
			return i;
		}

	}

	// No such segment found, throw exception
	CEGUI_THROW(InvalidRequestException(
        "the given ListHeaderSegment is not attached to this ListHeader."));
}


/*************************************************************************
	Return the column index that has a segment with the requested ID.
*************************************************************************/
uint ListHeader::getColumnFromID(uint id) const
{
	for (uint i = 0; i < getColumnCount(); ++i)
	{
		if (d_segments[i]->getID() == id)
		{
			return i;
		}

	}

	// No such segment found, throw exception
	CEGUI_THROW(InvalidRequestException(
        "no column with the requested ID is available on this ListHeader."));
}


/*************************************************************************
	Return the current sort-key column
*************************************************************************/
uint ListHeader::getSortColumn(void) const
{
	return getColumnFromSegment(getSortSegment());
}


/*************************************************************************
	return the zero based column index of the segment with the requested
	text.
*************************************************************************/
uint ListHeader::getColumnWithText(const String& text) const
{
	for (uint i = 0; i < getColumnCount(); ++i)
	{
		if (d_segments[i]->getText() == text)
		{
			return i;
		}

	}

	// No such segment found, throw exception
	CEGUI_THROW(InvalidRequestException(
        "no column with the text '" + text + "' is attached to this ListHeader."));
}


/*************************************************************************
	return the pixel offset to the given segment
*************************************************************************/
float ListHeader::getPixelOffsetToSegment(const ListHeaderSegment& segment) const
{
	float offset = 0.0f;

	for (uint i = 0; i < getColumnCount(); ++i)
	{
		if (d_segments[i] == &segment)
		{
			return offset;
		}

		offset += d_segments[i]->getPixelSize().d_width;
	}

	// No such segment found, throw exception
	CEGUI_THROW(InvalidRequestException(
        "the given ListHeaderSegment is not attached to this ListHeader."));
}


/*************************************************************************
	return the pixel offset to the segment with the given column index
*************************************************************************/
float ListHeader::getPixelOffsetToColumn(uint column) const
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "requested column index is out of range for this ListHeader."));
	}
	else
	{
		float offset = 0.0f;

		for (uint i = 0; i < column; ++i)
		{
			offset += d_segments[i]->getPixelSize().d_width;
		}

		return offset;
	}

}


/*************************************************************************
	Return the total pixel width of all segments
*************************************************************************/
float ListHeader::getTotalSegmentsPixelExtent(void) const
{
	float extent = 0.0f;

	for (uint i = 0; i < getColumnCount(); ++i)
	{
		extent += d_segments[i]->getPixelSize().d_width;
	}

	return extent;
}


/*************************************************************************
	Return the width of the segment at the specified column index.
*************************************************************************/
UDim ListHeader::getColumnWidth(uint column) const
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "requested column index is out of range for this ListHeader."));
	}
	else
	{
		return d_segments[column]->getWidth();
	}

}


/*************************************************************************
	return the current sort direction
*************************************************************************/
ListHeaderSegment::SortDirection ListHeader::getSortDirection(void) const
{
	return d_sortDir;
}


/*************************************************************************
	Return whether sorting is enabled for this header.
*************************************************************************/
bool ListHeader::isSortingEnabled(void) const
{
	return d_sortingEnabled;
}


/*************************************************************************
	Return whether segment sizing is enabled for this header
*************************************************************************/
bool ListHeader::isColumnSizingEnabled(void) const
{
	return d_sizingEnabled;
}


/*************************************************************************
	Return whether segment dragging is enabled for this header.
*************************************************************************/
bool ListHeader::isColumnDraggingEnabled(void) const
{
	return d_movingEnabled;
}


/*************************************************************************
	Set whether the ability to change sort segment / direction is enabled
*************************************************************************/
void ListHeader::setSortingEnabled(bool setting)
{
	if (d_sortingEnabled != setting)
	{
		d_sortingEnabled = setting;

		// make the setting change for all component segments.
		for (uint i = 0; i <getColumnCount(); ++i)
		{
			d_segments[i]->setClickable(d_sortingEnabled);
		}

		// Fire setting changed event.
		WindowEventArgs args(this);
		onSortSettingChanged(args);
	}

}


/*************************************************************************
	Set the sort direction on the current sort segment / column.
*************************************************************************/
void ListHeader::setSortDirection(ListHeaderSegment::SortDirection direction)
{
	if (d_sortDir != direction)
	{
		d_sortDir = direction;

		// set direction of current sort segment
		if (d_sortSegment)
		{
			d_sortSegment->setSortDirection(direction);
		}

		// Fire sort direction changed event.
		WindowEventArgs args(this);
		onSortDirectionChanged(args);
	}

}


/*************************************************************************
	Set the current sort segment.
*************************************************************************/
void ListHeader::setSortSegment(const ListHeaderSegment& segment)
{
	setSortColumn(getColumnFromSegment(segment));
}


/*************************************************************************
	Set the current sort segment via column index.
*************************************************************************/
void ListHeader::setSortColumn(uint column)
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "specified column index is out of range for this ListHeader."));
	}
	else
	{
		// if column is different to current sort segment
		if (d_sortSegment != d_segments[column])
		{
			// set sort direction on 'old' sort segment to none.
			if (d_sortSegment)
			{
				d_sortSegment->setSortDirection(ListHeaderSegment::None);
			}

			// set-up new sort segment
			d_sortSegment = d_segments[column];
			d_sortSegment->setSortDirection(d_sortDir);

			// Fire sort column changed event
			WindowEventArgs args(this);
			onSortColumnChanged(args);
		}

	}

}


/*************************************************************************
	Set the current sort segment via ID code.
*************************************************************************/
void ListHeader::setSortColumnFromID(uint id)
{
	setSortSegment(getSegmentFromID(id));
}


/*************************************************************************
	Set whether or not segments may be sized.
*************************************************************************/
void ListHeader::setColumnSizingEnabled(bool setting)
{
	if (d_sizingEnabled != setting)
	{
		d_sizingEnabled = setting;

		// make the setting change for all component segments.
		for (uint i = 0; i <getColumnCount(); ++i)
		{
			d_segments[i]->setSizingEnabled(d_sizingEnabled);
		}

		// Fire setting changed event.
		WindowEventArgs args(this);
		onDragSizeSettingChanged(args);
	}

}


/*************************************************************************
	Set whether columns may be dragged into new orders.
*************************************************************************/
void ListHeader::setColumnDraggingEnabled(bool setting)
{
	if (d_movingEnabled != setting)
	{
		d_movingEnabled = setting;

		// make the setting change for all component segments.
		for (uint i = 0; i <getColumnCount(); ++i)
		{
			d_segments[i]->setDragMovingEnabled(d_movingEnabled);
		}

		// Fire setting changed event.
		WindowEventArgs args(this);
		onDragMoveSettingChanged(args);
	}

}


/*************************************************************************
	Add a new column segment to the header.
*************************************************************************/
void ListHeader::addColumn(const String& text, uint id, const UDim& width)
{
	// add just inserts at end.
	insertColumn(text, id, width, getColumnCount());
}


/*************************************************************************
	Insert a new column segment into the header
*************************************************************************/
void ListHeader::insertColumn(const String& text, uint id, const UDim& width, uint position)
{
	// if position is too big, insert at end.
	if (position > getColumnCount())
	{
		position = getColumnCount();
	}

	ListHeaderSegment* seg = createInitialisedSegment(text, id, width);
	d_segments.insert((d_segments.begin() + position), seg);

	// add window as a child of this
	addChild(seg);

	layoutSegments();

	// Fire segment added event.
	WindowEventArgs args(this);
	onSegmentAdded(args);

	// if sort segment is invalid, make it valid now we have a segment attached
	if (!d_sortSegment)
	{
		setSortColumn(position);
	}
}


/*************************************************************************
	Remove a column from the header
*************************************************************************/
void ListHeader::removeColumn(uint column)
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "specified column index is out of range for this ListHeader."));
	}
	else
	{
		ListHeaderSegment* seg = d_segments[column];

		// remove from the list of segments
		d_segments.erase(d_segments.begin() + column);

		// have we removed the sort column?
		if (d_sortSegment == seg)
		{
			// any other columns?
			if (getColumnCount() > 0)
			{
				// put first column in as sort column
				d_sortDir = ListHeaderSegment::None;
				setSortColumn(0);
			}
			// no columns, set sort segment to NULL
			else
			{
				d_sortSegment = 0;
			}

		}

		// detach segment window from the header (this)
		removeChild(seg);

		// destroy the segment (done in derived class, since that's where it was created).
		destroyListSegment(seg);

		layoutSegments();

		// Fire segment removed event.
		WindowEventArgs args(this);
		onSegmentRemoved(args);
	}

}


/*************************************************************************
	Move a column segment to a new position
*************************************************************************/
void ListHeader::moveColumn(uint column, uint position)
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "specified column index is out of range for this ListHeader."));
	}
	else
	{
		// if position is too big, move to end.
		if (position >= getColumnCount())
		{
			position = getColumnCount() - 1;
		}

		ListHeaderSegment* seg = d_segments[column];

		// remove original copy of segment
		d_segments.erase(d_segments.begin() + column);

		// insert the segment at it's new position
		d_segments.insert(d_segments.begin() + position, seg);

		// Fire sequence changed event
		HeaderSequenceEventArgs args(this, column, position);
		onSegmentSequenceChanged(args);

		layoutSegments();
	}

}


/*************************************************************************
	Insert a new column segment into the header
*************************************************************************/
void ListHeader::insertColumn(const String& text, uint id, const UDim& width, const ListHeaderSegment& position)
{
	insertColumn(text, id, width, getColumnFromSegment(position));
}


/*************************************************************************
	Remove a segment from the header
*************************************************************************/
void ListHeader::removeSegment(const ListHeaderSegment& segment)
{
	removeColumn(getColumnFromSegment(segment));
}


/*************************************************************************
	Move a column segment to a new position.
*************************************************************************/
void ListHeader::moveColumn(uint column, const ListHeaderSegment& position)
{
	moveColumn(column, getColumnFromSegment(position));
}


/*************************************************************************
	Move a segment to a new position
*************************************************************************/
void ListHeader::moveSegment(const ListHeaderSegment& segment, uint position)
{
	moveColumn(getColumnFromSegment(segment), position);
}


/*************************************************************************
	Move a segment to a new position
*************************************************************************/
void ListHeader::moveSegment(const ListHeaderSegment& segment, const ListHeaderSegment& position)
{
	moveColumn(getColumnFromSegment(segment), getColumnFromSegment(position));
}


/*************************************************************************
	Set the current segment offset value (metrics dependant)
*************************************************************************/
void ListHeader::setSegmentOffset(float offset)
{
	if (d_segmentOffset != offset)
	{
		d_segmentOffset = offset;
		layoutSegments();
		invalidate();
	
		// Fire event.
		WindowEventArgs args(this);
		onSegmentOffsetChanged(args);
	}

}


/*************************************************************************
	Set the width of the specified column.
*************************************************************************/
void ListHeader::setColumnWidth(uint column, const UDim& width)
{
	if (column >= getColumnCount())
	{
		CEGUI_THROW(InvalidRequestException(
            "specified column index is out of range for this ListHeader."));
	}
	else
	{
		d_segments[column]->setSize(USize(width, d_segments[column]->getSize().d_height));

		layoutSegments();

		// Fire segment sized event.
		WindowEventArgs args(d_segments[column]);
		onSegmentSized(args);
	}

}


/*************************************************************************
	Create initialise and return a ListHeaderSegment object, with all
	events subscribed and ready to use.
*************************************************************************/
ListHeaderSegment* ListHeader::createInitialisedSegment(const String& text, uint id, const UDim& width)
{
	// Build unique name
	std::stringstream name;
	name << SegmentNameSuffix << d_uniqueIDNumber;

	// create segment.
	ListHeaderSegment* newseg = createNewSegment(name.str().c_str());
	d_uniqueIDNumber++;

	// setup segment;
	newseg->setSize(USize(width, cegui_reldim(1.0f)));
	newseg->setMinSize(USize(cegui_absdim(MinimumSegmentPixelWidth), cegui_absdim(0)));
	newseg->setText(text);
	newseg->setID(id);
    newseg->setSizingEnabled(d_sizingEnabled);
    newseg->setDragMovingEnabled(d_movingEnabled);
    newseg->setClickable(d_sortingEnabled);

	// subscribe events we listen to
	newseg->subscribeEvent(ListHeaderSegment::EventSegmentSized, Event::Subscriber(&CEGUI::ListHeader::segmentSizedHandler, this));
	newseg->subscribeEvent(ListHeaderSegment::EventSegmentDragStop, Event::Subscriber(&CEGUI::ListHeader::segmentMovedHandler, this));
	newseg->subscribeEvent(ListHeaderSegment::EventSegmentClicked, Event::Subscriber(&CEGUI::ListHeader::segmentClickedHandler, this));
	newseg->subscribeEvent(ListHeaderSegment::EventSplitterDoubleClicked, Event::Subscriber(&CEGUI::ListHeader::segmentDoubleClickHandler, this));
	newseg->subscribeEvent(ListHeaderSegment::EventSegmentDragPositionChanged, Event::Subscriber(&CEGUI::ListHeader::segmentDragHandler, this));

	return newseg;
}


/*************************************************************************
	Layout the segments
*************************************************************************/
void ListHeader::layoutSegments(void)
{
	UVector2 pos(cegui_absdim(-d_segmentOffset), cegui_absdim(0.0f));

	for (uint i = 0; i < getColumnCount(); ++i)
	{
		d_segments[i]->setPosition(pos);
		pos.d_x += d_segments[i]->getWidth();
	}

}

bool ListHeader::validateWindowRenderer(const WindowRenderer* renderer) const
{
	return dynamic_cast<const ListHeaderWindowRenderer*>(renderer) != 0;
}

/*************************************************************************
	Handler called when the sort column is changed.
*************************************************************************/
void ListHeader::onSortColumnChanged(WindowEventArgs& e)
{
	fireEvent(EventSortColumnChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the sort direction is changed.
*************************************************************************/
void ListHeader::onSortDirectionChanged(WindowEventArgs& e)
{
	fireEvent(EventSortDirectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when a segment is sized by the user.
	e.window points to the segment.
*************************************************************************/
void ListHeader::onSegmentSized(WindowEventArgs& e)
{
	fireEvent(EventSegmentSized, e, EventNamespace);
}


/*************************************************************************
	Handler called when a segment is clicked by the user.
	e.window points to the segment.
*************************************************************************/
void ListHeader::onSegmentClicked(WindowEventArgs& e)
{
	fireEvent(EventSegmentClicked, e, EventNamespace);
}


/*************************************************************************
	Handler called when a segment splitter / sizer is double-clicked.
	e.window points to the segment.
*************************************************************************/
void ListHeader::onSplitterDoubleClicked(WindowEventArgs& e)
{
	fireEvent(EventSplitterDoubleClicked, e, EventNamespace);
}


/*************************************************************************
	Handler called when the segment / column order changes.
*************************************************************************/
void ListHeader::onSegmentSequenceChanged(WindowEventArgs& e)
{
	fireEvent(EventSegmentSequenceChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when a new segment is added to the header.
*************************************************************************/
void ListHeader::onSegmentAdded(WindowEventArgs& e)
{
	fireEvent(EventSegmentAdded, e, EventNamespace);
}


/*************************************************************************
	Handler called when a segment is removed from the header.
*************************************************************************/
void ListHeader::onSegmentRemoved(WindowEventArgs& e)
{
	fireEvent(EventSegmentRemoved, e, EventNamespace);
}


/*************************************************************************
	Handler called then setting that controls the users ability to
	modify the search column & direction changes.
*************************************************************************/
void ListHeader::onSortSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventSortSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the setting that controls the users ability to
	drag and drop segments changes.
*************************************************************************/
void ListHeader::onDragMoveSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventDragMoveSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the setting that controls the users ability t
	size segments changes.
*************************************************************************/
void ListHeader::onDragSizeSettingChanged(WindowEventArgs& e)
{
	fireEvent(EventDragSizeSettingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the base rendering offset for the segments
	(scroll position) changes.
*************************************************************************/
void ListHeader::onSegmentOffsetChanged(WindowEventArgs& e)
{
	fireEvent(EventSegmentRenderOffsetChanged, e, EventNamespace);
}


/*************************************************************************
	Handler method for when a segment is sized.
*************************************************************************/
bool ListHeader::segmentSizedHandler(const EventArgs& e)
{
	layoutSegments();

	// Fire segment sized event.
	WindowEventArgs args(((WindowEventArgs&)e).window);
	onSegmentSized(args);

	return true;
}


/*************************************************************************
	Handler method for when a segment is dragged & dropped.
*************************************************************************/
bool ListHeader::segmentMovedHandler(const EventArgs& e)
{
	const Vector2f mousePos(getUnprojectedPosition(
        getGUIContext().getMouseCursor().getPosition()));

	// segment must be dropped within the window
	if (isHit(mousePos))
	{
		// get mouse position as something local
		Vector2f localMousePos(CoordConverter::screenToWindow(*this, mousePos));

		// set up to allow for current offsets
		float currwidth = -d_segmentOffset;

		// calculate column where dragged segment was dropped
        uint col;
		for (col = 0; col < getColumnCount(); ++col)
		{
			currwidth += d_segments[col]->getPixelSize().d_width;

			if (localMousePos.d_x < currwidth)
			{
				// this is the column, exit loop early
				break;
			}

		}

		// find original column for dragged segment.
		ListHeaderSegment* seg = ((ListHeaderSegment*)((WindowEventArgs&)e).window);
		uint curcol = getColumnFromSegment(*seg);

		// move column
		moveColumn(curcol, col);
	}

	return true;
}


/*************************************************************************
	Hanlder for when a segment is clicked (to change sort segment / direction)
*************************************************************************/
bool ListHeader::segmentClickedHandler(const EventArgs& e)
{
	// double-check we allow this action
	if (d_sortingEnabled)
	{
		ListHeaderSegment* seg = ((ListHeaderSegment*)((WindowEventArgs&)e).window);

		// is this a new sort column?
		if (d_sortSegment != seg)
		{
			d_sortDir = ListHeaderSegment::Descending;
			setSortSegment(*seg);
		}
		// not a new segment, toggle current direction
		else if (d_sortSegment)
		{
			ListHeaderSegment::SortDirection currDir = d_sortSegment->getSortDirection();

			// set new direction based on the current value.
			switch (currDir)
			{
			case ListHeaderSegment::None:
				setSortDirection(ListHeaderSegment::Descending);
				break;

			case ListHeaderSegment::Ascending:
				setSortDirection(ListHeaderSegment::Descending);
				break;

			case ListHeaderSegment::Descending:
				setSortDirection(ListHeaderSegment::Ascending);
				break;
			}

		}

		// Notify that a segment has been clicked
		WindowEventArgs args(((WindowEventArgs&)e).window);
		onSegmentClicked(args);
	}

	return true;
}


/*************************************************************************
	Handler called when a segment splitter is double-clicked.
*************************************************************************/
bool ListHeader::segmentDoubleClickHandler(const EventArgs& e)
{
	WindowEventArgs args(((WindowEventArgs&)e).window);
	onSplitterDoubleClicked(args);

	return true;
}


/*************************************************************************
	Handler called whenever the mouse moves while dragging a segment
*************************************************************************/
bool ListHeader::segmentDragHandler(const EventArgs&)
{
	// what we do here is monitor the position and scroll if we can when mouse is outside area.

	// get mouse position as something local
    const Vector2f localMousePos(CoordConverter::screenToWindow(*this,
        getUnprojectedPosition(getGUIContext().
            getMouseCursor().getPosition())));

	// scroll left?
	if (localMousePos.d_x < 0.0f)
	{
		if (d_segmentOffset > 0.0f)
		{
			setSegmentOffset(ceguimax(0.0f, d_segmentOffset - ScrollSpeed));
		}
	}
	// scroll right?
	else if (localMousePos.d_x >= d_pixelSize.d_width)
	{
		float maxOffset = ceguimax(0.0f, getTotalSegmentsPixelExtent() - d_pixelSize.d_width);

		// if we have not scrolled to the limit
		if (d_segmentOffset < maxOffset)
		{
			// scroll, but never beyond the limit
			setSegmentOffset(ceguimin(maxOffset, d_segmentOffset + ScrollSpeed));
		}

	}

	return true;
}


/*************************************************************************
	Add ListHeader specific properties
*************************************************************************/
void ListHeader::addHeaderProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(ListHeader, bool,
        "SortSettingEnabled", "Property to get/set the setting for for user modification of the sort column & direction.  Value is either \"true\" or \"false\".",
        &ListHeader::setSortingEnabled, &ListHeader::isSortingEnabled, true /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ListHeader, bool,
        "ColumnsSizable", "Property to get/set the setting for user sizing of the column headers.  Value is either \"true\" or \"false\".",
        &ListHeader::setColumnSizingEnabled, &ListHeader::isColumnSizingEnabled, true /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ListHeader, bool,
        "ColumnsMovable", "Property to get/set the setting for user moving of the column headers.  Value is either \"true\" or \"false\".",
        &ListHeader::setColumnDraggingEnabled, &ListHeader::isColumnDraggingEnabled, true /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ListHeader, uint,
        "SortColumnID", "Property to get/set the current sort column (via ID code).  Value is an unsigned integer number.",
        &ListHeader::setSortColumnFromID, &ListHeader::getSortSegmentID, 0 /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(ListHeader, ListHeaderSegment::SortDirection,
        "SortDirection", "Property to get/set the sort direction setting of the header.  Value is the text of one of the SortDirection enumerated value names.",
        &ListHeader::setSortDirection, &ListHeader::getSortDirection, ListHeaderSegment::None
    );
}

/*************************************************************************
    Create new segment
*************************************************************************/
ListHeaderSegment* ListHeader::createNewSegment(const String& name) const
{
    if (d_windowRenderer != 0)
    {
        ListHeaderWindowRenderer* wr = (ListHeaderWindowRenderer*)d_windowRenderer;
        return wr->createNewSegment(name);
    }
    else
    {
        //return createNewSegment_impl(name);
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

/*************************************************************************
    Destroy segment
*************************************************************************/
void ListHeader::destroyListSegment(ListHeaderSegment* segment) const
{
    if (d_windowRenderer != 0)
    {
        ListHeaderWindowRenderer* wr = (ListHeaderWindowRenderer*)d_windowRenderer;
        wr->destroyListSegment(segment);
    }
    else
    {
        //return destroyListSegment_impl(segment);
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

} // End of  CEGUI namespace section

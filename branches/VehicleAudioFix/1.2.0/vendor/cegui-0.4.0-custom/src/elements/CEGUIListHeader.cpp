/************************************************************************
	filename: 	CEGUIListHeader.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of ListHeader widget base class
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
#include "elements/CEGUIListHeader.h"
#include "CEGUIExceptions.h"

#include <sstream>


// Start of CEGUI namespace section
namespace CEGUI
{
const String ListHeader::EventNamespace("ListHeader");

/*************************************************************************
	Properties for this class
*************************************************************************/
ListHeaderProperties::SortSettingEnabled	ListHeader::d_sortSettingProperty;
ListHeaderProperties::ColumnsSizable		ListHeader::d_sizableProperty;
ListHeaderProperties::ColumnsMovable		ListHeader::d_movableProperty;
ListHeaderProperties::SortColumnID			ListHeader::d_sortColumnIDProperty;
ListHeaderProperties::SortDirection			ListHeader::d_sortDirectionProperty;


/*************************************************************************
	Constants
*************************************************************************/
// Event names
const String ListHeader::EventSortColumnChanged( (utf8*)"SortColumnChanged" );
const String ListHeader::EventSortDirectionChanged( (utf8*)"SortDirectionChanged" );
const String ListHeader::EventSegmentSized( (utf8*)"SegmentSized" );
const String ListHeader::EventSegmentClicked( (utf8*)"SegmentClicked" );
const String ListHeader::EventSplitterDoubleClicked( (utf8*)"SplitterDoubleClicked" );
const String ListHeader::EventSegmentSequenceChanged( (utf8*)"SegmentSequenceChanged" );
const String ListHeader::EventSegmentAdded( (utf8*)"SegmentAdded" );
const String ListHeader::EventSegmentRemoved( (utf8*)"SegmentRemoved" );
const String ListHeader::EventSortSettingChanged( (utf8*)"SortSettingChanged" );
const String ListHeader::EventDragMoveSettingChanged( (utf8*)"DragMoveSettingChanged" );
const String ListHeader::EventDragSizeSettingChanged( (utf8*)"DragSizeSettingChanged" );
const String ListHeader::EventSegmentRenderOffsetChanged( (utf8*)"SegmentOffsetChanged" );

// values
const float	ListHeader::ScrollSpeed	= 8.0f;
const float	ListHeader::MinimumSegmentPixelWidth	= 20.0f;


/*************************************************************************
	Constructor for the list header base class.
*************************************************************************/
ListHeader::ListHeader(const String& type, const String& name) :
	Window(type, name),
	d_sortSegment(NULL),
	d_sizingEnabled(true),
	d_sortingEnabled(true),
	d_movingEnabled(true),
	d_uniqueIDNumber(0),
	d_segmentOffset(0.0f),
	d_sortDir(ListHeaderSegment::None)
{
	addListHeaderEvents();
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
		throw InvalidRequestException((utf8*)"ListHeader::getSegmentFromColumn - requested column index is out of range for this ListHeader.");
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
	throw InvalidRequestException((utf8*)"ListHeader::getSegmentFromID - no segment with the requested ID is attached to this ListHeader.");
}


/*************************************************************************
	Return the sort-key segment.
*************************************************************************/
ListHeaderSegment& ListHeader::getSortSegment(void) const
{
	if (d_sortSegment == NULL)
	{
		throw	InvalidRequestException((utf8*)"ListHeader::getSortSegment - Sort segment was NULL!  (No segments are attached to the ListHeader?)");
	}
	else
	{
		return *d_sortSegment;
	}

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
	throw InvalidRequestException((utf8*)"ListHeader::getColumnFromSegment - the given ListHeaderSegment is not attached to this ListHeader.");
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
	throw InvalidRequestException((utf8*)"ListHeader::getColumnFromID - no column with the requested ID is available on this ListHeader.");
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
	throw InvalidRequestException((utf8*)"ListHeader::getColumnWithText - no column with the text '" + text + "' is attached to this ListHeader.");
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

		offset += d_segments[i]->getAbsoluteWidth();
	}

	// No such segment found, throw exception
	throw InvalidRequestException((utf8*)"ListHeader::getPixelOffsetToSegment - the given ListHeaderSegment is not attached to this ListHeader.");
}


/*************************************************************************
	return the pixel offset to the segment with the given column index
*************************************************************************/
float ListHeader::getPixelOffsetToColumn(uint column) const
{
	if (column >= getColumnCount())
	{
		throw InvalidRequestException((utf8*)"ListHeader::getPixelOffsetToColumn - requested column index is out of range for this ListHeader.");
	}
	else
	{
		float offset = 0.0f;

		for (uint i = 0; i < column; ++i)
		{
			offset += d_segments[i]->getAbsoluteWidth();
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
		extent += d_segments[i]->getAbsoluteWidth();
	}

	return extent;
}


/*************************************************************************
	Return the pixel width of the segment at the specified column index.
*************************************************************************/
float ListHeader::getColumnPixelWidth(uint column) const
{
	if (column >= getColumnCount())
	{
		throw InvalidRequestException((utf8*)"ListHeader::getColumnPixelWidth - requested column index is out of range for this ListHeader.");
	}
	else
	{
		return d_segments[column]->getAbsoluteWidth();
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
	d_sortDir = direction;

	// set direction of current sort segment
	if (d_sortSegment != NULL)
	{
		d_sortSegment->setSortDirection(direction);
	}

	// Fire sort direction changed event.
	WindowEventArgs args(this);
	onSortDirectionChanged(args);
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
		throw InvalidRequestException((utf8*)"ListHeader::setSortColumn - specified column index is out of range for this ListHeader.");
	}
	else
	{
		// set sort direction on 'old' sort segment to none.
		if (d_sortSegment != NULL)
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
void ListHeader::addColumn(const String& text, uint id, float width)
{
	// add just inserts at end.
	insertColumn(text, id, width, getColumnCount());
}


/*************************************************************************
	Insert a new column segment into the header
*************************************************************************/
void ListHeader::insertColumn(const String& text, uint id, float width, uint position)
{
	// if position is too big, insert at end.
	if (position > getColumnCount())
	{
		position = getColumnCount();
	}

	ListHeaderSegment* seg = createInitialisedSegment(text, id, width);
	d_segments.insert((d_segments.begin() + position), seg);

	// add window as a child of this
	addChildWindow(seg);

	layoutSegments();

	// Fire segment added event.
	WindowEventArgs args(this);
	onSegmentAdded(args);

	// if sort segment is invalid, make it valid now we have a segment attached
	if (d_sortSegment == NULL)
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
		throw InvalidRequestException((utf8*)"ListHeader::removeColumn - specified column index is out of range for this ListHeader.");
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
				d_sortSegment = NULL;
			}

		}

		// detach segment window from the header (this)
		removeChildWindow(seg);

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
		throw InvalidRequestException((utf8*)"ListHeader::moveColumn - specified column index is out of range for this ListHeader.");
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
void ListHeader::insertColumn(const String& text, uint id, float width, const ListHeaderSegment& position)
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
		requestRedraw();
	
		// Fire event.
		WindowEventArgs args(this);
		onSegmentOffsetChanged(args);
	}

}


/*************************************************************************
	Set the pixel width of the specified column.	
*************************************************************************/
void ListHeader::setColumnPixelWidth(uint column, float width)
{
	if (column >= getColumnCount())
	{
		throw InvalidRequestException((utf8*)"ListHeader::setColumnPixelWidth - specified column index is out of range for this ListHeader.");
	}
	else
	{
		d_segments[column]->setWidth(Absolute, width);

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
ListHeaderSegment* ListHeader::createInitialisedSegment(const String& text, uint id, float width)
{
	// Build unique name
	std::stringstream name;
	name << getName().c_str() << "__auto_seg_" << d_uniqueIDNumber;

	// create segment.
	ListHeaderSegment* newseg = createNewSegment(name.str());
	d_uniqueIDNumber++;

	// setup segment;
	newseg->setMetricsMode(Relative);
	newseg->setSize(Size(width, 1.0f));
	newseg->setMinimumSize(absoluteToRelative_impl(NULL, Size(MinimumSegmentPixelWidth, 0.0f)));
	newseg->setText(text);
	newseg->setID(id);

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
	Point pos(-d_segmentOffset, 0.0f);

	for (uint i = 0; i < getColumnCount(); ++i)
	{
		d_segments[i]->setPosition(pos);
		pos.d_x += d_segments[i]->getWidth();
	}

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
	Point mousePos(MouseCursor::getSingleton().getPosition());

	// segment must be dropped within the window
	if (isHit(mousePos))
	{
		// get mouse position as something local
		Point localMousePos(screenToWindow(mousePos));

		// set up to allow for current offsets
		float currwidth = -d_segmentOffset;

		// get required figures as pixels
		if (getMetricsMode() == Relative)
		{
			localMousePos = relativeToAbsolute(localMousePos);
			currwidth = relativeToAbsoluteX(currwidth);
		}

		// calculate column where dragged segment was dropped
        uint col;
		for (col = 0; col < getColumnCount(); ++col)
		{
			currwidth += d_segments[col]->getAbsoluteWidth();

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
		else if (d_sortSegment != NULL)
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
bool ListHeader::segmentDragHandler(const EventArgs& e)
{
	// what we do here is monitor the position and scroll if we can when mouse is outside area.

	// get mouse position as something local
	Point localMousePos(screenToWindow(MouseCursor::getSingleton().getPosition()));

	MetricsMode mmode = getMetricsMode();

	// get required figures as pixels
	if (mmode == Relative)
	{
		localMousePos = relativeToAbsolute(localMousePos);
	}

	// scroll left?
	if (localMousePos.d_x < 0.0f)
	{
		if (d_segmentOffset > 0.0f)
		{
			float adjust = ScrollSpeed;
			
			if (mmode == Relative)
			{
				adjust = absoluteToRelativeX(adjust);
			}
			
			setSegmentOffset(ceguimax(0.0f, d_segmentOffset - adjust));
		}

	}
	// scroll right?
	else if (localMousePos.d_x >= getAbsoluteWidth())
	{
		float adjust	= ScrollSpeed;
		float pixOffset = d_segmentOffset;
		float maxOffset = ceguimax(0.0f, getTotalSegmentsPixelExtent() - getAbsoluteWidth());

		// convert values as required so calculations can be done in a consistent way
		if (mmode == Relative)
		{
			maxOffset = absoluteToRelativeX(maxOffset);
			pixOffset = relativeToAbsoluteX(pixOffset);
			adjust = absoluteToRelativeX(adjust);
		}

		// if we have not scrolled to the limit
		if (d_segmentOffset < maxOffset)
		{
			// scroll, but never beyond the limit
			setSegmentOffset(ceguimin(maxOffset, d_segmentOffset + adjust));
		}

	}

	return true;
}


/*************************************************************************
	Add ListHeader specific events	
*************************************************************************/
void ListHeader::addListHeaderEvents(bool bCommon)
{
    if ( bCommon == true )
    {
        addEvent(EventSortColumnChanged);
        addEvent(EventSortDirectionChanged);
        addEvent(EventSegmentSized);
        addEvent(EventSegmentClicked);
        addEvent(EventSplitterDoubleClicked);
        addEvent(EventSegmentSequenceChanged);
        addEvent(EventSegmentRenderOffsetChanged);
    }
    else
    {
        addEvent(EventSegmentAdded);
        addEvent(EventSegmentRemoved);
        addEvent(EventSortSettingChanged);
        addEvent(EventDragMoveSettingChanged);
        addEvent(EventDragSizeSettingChanged);
    }
}

/*************************************************************************
	Add ListHeader specific properties
*************************************************************************/
void ListHeader::addHeaderProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_sizableProperty);
        addProperty(&d_movableProperty);
        addProperty(&d_sortSettingProperty);
        addProperty(&d_sortColumnIDProperty);
        addProperty(&d_sortDirectionProperty);
    }
}


} // End of  CEGUI namespace section

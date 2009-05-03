/************************************************************************
	filename: 	CEGUIScrollbar.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Scrollbar widget base class
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
#include "elements/CEGUIScrollbar.h"
#include "elements/CEGUIThumb.h"


// Start of CEGUI namespace section
namespace CEGUI
{
const String Scrollbar::EventNamespace("Scrollbar");

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
ScrollbarProperties::DocumentSize	Scrollbar::d_documentSizeProperty;
ScrollbarProperties::PageSize		Scrollbar::d_pageSizeProperty;
ScrollbarProperties::StepSize		Scrollbar::d_stepSizeProperty;
ScrollbarProperties::OverlapSize	Scrollbar::d_overlapSizeProperty;
ScrollbarProperties::ScrollPosition	Scrollbar::d_scrollPositionProperty;


/*************************************************************************
	Event name constants
*************************************************************************/
const String Scrollbar::EventScrollPositionChanged( (utf8*)"ScrollPosChanged" );
const String Scrollbar::EventThumbTrackStarted( (utf8*)"ThumbTrackStarted" );
const String Scrollbar::EventThumbTrackEnded( (utf8*)"ThumbTrackEnded" );
const String Scrollbar::EventScrollConfigChanged( (utf8*)"ScrollConfigChanged" );


/*************************************************************************
	Constructor for Scrollbar objects
*************************************************************************/
Scrollbar::Scrollbar(const String& type, const String& name) :
	Window(type, name),
	d_documentSize(1.0f),
	d_pageSize(0.0f),
	d_stepSize(1.0f),
	d_overlapSize(0.0f),
	d_position(0.0f)
{
	addScrollbarEvents();
	addScrollbarProperties();
}


/*************************************************************************
	Destructor for Scrollbar objects
*************************************************************************/
Scrollbar::~Scrollbar(void)
{
}


/*************************************************************************
	Initialises the Scrollbar object ready for use.
*************************************************************************/
void Scrollbar::initialise(void)
{
	// Set up thumb
	d_thumb = createThumb(getName() + "__auto_thumb__");
	addChildWindow(d_thumb);
	d_thumb->subscribeEvent(Thumb::EventThumbPositionChanged, Event::Subscriber(&CEGUI::Scrollbar::handleThumbMoved, this));
	d_thumb->subscribeEvent(Thumb::EventThumbTrackStarted, Event::Subscriber(&CEGUI::Scrollbar::handleThumbTrackStarted, this));
	d_thumb->subscribeEvent(Thumb::EventThumbTrackEnded, Event::Subscriber(&CEGUI::Scrollbar::handleThumbTrackEnded, this));

	// set up Increase button
	d_increase = createIncreaseButton(getName() + "__auto_incbtn__");
	addChildWindow(d_increase);
	d_increase->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&CEGUI::Scrollbar::handleIncreaseClicked, this));

	// set up Decrease button
	d_decrease = createDecreaseButton(getName() + "__auto_decbtn__");
	addChildWindow(d_decrease);
	d_decrease->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&CEGUI::Scrollbar::handleDecreaseClicked, this));

	// do initial layout
	performChildWindowLayout();
}


/*************************************************************************
	Set the size of the document or data.
*************************************************************************/
void Scrollbar::setDocumentSize(float document_size)
{
	if (d_documentSize != document_size)
	{
		d_documentSize = document_size;
		updateThumb();

		WindowEventArgs args(this);
		onScrollConfigChanged(args);
	}

}


/*************************************************************************
	Set the page size for this scroll bar.
*************************************************************************/
void Scrollbar::setPageSize(float page_size)
{
	if (d_pageSize != page_size)
	{
		d_pageSize = page_size;
		updateThumb();

		WindowEventArgs args(this);
		onScrollConfigChanged(args);
	}

}


/*************************************************************************
	Set the step size for this scroll bar.
*************************************************************************/
void Scrollbar::setStepSize(float step_size)
{
	if (d_stepSize != step_size)
	{
		d_stepSize = step_size;

		WindowEventArgs args(this);
		onScrollConfigChanged(args);
	}

}


/*************************************************************************
	Set the overlap size for this scroll bar.
*************************************************************************/
void Scrollbar::setOverlapSize(float overlap_size)
{
	if (d_overlapSize != overlap_size)
	{
		d_overlapSize = overlap_size;

		WindowEventArgs args(this);
		onScrollConfigChanged(args);
	}

}


/*************************************************************************
	Set the current position of scroll bar within the document.
*************************************************************************/
void Scrollbar::setScrollPosition(float position)
{
	float old_pos = d_position;

	// max position is (docSize - pageSize), but must be at least 0 (in case doc size is very small)
	float max_pos = ceguimax((d_documentSize - d_pageSize), 0.0f);

	// limit position to valid range:  0 <= position <= max_pos
	d_position = (position >= 0) ? ((position <= max_pos) ? position : max_pos) : 0.0f;

	updateThumb();

	// notification if required
	if (d_position != old_pos)
	{
		WindowEventArgs args(this);
		onScrollPositionChanged(args);
	}

}


/*************************************************************************
	Add scroll bar specific events
*************************************************************************/
void Scrollbar::addScrollbarEvents(void)
{
	addEvent(EventScrollPositionChanged);
	addEvent(EventThumbTrackStarted);
	addEvent(EventThumbTrackEnded);
	addEvent(EventScrollConfigChanged);
}


/*************************************************************************
	Handler triggered when the scroll position changes
*************************************************************************/
void Scrollbar::onScrollPositionChanged(WindowEventArgs& e)
{
	fireEvent(EventScrollPositionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the user begins to drag the scroll bar thumb. 	
*************************************************************************/
void Scrollbar::onThumbTrackStarted(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackStarted, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the scroll bar thumb is released
*************************************************************************/
void Scrollbar::onThumbTrackEnded(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackEnded, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the scroll bar data configuration changes
*************************************************************************/
void Scrollbar::onScrollConfigChanged(WindowEventArgs& e)
{
	performChildWindowLayout();
	fireEvent(EventScrollConfigChanged, e, EventNamespace);
}



/*************************************************************************
	Handler for when mouse button is clicked within the container
*************************************************************************/
void Scrollbar::onMouseButtonDown(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		float adj = getAdjustDirectionFromPoint(e.position);

		// adjust scroll bar position in whichever direction as required.
		if (adj != 0)
		{
			setScrollPosition(d_position + ((d_pageSize - d_overlapSize) * adj));
		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for scroll wheel changes
*************************************************************************/
void Scrollbar::onMouseWheel(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseWheel(e);

	// scroll by e.wheelChange * stepSize
	setScrollPosition(d_position + d_stepSize * -e.wheelChange);

	// ensure the message does not go to our parent.
	e.handled = true;
}


/*************************************************************************
	handler function for when thumb moves.
*************************************************************************/
bool Scrollbar::handleThumbMoved(const EventArgs& e)
{
	// adjust scroll bar position as required.
	setScrollPosition(getValueFromThumb());

	return true;
}


/*************************************************************************
	handler function for when the increase button is clicked.
*************************************************************************/
bool Scrollbar::handleIncreaseClicked(const EventArgs& e)
{
	if (((const MouseEventArgs&)e).button == LeftButton)
	{
		// adjust scroll bar position as required.
		setScrollPosition(d_position + d_stepSize);

		return true;
	}
	else
	{
		return false;
	}

}


/*************************************************************************
	handler function for when the decrease button is clicked.
*************************************************************************/
bool Scrollbar::handleDecreaseClicked(const EventArgs& e)
{
	if (((const MouseEventArgs&)e).button == LeftButton)
	{
		// adjust scroll bar position as required.
		setScrollPosition(d_position - d_stepSize);

		return true;
	}
	else
	{
		return false;
	}
}


/*************************************************************************
	handler function for when thumb tracking begins
*************************************************************************/
bool Scrollbar::handleThumbTrackStarted(const EventArgs& e)
{
	// simply trigger our own version of this event
	WindowEventArgs args(this);
	onThumbTrackStarted(args);

	return true;
}


/*************************************************************************
	handler function for when thumb tracking begins
*************************************************************************/
bool Scrollbar::handleThumbTrackEnded(const EventArgs& e)
{
	// simply trigger our own version of this event
	WindowEventArgs args(this);
	onThumbTrackEnded(args);

	return true;
}


/*************************************************************************
	Add scroll bar properties
*************************************************************************/
void Scrollbar::addScrollbarProperties(void)
{
	addProperty(&d_documentSizeProperty);
	addProperty(&d_pageSizeProperty);
	addProperty(&d_stepSizeProperty);
	addProperty(&d_overlapSizeProperty);
	addProperty(&d_scrollPositionProperty);
}



} // End of  CEGUI namespace section

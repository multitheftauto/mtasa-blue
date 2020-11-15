/***********************************************************************
	created:	25/4/2004
	author:		Paul D Turner
	
	purpose:	Implements common parts of the Thumb base class widget
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
#include "CEGUI/widgets/Thumb.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String Thumb::EventNamespace("Thumb");
const String Thumb::WidgetTypeName("CEGUI/Thumb");

/*************************************************************************
	Event name constants
*************************************************************************/
// generated internally by Window
const String Thumb::EventThumbPositionChanged( "ThumbPositionChanged" );
const String Thumb::EventThumbTrackStarted( "ThumbTrackStarted" );
const String Thumb::EventThumbTrackEnded( "ThumbTrackEnded" );


/*************************************************************************
	Constructor for Thumb objects
*************************************************************************/
Thumb::Thumb(const String& type, const String& name) :
	PushButton(type, name),
	d_hotTrack(true),
	d_vertFree(false),
	d_horzFree(false),
	d_vertMin(0.0f),
	d_vertMax(1.0f),
    d_horzMin(0.0f),
	d_horzMax(1.0f),
    d_beingDragged(false)
{
	addThumbProperties();
}


/*************************************************************************
	Destructor for Thumb objects	
*************************************************************************/
Thumb::~Thumb(void)
{
}


/*************************************************************************
	set the movement range of the thumb for the vertical axis.	
*************************************************************************/
void Thumb::setVertRange(float min, float max)
{
	// ensure min <= max, swap if not.
	if (min > max)
	{
		float tmp = min;
		max = min;
		min = tmp;
	}

	d_vertMax = max;
	d_vertMin = min;

	// validate current position.
	const float cp = CoordConverter::asRelative(getYPosition(), getParentPixelSize().d_height);

	if (cp < min)
	{
		setYPosition(cegui_reldim(min));
	}
	else if (cp > max)
	{
		setYPosition(cegui_reldim(max));
	}

}

/*************************************************************************
	set the movement range of the thumb for the vertical axis.	
*************************************************************************/
void Thumb::setVertRange(const std::pair<float, float> &range)
{
	setVertRange(range.first,range.second);
}
/*************************************************************************
	set the movement range of the thumb for the horizontal axis.
*************************************************************************/
void Thumb::setHorzRange(float min, float max)
{
    Sizef parentSize(getParentPixelSize());

	// ensure min <= max, swap if not.
	if (min > max)
	{
		float tmp = min;
		max = min;
		min = tmp;
	}

	d_horzMax = max;
	d_horzMin = min;

	// validate current position.
	const float cp = CoordConverter::asAbsolute(getXPosition(), parentSize.d_width);

	if (cp < min)
	{
		setXPosition(cegui_absdim(min));
	}
	else if (cp > max)
	{
		setXPosition(cegui_absdim(max));
	}

}
/*************************************************************************
	set the movement range of the thumb for the horizontal axis.
*************************************************************************/
void Thumb::setHorzRange(const std::pair<float, float> &range)
{
	setHorzRange(range.first,range.second);
}


/*************************************************************************
	event triggered internally when the position of the thumb	
*************************************************************************/
void Thumb::onThumbPositionChanged(WindowEventArgs& e)
{
	fireEvent(EventThumbPositionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the user begins to drag the thumb. 	
*************************************************************************/
void Thumb::onThumbTrackStarted(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackStarted, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the thumb is released
*************************************************************************/
void Thumb::onThumbTrackEnded(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackEnded, e, EventNamespace);
}


/*************************************************************************
	Handler for mouse movement events
*************************************************************************/
void Thumb::onMouseMove(MouseEventArgs& e)
{
	// default processing
	PushButton::onMouseMove(e);

	// only react if we are being dragged
	if (d_beingDragged)
	{
        Sizef parentSize(getParentPixelSize());

		Vector2f delta;
		float hmin, hmax, vmin, vmax;

        delta = CoordConverter::screenToWindow(*this, e.position);

        hmin = d_horzMin;
        hmax = d_horzMax;
        vmin = d_vertMin;
        vmax = d_vertMax;

		// calculate amount of movement      
		delta -= d_dragPoint;
        delta.d_x /= parentSize.d_width;
        delta.d_y /= parentSize.d_height;

		//
		// Calculate new (pixel) position for thumb
		//
		UVector2 newPos(getPosition());

		if (d_horzFree)
		{
			newPos.d_x.d_scale += delta.d_x;

			// limit value to within currently set range
			newPos.d_x.d_scale = (newPos.d_x.d_scale < hmin) ? hmin : (newPos.d_x.d_scale > hmax) ? hmax : newPos.d_x.d_scale;
		}

		if (d_vertFree)
		{
			newPos.d_y.d_scale += delta.d_y;

			// limit new position to within currently set range
			newPos.d_y.d_scale = (newPos.d_y.d_scale < vmin) ? vmin : (newPos.d_y.d_scale > vmax) ? vmax : newPos.d_y.d_scale;
		}

		// update thumb position if needed
		if (newPos != getPosition())
		{
			setPosition(newPos);

			// send notification as required
			if (d_hotTrack)
			{
				WindowEventArgs args(this);
				onThumbPositionChanged(args);
			}

		}

	}

	++e.handled;
}


/*************************************************************************
	Handler for mouse button down events
*************************************************************************/
void Thumb::onMouseButtonDown(MouseEventArgs& e)
{
	// default processing
	PushButton::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		// initialise the dragging state
		d_beingDragged = true;
		d_dragPoint = CoordConverter::screenToWindow(*this, e.position);

		// trigger tracking started event
		WindowEventArgs args(this);
		onThumbTrackStarted(args);

		++e.handled;
	}

}


/*************************************************************************
	Handler for event triggered when we lose capture of mouse input
*************************************************************************/
void Thumb::onCaptureLost(WindowEventArgs& e)
{
	// default handling
	PushButton::onCaptureLost(e);

	d_beingDragged = false;

	// trigger tracking ended event
	WindowEventArgs args(this);
	onThumbTrackEnded(args);

	// send notification whenever thumb is released
	onThumbPositionChanged(args);
}


/*************************************************************************
	Return a std::pair that describes the current range set for the
	vertical movement.	
*************************************************************************/
std::pair<float, float>	Thumb::getVertRange(void) const
{
	return std::make_pair(d_vertMin, d_vertMax);
}


/*************************************************************************
	Return a std::pair that describes the current range set for the
	horizontal movement.	
*************************************************************************/
std::pair<float, float>	Thumb::getHorzRange(void) const
{
	return std::make_pair(d_horzMin, d_horzMax);
}


/*************************************************************************
	Add thumb specifiec properties
*************************************************************************/
void Thumb::addThumbProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;
    
    CEGUI_DEFINE_PROPERTY(Thumb, bool,
        "HotTracked", "Property to get/set the state of the state of the 'hot-tracked' setting for the thumb."
        "  Value is either \"true\" or \"false\".",
        &Thumb::setHotTracked, &Thumb::isHotTracked, true
    );
    
    typedef std::pair<float,float> range;
    
    CEGUI_DEFINE_PROPERTY(Thumb, range,
        "VertRange", "Property to get/set the vertical movement range for the thumb.  Value is \"min:[float] max:[float]\".",
        &Thumb::setVertRange, &Thumb::getVertRange, range(0.0f, 1.0f)
    );

    CEGUI_DEFINE_PROPERTY(Thumb, range,
        "HorzRange", "Property to get/set the horizontal movement range for the thumb.  Value is \"min:[float] max:[float]\".",
        &Thumb::setHorzRange, &Thumb::getHorzRange, range(0.0f, 1.0f)
    );

    CEGUI_DEFINE_PROPERTY(Thumb, bool,
        "VertFree", "Property to get/set the state the setting to free the thumb vertically.  Value is either \"true\" or \"false\".",
        &Thumb::setVertFree, &Thumb::isVertFree, false
    );
    
    CEGUI_DEFINE_PROPERTY(Thumb, bool,
        "HorzFree", "Property to get/set the state the setting to free the thumb horizontally.  Value is either \"true\" or \"false\".",
        &Thumb::setHorzFree, &Thumb::isHorzFree, false
    );
}

//----------------------------------------------------------------------------//
void Thumb::banPropertiesForAutoWindow()
{
    PushButton::banPropertiesForAutoWindow();

    banPropertyFromXML("VertRange");
    banPropertyFromXML("HorzRange");
    banPropertyFromXML("VertFree");
    banPropertyFromXML("HorzFree");
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

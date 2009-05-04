/************************************************************************
	filename: 	CEGUISlider.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Slider widget base class
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
#include "elements/CEGUISlider.h"
#include "elements/CEGUIThumb.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String Slider::EventNamespace("Slider");

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
SliderProperties::CurrentValue	Slider::d_currentValueProperty;
SliderProperties::MaximumValue	Slider::d_maximumValueProperty;
SliderProperties::ClickStepSize	Slider::d_clickStepSizeProperty;


/*************************************************************************
	Event name constants
*************************************************************************/
const String Slider::EventValueChanged( (utf8*)"ValueChanged" );
const String Slider::EventThumbTrackStarted( (utf8*)"ThumbTrackStarted" );
const String Slider::EventThumbTrackEnded( (utf8*)"ThumbTrackEnded" );


/*************************************************************************
	Slider base class constructor
*************************************************************************/
Slider::Slider(const String& type, const String& name) :
	Window(type, name),
	d_value(0.0f),
	d_maxValue(1.0f),
	d_step(0.01f),
	d_thumb(NULL)
{
	addSliderEvents();
	addSliderProperties();
}


/*************************************************************************
	Slider base class destructor
*************************************************************************/
Slider::~Slider(void)
{
}


/*************************************************************************
	Initialises the Window based object ready for use.	
*************************************************************************/
void Slider::initialise(void)
{
	// create and attach thumb
	d_thumb = createThumb(getName() + "__auto_thumb__");
	addChildWindow(d_thumb);

	// bind handler to thumb events
	d_thumb->subscribeEvent(Thumb::EventThumbPositionChanged, Event::Subscriber(&CEGUI::Slider::handleThumbMoved, this));
	d_thumb->subscribeEvent(Thumb::EventThumbTrackStarted, Event::Subscriber(&CEGUI::Slider::handleThumbTrackStarted, this));
	d_thumb->subscribeEvent(Thumb::EventThumbTrackEnded, Event::Subscriber(&CEGUI::Slider::handleThumbTrackEnded, this));

	performChildWindowLayout();
}


/*************************************************************************
	set the maximum value for the slider.
	Note that the minimum value is fixed at 0.	
*************************************************************************/
void Slider::setMaxValue(float maxVal)
{
	d_maxValue = maxVal;

	float oldval = d_value;

	// limit current value to be within new max
	if (d_value > d_maxValue) {
		d_value = d_maxValue;
	}

	updateThumb();

	// send notification if slider value changed.
	if (d_value != oldval)
	{
		WindowEventArgs args(this);
		onValueChanged(args);
	}

}


/*************************************************************************
	set the current slider value.
*************************************************************************/
void Slider::setCurrentValue(float value)
{
	float oldval = d_value;

	// range for value: 0 <= value <= maxValue
	d_value = (value >= 0.0f) ? ((value <= d_maxValue) ? value : d_maxValue) : 0.0f;

	updateThumb();

	// send notification if slider value changed.
	if (d_value != oldval)
	{
		WindowEventArgs args(this);
		onValueChanged(args);
	}

}


/*************************************************************************
	Add slider specific events	
*************************************************************************/
void Slider::addSliderEvents(bool bCommon)
{
	if ( bCommon == false )	addEvent(EventValueChanged);
	if ( bCommon == true )	addEvent(EventThumbTrackStarted);
	if ( bCommon == true )	addEvent(EventThumbTrackEnded);
}


/*************************************************************************
	Handler triggered when the slider value changes
*************************************************************************/
void Slider::onValueChanged(WindowEventArgs& e)
{
	fireEvent(EventValueChanged, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the user begins to drag the slider thumb. 	
*************************************************************************/
void Slider::onThumbTrackStarted(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackStarted, e, EventNamespace);
}


/*************************************************************************
	Handler triggered when the slider thumb is released
*************************************************************************/
void Slider::onThumbTrackEnded(WindowEventArgs& e)
{
	fireEvent(EventThumbTrackEnded, e, EventNamespace);
}


/*************************************************************************
	Handler for when a mouse button is pressed
*************************************************************************/
void Slider::onMouseButtonDown(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		float adj = getAdjustDirectionFromPoint(e.position);

		// adjust slider position in whichever direction as required.
		if (adj != 0)
		{
			setCurrentValue(d_value + (adj * d_step));
		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for scroll wheel changes
*************************************************************************/
void Slider::onMouseWheel(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseWheel(e);

	// scroll by e.wheelChange * stepSize
	setCurrentValue(d_value + d_step * -e.wheelChange);

	// ensure the message does not go to our parent.
	e.handled = true;
}


/*************************************************************************
	handler function for when thumb moves.	
*************************************************************************/
bool Slider::handleThumbMoved(const EventArgs& e)
{
	setCurrentValue(getValueFromThumb());

	return true;
}


/*************************************************************************
	handler function for when thumb tracking begins
*************************************************************************/
bool Slider::handleThumbTrackStarted(const EventArgs& e)
{
	// simply trigger our own version of this event
	WindowEventArgs args(this);
	onThumbTrackStarted(args);

	return true;
}


/*************************************************************************
	handler function for when thumb tracking begins
*************************************************************************/
bool Slider::handleThumbTrackEnded(const EventArgs& e)
{
	// simply trigger our own version of this event
	WindowEventArgs args(this);
	onThumbTrackEnded(args);

	return true;
}


/*************************************************************************
	Add properties for the slider
*************************************************************************/
void Slider::addSliderProperties( bool bCommon )
{
	if ( bCommon == false )   addProperty(&d_currentValueProperty);
	if ( bCommon == false )   addProperty(&d_clickStepSizeProperty);
	if ( bCommon == false )   addProperty(&d_maximumValueProperty);
}


} // End of  CEGUI namespace section

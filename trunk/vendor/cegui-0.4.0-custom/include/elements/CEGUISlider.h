/************************************************************************
	filename: 	CEGUISlider.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Slider widget
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
#ifndef _CEGUISlider_h_
#define _CEGUISlider_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUISliderProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for Slider widgets.

	The slider widget has a default range of 0.0f - 1.0f.  This enables use of the slider value to scale
	any value needed by way of a simple multiplication.
*/
class CEGUIEXPORT Slider : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Event name constants
	*************************************************************************/
	static const String EventValueChanged;		//!< Event fired when the slider value changes.
	static const String EventThumbTrackStarted;	//!< Name of the event fired when the user begins dragging the thumb.
	static const String EventThumbTrackEnded;		//!< Name of the event fired when the user releases the thumb.


	/*************************************************************************
		Accessors
	*************************************************************************/
	/*!
	\brief
		return the current slider value.

	\return
		float value equal to the sliders current value.
	*/
	float	getCurrentValue(void) const			{return d_value;}


	/*!
	\brief
		return the maximum value set for this widget

	\return
		float value equal to the currently set maximum value for this slider.
	*/
	float	getMaxValue(void) const				{return d_maxValue;}


	/*!
	\brief
		return the current click step setting for the slider.

		The click step size is the amount the slider value will be adjusted when the widget
		is clicked wither side of the slider thumb.

	\return
		float value representing the current click step setting.
	*/
	float	getClickStep(void) const		{return d_step;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Initialises the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual	void	initialise(void);


	/*!
	\brief
		set the maximum value for the slider.  Note that the minimum value is fixed at 0.

	\param maxVal
		float value specifying the maximum value for this slider widget.

	\return
		Nothing.
	*/
	void	setMaxValue(float maxVal);


	/*!
	\brief
		set the current slider value.

	\param value
		float value specifying the new value for this slider widget.

	\return
		Nothing.
	*/
	void	setCurrentValue(float value);


	/*!
	\brief
		set the current click step setting for the slider.

		The click step size is the amount the slider value will be adjusted when the widget
		is clicked wither side of the slider thumb.

	\param step
		float value representing the click step setting to use.

	\return
		Nothing.
	*/
	void	setClickStep(float step)		{d_step = step;}


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Slider base class constructor
	*/
	Slider(const String& type, const String& name);


	/*!
	\brief
		Slider base class destructor
	*/
	virtual ~Slider(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add slider specific events
	*/
	void	addSliderEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addSliderEvents(false); }


	/*!
	\brief
		create a Thumb based widget to use as the thumb for this slider.

    \param name
        String containing the name to be assigned to the thumb upon creation.
	*/
	virtual Thumb*	createThumb(const String& name) const		= 0;


	/*!
	\brief
		update the size and location of the thumb to properly represent the current state of the slider
	*/
	virtual void	updateThumb(void)	= 0;


	/*!
	\brief
		return value that best represents current slider value given the current location of the thumb.

	\return
		float value that, given the thumb widget position, best represents the current value for the slider.
	*/
	virtual float	getValueFromThumb(void) const	= 0;


	/*!
	\brief
		Given window location \a pt, return a value indicating what change should be 
		made to the slider.

	\param pt
		Point object describing a pixel position in window space.

	\return
		- -1 to indicate slider should be moved to a lower setting.
		-  0 to indicate slider should not be moved.
		- +1 to indicate slider should be moved to a higher setting.
	*/
	virtual float	getAdjustDirectionFromPoint(const Point& pt) const	= 0;


	/*!
	\brief
		handler function for when thumb moves.
	*/
	bool	handleThumbMoved(const EventArgs& e);


	/*!
	\brief
		handler function for when thumb tracking begins
	*/
	bool	handleThumbTrackStarted(const EventArgs& e);


	/*!
	\brief
		handler function for when thumb tracking begins
	*/
	bool	handleThumbTrackEnded(const EventArgs& e);


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"Slider")	return true;
		return Window::testClassName_impl(class_name);
	}


	/*************************************************************************
		New event handlers for slider widget
	*************************************************************************/
	/*!
	\brief
		Handler triggered when the slider value changes
	*/
	virtual void	onValueChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler triggered when the user begins to drag the slider thumb. 
	*/
	virtual void	onThumbTrackStarted(WindowEventArgs& e);


	/*!
	\brief
		Handler triggered when the slider thumb is released
	*/
	virtual void	onThumbTrackEnded(WindowEventArgs& e);


	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual	void	onMouseWheel(MouseEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float	d_value;		//!< current slider value
	float	d_maxValue;		//!< slider maximum value (minimum is fixed at 0)
	float	d_step;			//!< amount to adjust slider by when clicked (and not dragged).

	// Pointers to the controls that make up the slider
	Thumb*	d_thumb;		//!< widget used to represent the 'thumb' of the slider.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static SliderProperties::CurrentValue	d_currentValueProperty;
	static SliderProperties::MaximumValue	d_maximumValueProperty;
	static SliderProperties::ClickStepSize	d_clickStepSizeProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addSliderProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addSliderProperties(false); }
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUISlider_h_

/************************************************************************
	filename: 	CEGUIScrollbar.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Scrollbar widget
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
#ifndef _CEGUIScrollbar_h_
#define _CEGUIScrollbar_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIScrollbarProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base scroll bar class.

	This base class for scroll bars does not have any idea of direction - a derived class would
	add whatever meaning is appropriate according to what that derived class
	represents to the user.
*/
class CEGUIEXPORT Scrollbar : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Event name constants
	*************************************************************************/
	static const String EventScrollPositionChanged;		//!< Name of the event fired when the scroll bar position value changes
	static const String EventThumbTrackStarted;			//!< Name of the event fired when the user begins dragging the thumb.
	static const String EventThumbTrackEnded;				//!< Name of the event fired when the user releases the thumb.
	static const String EventScrollConfigChanged;			//!< Name of the event fired when the scroll bar configuration data changes.


	/*************************************************************************
		Accessor functions
	*************************************************************************/
	/*!
	\brief
		Return the size of the document or data.
		
		The document size should be thought of as the total size of the data that
		is being scrolled through (the number of lines in a text file for example).

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\return
		float value specifying the currently set document size.
	*/
	float	getDocumentSize(void) const			{return d_documentSize;}


	/*!
	\brief
		Return the page size for this scroll bar.
		
		The page size is typically the amount of data that can be displayed at one
		time.  This value is also used when calculating the amount the position will
		change when you click either side of the scroll bar thumb - the amount the
		position changes will is (pageSize - overlapSize).

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\return
		float value specifying the currently set page size.
	*/
	float	getPageSize(void) const				{return d_pageSize;}


	/*!
	\brief
		Return the step size for this scroll bar.
		
		The step size is typically a single unit of data that can be displayed, this is the
		amount the position will change when you click either of the arrow buttons on the
		scroll bar.  (this could be 1 for a single line of text, for example).

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\return
		float value specifying the currently set step size.
	*/
	float	getStepSize(void) const				{return d_stepSize;}


	/*!
	\brief
		Return the overlap size for this scroll bar.
		
		The overlap size is the amount of data from the end of a 'page' that will
		remain visible when the position is moved by a page.  This is usually used
		so that the user keeps some context of where they were within the document's
		data when jumping a page at a time.

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\return
		float value specifying the currently set overlap size.
	*/
	float	getOverlapSize(void) const			{return d_overlapSize;}


	/*!
	\brief
		Return the current position of scroll bar within the document.

		The range of the scroll bar is from 0 to the size of the document minus the
		size of a page (0 <= position <= (documentSize - pageSize)).

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\return
		float value specifying the current position of the scroll bar within its
		document.
	*/
	float	getScrollPosition(void) const		{return d_position;}


	/*************************************************************************
		Manipulator Commands
	*************************************************************************/
	/*!
	\brief
		Initialises the Scrollbar object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


	/*!
	\brief
		Set the size of the document or data.
		
		The document size should be thought of as the total size of the data that
		is being scrolled through (the number of lines in a text file for example).

	\note
		The value set has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\param document_size
		float value specifying the document size.

	\return
		Nothing.
	*/
	void	setDocumentSize(float document_size);


	/*!
	\brief
		Set the page size for this scroll bar.
		
		The page size is typically the amount of data that can be displayed at one
		time.  This value is also used when calculating the amount the position will
		change when you click either side of the scroll bar thumb - the amount the
		position changes will is (pageSize - overlapSize).

	\note
		The value set has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\param page_size
		float value specifying the page size.

	\return
		Nothing.
	*/
	void	setPageSize(float page_size);


	/*!
	\brief
		Set the step size for this scroll bar.
		
		The step size is typically a single unit of data that can be displayed, this is the
		amount the position will change when you click either of the arrow buttons on the
		scroll bar.  (this could be 1 for a single line of text, for example).

	\note
		The value set has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\param step_size
		float value specifying the step size.

	\return
		Nothing.
	*/
	void	setStepSize(float step_size);


	/*!
	\brief
		Set the overlap size for this scroll bar.
		
		The overlap size is the amount of data from the end of a 'page' that will
		remain visible when the position is moved by a page.  This is usually used
		so that the user keeps some context of where they were within the document's
		data when jumping a page at a time.

	\note
		The value set has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\param overlap_size
		float value specifying the overlap size.

	\return
		Nothing.
	*/
	void	setOverlapSize(float overlap_size);


	/*!
	\brief
		Set the current position of scroll bar within the document.

		The range of the scroll bar is from 0 to the size of the document minus the
		size of a page (0 <= position <= (documentSize - pageSize)), any attempt to
		set the position outside this range will be adjusted so that it falls within
		the legal range.

	\note
		The returned value has no meaning within the Gui system, it is left up to the
		application to assign appropriate values for the application specific use of
		the scroll bar.

	\param position
		float value specifying the position of the scroll bar within its
		document.

	\return
		Nothing.
	*/
	void	setScrollPosition(float position);


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Scrollbar objects
	*/
	Scrollbar(const String& type, const String& name);


	/*!
	\brief
		Destructor for Scrollbar objects
	*/
	virtual ~Scrollbar(void);


protected:
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		Add scroll bar specific events
	*/
	void	addScrollbarEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addScrollbarEvents(false); }


	/*!
	\brief
		create a PushButton based widget to use as the increase button for this scroll bar.

    \param name
        String holding the name that must be passed when creating the component widget.
	*/
	virtual PushButton*	createIncreaseButton(const String& name) const		= 0;


	/*!
	\brief
		create a PushButton based widget to use as the decrease button for this scroll bar.

    \param name
        String holding the name that must be passed when creating the component widget.
	*/
	virtual PushButton*	createDecreaseButton(const String& name) const		= 0;


	/*!
	\brief
		create a Thumb based widget to use as the thumb for this scroll bar.

    \param name
        String holding the name that must be passed when creating the component widget.
	*/
	virtual Thumb*	createThumb(const String& name) const		= 0;


	/*!
	\brief
		update the size and location of the thumb to properly represent the current state of the scroll bar
	*/
	virtual void	updateThumb(void)	= 0;


	/*!
	\brief
		return value that best represents current scroll bar position given the current location of the thumb.

	\return
		float value that, given the thumb widget position, best represents the current position for the scroll bar.
	*/
	virtual float	getValueFromThumb(void) const	= 0;


	/*!
	\brief
		Given window location \a pt, return a value indicating what change should be 
		made to the scroll bar.

	\param pt
		Point object describing a pixel position in window space.

	\return
		- -1 to indicate scroll bar position should be moved to a lower value.
		-  0 to indicate scroll bar position should not be changed.
		- +1 to indicate scroll bar position should be moved to a higher value.
	*/
	virtual float	getAdjustDirectionFromPoint(const Point& pt) const	= 0;


	/*!
	\brief
		handler function for when thumb moves.
	*/
	bool	handleThumbMoved(const EventArgs& e);


	/*!
	\brief
		handler function for when the increase button is clicked.
	*/
	bool	handleIncreaseClicked(const EventArgs& e);


	/*!
	\brief
		handler function for when the decrease button is clicked.
	*/
	bool	handleDecreaseClicked(const EventArgs& e);


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
		if (class_name==(const utf8*)"Scrollbar")	return true;
		return Window::testClassName_impl(class_name);
	}


	/*************************************************************************
		New event handlers for slider widget
	*************************************************************************/
	/*!
	\brief
		Handler triggered when the scroll position changes
	*/
	virtual void	onScrollPositionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler triggered when the user begins to drag the scroll bar thumb. 
	*/
	virtual void	onThumbTrackStarted(WindowEventArgs& e);


	/*!
	\brief
		Handler triggered when the scroll bar thumb is released
	*/
	virtual void	onThumbTrackEnded(WindowEventArgs& e);


	/*!
	\brief
		Handler triggered when the scroll bar data configuration changes
	*/
	virtual void	onScrollConfigChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual	void	onMouseWheel(MouseEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float	d_documentSize;		//!< The size of the document / data being scrolled thorugh.
	float	d_pageSize;			//!< The size of a single 'page' of data.
	float	d_stepSize;			//!< Step size used for increase / decrease button clicks.
	float	d_overlapSize;		//!< Amount of overlap when jumping by a page.
	float	d_position;			//!< Current scroll position.

	// Pointers to the controls that make up the scroll bar
	Thumb*		d_thumb;		//!< widget used to represent the 'thumb' of the scroll bar.
	PushButton*	d_increase;		//!< Widget used for the increase button of the scroll bar.
	PushButton*	d_decrease;		//!< Widget used for the decrease button of the scroll bar.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static ScrollbarProperties::DocumentSize	d_documentSizeProperty;
	static ScrollbarProperties::PageSize		d_pageSizeProperty;
	static ScrollbarProperties::StepSize		d_stepSizeProperty;
	static ScrollbarProperties::OverlapSize		d_overlapSizeProperty;
	static ScrollbarProperties::ScrollPosition	d_scrollPositionProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addScrollbarProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addScrollbarProperties(false); }
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIScrollbar_h_

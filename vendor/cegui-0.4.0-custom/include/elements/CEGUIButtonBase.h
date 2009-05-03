/************************************************************************
	filename: 	CEGUIButtonBase.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for ButtonBase widget
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
#ifndef _CEGUIButtonBase_h_
#define _CEGUIButtonBase_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIButtonBaseProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for all the 'button' type widgets (push button, radio button, check-box, etc)
*/
class CEGUIEXPORT ButtonBase : public Window
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	// default colours for text label rendering
	static const colour		DefaultNormalLabelColour;		//!< Default colour used when rendering label text in normal state.
	static const colour		DefaultHoverLabelColour;		//!< Default colour used when rendering label text in hover / highlight state.
	static const colour		DefaultPushedLabelColour;		//!< Default colour used when rendering label text in pushed state.
	static const colour		DefaultDisabledLabelColour;		//!< Default colour used when rendering label text in disabled state.


	/*************************************************************************
		Accessor type functions
	*************************************************************************/
	/*!
	\brief
		return true if user is hovering over this widget (or it's pushed and user is not over it for highlight)

	\return
		true if the user is hovering or if the button is pushed and the mouse is not over the button.  Otherwise return false.
	*/
	bool	isHovering(void) const			{return d_hovering;}


	/*!
	\brief
		Return true if the button widget is in the pushed state.

	\return
		true if the button-type widget is pushed, false if the widget is not pushed.
	*/
	bool	isPushed(void) const			{return d_pushed;}


	/*!
	\brief
		return text label colour used for normal rendering

	\return
		colour value that is used for the label text when rendering in the normal state.
	*/
	colour	getNormalTextColour(void) const			{return d_normalColour;}


	/*!
	\brief
		return text label colour used for hover / highlight rendering

	\return
		colour value that is used for the label text when rendering in the hover / highlighted states.
	*/
	colour	getHoverTextColour(void) const			{return d_hoverColour;}


	/*!
	\brief
		return text label colour used for pushed rendering

	\return
		colour value that is used for the label text when rendering in the pushed state.
	*/
	colour	getPushedTextColour(void) const			{return d_pushedColour;}


	/*!
	\brief
		return text label colour used for disabled rendering

	\return
		colour value that is used for the label text when rendering in the disabled state.
	*/
	colour	getDisabledTextColour(void) const		{return d_disabledColour;}

	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the colour to use for the label text when rendering in the normal state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setNormalTextColour(const colour& colour);


	/*!
	\brief
		Set the colour to use for the label text when rendering in the hover / highlighted states.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setHoverTextColour(const colour& colour);


	/*!
	\brief
		Set the colour to use for the label text when rendering in the pushed state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setPushedTextColour(const colour& colour);


	/*!
	\brief
		Set the colour to use for the label text when rendering in the disabled state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setDisabledTextColour(const colour& colour);

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for ButtonBase objects
	*/
	ButtonBase(const String& type, const String& name);


	/*!
	\brief
		Destructor for ButtonBase objects
	*/
	virtual ~ButtonBase(void);


protected:
	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	virtual void	onMouseMove(MouseEventArgs& e);
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual void	onMouseButtonUp(MouseEventArgs& e);
	virtual void	onCaptureLost(WindowEventArgs& e);
	virtual void	onMouseLeaves(MouseEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Update the internal state of the widget with the mouse at the given position.

	\param mouse_pos
		Point object describing, in screen pixel co-ordinates, the location of the mouse cursor.

	\return
		Nothing
	*/
	void	updateInternalState(const Point& mouse_pos);


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
		if (class_name==(const utf8*)"ButtonBase")	return true;
		return Window::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Rendering Functions
	*************************************************************************/
	/*!
	\brief
		Perform the rendering for this widget.

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	virtual	void	drawSelf(float z);


	/*!
	\brief
		Render the button-type widget in it's 'normal' state

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	virtual void	drawNormal(float z)		= 0;


	/*!
	\brief
		Render the button-type widget in it's 'hover' (highlighted) state

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	virtual void	drawHover(float z)			{drawNormal(z);}


	/*!
	\brief
		Render the button-type widget in it's 'pushed' state

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	virtual void	drawPushed(float z)		{drawNormal(z);}


	/*!
	\brief
		Render the button-type widget in it's 'disabled' state

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	virtual void	drawDisabled(float z)		{drawNormal(z);}


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool	d_pushed;			//!< true when widget is pushed
	bool	d_hovering;			//!< true when the button is in 'hover' state and requires the hover rendering.

	// common rendering setting data
	colour	d_normalColour;					//!< Colour used for label text when rendering in normal state
	colour	d_hoverColour;					//!< Colour used for label text when rendering in highlighted state
	colour	d_pushedColour;					//!< Colour used for label text when rendering in pushed state
	colour	d_disabledColour;				//!< Colour used for label text when rendering in disabled state

private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static ButtonBaseProperties::NormalTextColour	d_normalTextColourProperty;
	static ButtonBaseProperties::HoverTextColour	d_hoverTextColourProperty;
	static ButtonBaseProperties::PushedTextColour	d_pushedTextColourProperty;
	static ButtonBaseProperties::DisabledTextColour	d_disabledTextColourProperty;

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addButtonBaseProperties(void);
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIButtonBase_h_

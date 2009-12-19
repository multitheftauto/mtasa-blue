/************************************************************************
	filename: 	CEGUIMenuItem.h
	created:	2/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for MenuItem widget
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
#ifndef _CEGUIMenuItem_h_
#define _CEGUIMenuItem_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUITextItem.h"
#include "elements/CEGUIMenuItemProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for menu items.
*/
class CEGUIEXPORT MenuItem : public TextItem
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Constants
	*************************************************************************/
	// default colours for rendering
	
	static const colour		DefaultHoverColour;			//!< Default colour used when rendering in hover / highlight state.
	static const colour		DefaultPushedColour;		//!< Default colour used when rendering in pushed state.
	static const colour		DefaultOpenedColour;		//!< Default colour used when rendering in opended state.
	static const colour		DefaultNormalTextColour;	//!< Default colour used when rendering the text in normal state.
	static const colour		DefaultDisabledTextColour;	//!< Default colour used when rendering the text in disabled state.


	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventClicked;					//!< The menuitem was clicked.


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
		return text label colour used for hover / highlight rendering

	\return
		colour value that is used for the label text when rendering in the hover / highlighted states.
	*/
	colour	getHoverColour(void) const			{return d_hoverColour;}


	/*!
	\brief
		return text label colour used for pushed rendering

	\return
		colour value that is used for the label text when rendering in the pushed state.
	*/
	colour	getPushedColour(void) const			{return d_pushedColour;}


	/*!
	\brief
		return text label colour used for opened rendering

	\return
		colour value that is used for the label text when rendering in the opened state.
	*/
	colour	getOpenedColour(void) const			{return d_openedColour;}


	/*!
	\brief
		return text label colour used for normal rendering

	\return
		colour value that is used for the label text when rendering in the normal state.
	*/
	colour	getNormalTextColour(void) const			{return d_normalTextColour;}


	/*!
	\brief
		return text label colour used for disabled rendering

	\return
		colour value that is used for the label text when rendering in the disabled state.
	*/
	colour	getDisabledTextColour(void) const		{return d_disabledTextColour;}


	/*!
	\brief
		Get the PopupMenu that is currently attached to this MenuItem.

	\return
		A pointer to the currently attached PopupMenu.  Null is there is no PopupMenu attached.
	*/
	PopupMenu*	getPopupMenu(void) const			{return d_popup;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the colour to use when rendering in the hover / highlighted states.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setHoverColour(const colour& colour);


	/*!
	\brief
		Set the colour to use when rendering in the pushed state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setPushedColour(const colour& colour);


	/*!
	\brief
		Set the colour to use when rendering in the opened state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setOpenedColour(const colour& colour);


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
		Set the colour to use for the label text when rendering in the disabled state.

	\param colour
		colour value specifying the colour to be used.

	\return
		Nothing.
	*/
	void	setDisabledTextColour(const colour& colour);


	/*!
	\brief
		Set the popup menu for this item.

	\param popup
		popupmenu window to attach to this item

	\return
		Nothing.
	*/
	void	setPopupMenu(PopupMenu* popup);


	/*!
	\brief
		Opens the PopupMenu.
	*/
	void	openPopupMenu(void);


	/*!
	\brief
		Closes the PopupMenu.

	\param notify
		true if the parent menubar is to be notified of the close.

	\return
		Nothing.
	*/
	void	closePopupMenu(bool notify=true);


	/*!
	\brief
		Toggles the PopupMenu.

	\return
		true if the popup was opened. false if it was closed.
	*/
	bool	togglePopupMenu(void);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for MenuItem objects
	*/
	MenuItem(const String& type, const String& name);


	/*!
	\brief
		Destructor for MenuItem objects
	*/
	virtual ~MenuItem(void);


protected:
	/*************************************************************************
		New Event Handlers
	*************************************************************************/
	/*!
	\brief
		handler invoked internally when the MenuItem is clicked.
	*/
	virtual void	onClicked(WindowEventArgs& e);


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
		Add menuitem specific events
	*/
	void	addMenuItemEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addMenuItemEvents(false); }


	/*!
	\brief
		Recursive function that closes all popups down the hierarcy starting with this one.

	\return
		Nothing.
	*/
	void	closeAllMenuItemPopups();


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
		if (class_name==(const utf8*)"MenuItem")	return true;
		return TextItem::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Rendering Functions
	*************************************************************************/

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool	d_pushed;			//!< true when widget is pushed
	bool	d_hovering;			//!< true when the button is in 'hover' state and requires the hover rendering.
	bool	d_opened;			//!< true when the menu item's popup menu is opened.

	// common rendering setting data
	colour	d_hoverColour;					//!< Colour used when rendering in highlighted state
	colour	d_pushedColour;					//!< Colour used when rendering in pushed state
	colour	d_openedColour;					//!< Colour used when rendering in opened state
	colour	d_normalTextColour;				//!< Colour used for the text when rendering in normal state
	colour	d_disabledTextColour;			//!< Colour used for the text when rendering in disabled state

	PopupMenu* d_popup;						//!< PopupMenu that this item displays when activated.

	bool	d_popupWasClosed;				//!< Used internally to determine if a popup was just closed on a Clicked event

private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static MenuItemProperties::HoverColour			d_hoverColourProperty;
	static MenuItemProperties::PushedColour			d_pushedColourProperty;
	static MenuItemProperties::OpenedColour			d_openedColourProperty;
	static MenuItemProperties::NormalTextColour		d_normalTextColourProperty;
	static MenuItemProperties::DisabledTextColour	d_disabledTextColourProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void addMenuItemProperties( bool bCommon = true );
	void addUncommonProperties( void )							{ __super::addUncommonProperties(); addMenuItemProperties(false); }


	/*!
	\brief
		Add given window to child list at an appropriate position
	*/
	virtual void	addChild_impl(Window* wnd);
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif	// end of guard _CEGUIMenuItem_h_

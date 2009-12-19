/************************************************************************
	filename: 	CEGUIMenuBase.h
	created:	5/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for MenuBase widget
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
#ifndef _CEGUIMenuBase_h_
#define _CEGUIMenuBase_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIMenuBaseProperties.h"
#include "elements/CEGUIItemListBase.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Abstract base class for menus.
*/
class CEGUIEXPORT MenuBase : public ItemListBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Constants
	*************************************************************************/
	static const colour DefaultBackgroundColour;			//!< Default background colour


	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventPopupOpened;					//!< A MenuItem attached to this menu opened a PopupMenu
	static const String EventPopupClosed;					//!< A MenuItem attached to this menu opened a PopupMenu


	/*************************************************************************
		Accessor type functions
	*************************************************************************/
	/*!
	\brief
		Get the item spacing for this menu.

	\return
		A float value with the current item spacing for this menu
	*/
	float	getItemSpacing(void) const					{return d_itemSpacing;}


	/*!
	\brief
		Get the horizontal padding for this menu's items.

	\return
		A float value with the current horizontal padding for this menu's items.
	*/
	float	getHorzPadding(void) const					{return d_horzPadding;}


	/*!
	\brief
		Get the vertical padding for this menu's items.

	\return
		A float value with the current vertical padding for this menu's items.
	*/
	float	getVertPadding(void) const					{return d_vertPadding;}


	/*!
	\brief
		Get the border width for this menu.

	\return
		A float value with the current border width for this menu.
	*/
	float	getBorderWidth(void) const					{return d_borderWidth;}	


	/*!
	\brief
		Get the ColourRect with the colours used for rendering the background of this menu.

	\return
		A ColourRect with the colours used for rendering the background of this menu.
	*/
	const ColourRect&	getBackgroundColours(void) const	{return d_backgroundColours;}


	/*!
	\brief
		Return whether this menu allows multiple popup menus to open at the same time.

	\return
		true if this menu allows multiple popup menus to be opened simultaneously. false if not
	*/
	bool	isMultiplePopupsAllowed(void) const			{return d_allowMultiplePopups;}


	/*!
	\brief
		Get currently opened MenuItem in this menu. Returns NULL if menu item is open.

	\return
		Pointer to the MenuItem currently open.
	*/
	MenuItem*	getPopupMenuItem(void) const				{return d_popup;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the item spacing for this menu.
	*/
	void	setItemSpacing(float spacing)				{d_itemSpacing=spacing;handleUpdatedItemData();}


	/*!
	\brief
		Set the horizontal padding for this menu's items.
	*/
	void	setHorzPadding(float padding)				{d_horzPadding=padding;handleUpdatedItemData();}


	/*!
	\brief
		Set the vertical padding for this menu's items.
	*/
	void	setVertPadding(float padding)				{d_vertPadding=padding;handleUpdatedItemData();}


	/*!
	\brief
		Set the border width for this menu.
	*/
	void	setBorderWidth(float border)				{d_borderWidth=border;handleUpdatedItemData();}


	/*!
	\brief
		Set the background colours to used when rendereing this menu.
	*/
	void	setBackgroundColours(const ColourRect& cr)	{d_backgroundColours=cr;}


	/*!
	\brief
		Change the currently open MenuItem in this menu.

	\param item
		Pointer to a MenuItem to open or NULL to close any opened.
	*/
	void	changePopupMenuItem(MenuItem* item);


	/*!
	\brief
		Set whether this menu allows multiple popup menus to opened simultaneously.
	*/
	void	setAllowMultiplePopups(bool setting)		{d_allowMultiplePopups=setting;}


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for MenuBase objects
	*/
	MenuBase(const String& type, const String& name);


	/*!
	\brief
		Destructor for MenuBase objects
	*/
	virtual ~MenuBase(void);


protected:
	/*************************************************************************
		New Event Handlers
	*************************************************************************/
	/*!
	\brief
		handler invoked internally when the a MenuItem attached to this menu opens its popup.
	*/
	virtual void	onPopupOpened(WindowEventArgs& e);


	/*!
	\brief
		handler invoked internally when the a MenuItem attached to this menu closes its popup.
	*/
	virtual void	onPopupClosed(WindowEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add MenuBase specific events
	*/
	void	addMenuBaseEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addMenuBaseEvents(false); }


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
		if (class_name==(const utf8*)"MenuBase")	return true;
		return ItemListBase::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float d_itemSpacing;		//!< The spacing in pixels between items.
	float d_horzPadding;		//!< The width in pixels that is added to the left and right of each item.
	float d_vertPadding;		//!< The height in pixels that is added to the top and bottom of each item.
	float d_borderWidth;		//!< The width in pixels of the border around the entire content of this menu.
	
	ColourRect d_backgroundColours;		//!< The colours used when rendering the background of this menu.
	
	MenuItem* d_popup;		//!< The currently open MenuItem. NULL if no item is open. If multiple popups are allowed, this means nothing.
	bool d_allowMultiplePopups;		//!< true if multiple popup menus are allowed smultaneously.  false if not.


private:
	/*************************************************************************
	Static Properties for this class
	*************************************************************************/
	static MenuBaseProperties::ItemSpacing			d_itemSpacingProperty;
	static MenuBaseProperties::HorzPadding			d_horzPaddingProperty;
	static MenuBaseProperties::VertPadding			d_vertPaddingProperty;
	static MenuBaseProperties::BorderWidth			d_borderWidthProperty;
	static MenuBaseProperties::BackgroundColours	d_backgroundColoursProperty;
	static MenuBaseProperties::AllowMultiplePopups	d_allowMultiplePopupsProperty;

	/*************************************************************************
	Private methods
	*************************************************************************/
	void	addMenuBaseProperties( bool bCommon = true );
protected:
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addMenuBaseProperties(false); }
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIMenuBase_h_

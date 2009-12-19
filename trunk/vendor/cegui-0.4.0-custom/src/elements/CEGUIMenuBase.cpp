/************************************************************************
	filename: 	CEGUIMenuBase.cpp
	created:	5/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of MenuBase widget base class
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
#include "elements/CEGUIMenuBase.h"
#include "elements/CEGUIPopupMenu.h"
#include "elements/CEGUIMenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
Definition of Properties for this class
*************************************************************************/
MenuBaseProperties::ItemSpacing			MenuBase::d_itemSpacingProperty;
MenuBaseProperties::HorzPadding			MenuBase::d_horzPaddingProperty;
MenuBaseProperties::VertPadding			MenuBase::d_vertPaddingProperty;
MenuBaseProperties::BorderWidth			MenuBase::d_borderWidthProperty;
MenuBaseProperties::BackgroundColours	MenuBase::d_backgroundColoursProperty;
MenuBaseProperties::AllowMultiplePopups	MenuBase::d_allowMultiplePopupsProperty;


/*************************************************************************
	Constants
*************************************************************************/
const colour MenuBase::DefaultBackgroundColour = 0xFFFFFFFF;

// event strings
const String MenuBase::EventNamespace("MenuBase");
const String MenuBase::EventPopupOpened( (utf8*)"PopupOpened" );
const String MenuBase::EventPopupClosed( (utf8*)"PopupClosed" );

/*************************************************************************
	Constructor for MenuBase base class.
*************************************************************************/
MenuBase::MenuBase(const String& type, const String& name)
	: ItemListBase(type, name),
	d_itemSpacing(0.0f),
	d_horzPadding(0.0f),
	d_vertPadding(0.0f),
	d_borderWidth(0.0f),
	d_backgroundColours(DefaultBackgroundColour),
	d_popup(NULL),
	d_allowMultiplePopups(false)
{
	// add new events specific to MenuBase.
	addMenuBaseEvents();
	
	// add properties for MenuBase class
	addMenuBaseProperties();
}


/*************************************************************************
	Destructor for MenuBase base class.
*************************************************************************/
MenuBase::~MenuBase(void)
{
}


/*************************************************************************
	Change the currently open MenuItem PopupMenu
*************************************************************************/
void MenuBase::changePopupMenuItem(MenuItem* item)
{
	if (!d_allowMultiplePopups&&d_popup==item)
		return;

	if (!d_allowMultiplePopups&&d_popup!=NULL)
	{
		d_popup->closePopupMenu(false);
		WindowEventArgs we(d_popup->getPopupMenu());
		d_popup = NULL;
		onPopupClosed(we);
	}

	if (item!=NULL)
	{
		d_popup = item;
		d_popup->getPopupMenu()->openPopupMenu();
		WindowEventArgs we(d_popup->getPopupMenu());
		onPopupOpened(we);
	}

}


/*************************************************************************
	Add MenuBase specific events	
*************************************************************************/
void MenuBase::addMenuBaseEvents(bool bCommon)
{
    if ( bCommon == false )
    {
        addEvent(EventPopupOpened);
        addEvent(EventPopupClosed);
    }
}


/*************************************************************************
	handler invoked internally when the a MenuItem attached to this
	MenuBase opens its popup.
*************************************************************************/
void MenuBase::onPopupOpened(WindowEventArgs& e)
{
	fireEvent(EventPopupOpened, e, EventNamespace);
}


/*************************************************************************
	handler invoked internally when the a MenuItem attached to this
	MenuBase closes its popup.
*************************************************************************/
void MenuBase::onPopupClosed(WindowEventArgs& e)
{
	fireEvent(EventPopupClosed, e, EventNamespace);
}


void MenuBase::addMenuBaseProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_itemSpacingProperty);
        addProperty(&d_horzPaddingProperty);
        addProperty(&d_vertPaddingProperty);
        addProperty(&d_borderWidthProperty);
        addProperty(&d_backgroundColoursProperty);
        addProperty(&d_allowMultiplePopupsProperty);
    }
}

} // End of  CEGUI namespace section

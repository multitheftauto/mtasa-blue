/************************************************************************
	filename: 	CEGUICheckbox.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Checkbox base class
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
#include "elements/CEGUICheckbox.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String Checkbox::EventNamespace("Checkbox");

/*************************************************************************
	Definitions for Properties
*************************************************************************/
CheckboxProperties::Selected	Checkbox::d_selectedProperty;


/*************************************************************************
	Event name constants
*************************************************************************/
// generated internally by Window
const String Checkbox::EventCheckStateChanged( (utf8*)"CheckStateChanged" );


/*************************************************************************
	Constructor
*************************************************************************/
Checkbox::Checkbox(const String& type, const String& name) :
	ButtonBase(type, name),
	d_selected(false)
{
	// add events for this widget
	addCheckboxEvents();

	addCheckboxProperties();
}


/*************************************************************************
	Destructor
*************************************************************************/
Checkbox::~Checkbox(void)
{
}


/*************************************************************************
	set whether the check-box is selected or not	
*************************************************************************/
void Checkbox::setSelected(bool select)
{
	if (select != d_selected)
	{
		d_selected = select;
		requestRedraw();

        WindowEventArgs args(this);
		onSelectStateChange(args);
	}

}


/*************************************************************************
	event triggered internally when state of check-box changes	
*************************************************************************/
void Checkbox::onSelectStateChange(WindowEventArgs& e)
{
	fireEvent(EventCheckStateChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for mouse button up events
*************************************************************************/
void Checkbox::onMouseButtonUp(MouseEventArgs& e)
{
	if ((e.button == LeftButton) && isPushed())
	{
		Window* sheet = System::getSingleton().getGUISheet();

		if (sheet != NULL)
		{
			// if mouse was released over this widget
			if (this == sheet->getChildAtPosition(e.position))
			{
				// toggle selected state
				setSelected(d_selected ^ true);
			}

		}

		e.handled = true;
	}

	// default handling
	ButtonBase::onMouseButtonUp(e);
}


/*************************************************************************
	Add check-box specific events
*************************************************************************/
void Checkbox::addCheckboxEvents(bool bCommon)
{
	if ( bCommon == true )	addEvent(EventCheckStateChanged);
}

/*************************************************************************
	Add properties
*************************************************************************/
void Checkbox::addCheckboxProperties( bool bCommon )
{
	if ( bCommon == false )   addProperty(&d_selectedProperty);
}



} // End of  CEGUI namespace section

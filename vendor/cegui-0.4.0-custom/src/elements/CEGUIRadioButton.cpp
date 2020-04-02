/************************************************************************
	filename: 	CEGUIRadioButton.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of RadioButton widget base class
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
#include "elements/CEGUIRadioButton.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String RadioButton::EventNamespace("RadioButton");

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
RadioButtonProperties::Selected	RadioButton::d_selectedProperty;
RadioButtonProperties::GroupID	RadioButton::d_groupIDProperty;


/*************************************************************************
	Event name constants
*************************************************************************/
// generated internally by Window
const String RadioButton::EventSelectStateChanged( (utf8*)"SelectStateChanged" );


/*************************************************************************
	Constructor
*************************************************************************/
RadioButton::RadioButton(const String& type, const String& name) :
	ButtonBase(type, name),
	d_selected(false),
	d_groupID(0)
{
	// add radio button specific events.
	addRadioButtonEvents();

	addRadioButtonProperties();
}


/*************************************************************************
	Destructor
*************************************************************************/
RadioButton::~RadioButton(void)
{
}


/*************************************************************************
	set whether the radio button is selected or not	
*************************************************************************/
void RadioButton::setSelected(bool select)
{
	if (select != d_selected)
	{
		d_selected = select;
		requestRedraw();

		// if new state is 'selected', we must de-select any selected radio buttons within our group.
		if (d_selected)
		{
			deselectOtherButtonsInGroup();
		}

		WindowEventArgs args(this);
		onSelectStateChanged(args);
	}

}


/*************************************************************************
	set the groupID for this radio button	
*************************************************************************/
void RadioButton::setGroupID(ulong group)
{
	d_groupID = group;

	if (d_selected)
	{
		deselectOtherButtonsInGroup();
	}

}


/*************************************************************************
	Add radio button specific events	
*************************************************************************/
void RadioButton::addRadioButtonEvents(bool bCommon)
{
	if ( bCommon == true )	addEvent(EventSelectStateChanged);
}


/*************************************************************************
	Deselect any selected radio buttons attached to the same parent
	within the same group (but not do not deselect 'this').
*************************************************************************/
void RadioButton::deselectOtherButtonsInGroup(void) const
{
	// nothing to do unless we are attached to another window.
	if (d_parent != NULL)
	{
		int child_count = d_parent->getChildCount();

		// scan all children
		for (int child = 0; child < child_count; ++child)
		{
			// is this child same type as we are?
			if (d_parent->getChildAtIdx(child)->getType() == getType())
			{
				RadioButton* rb = (RadioButton*)d_parent->getChildAtIdx(child);

				// is child same group, selected, but not 'this'?
				if (rb->isSelected() && (rb != this) && (rb->getGroupID() == d_groupID))
				{
					// deselect the radio button.
					rb->setSelected(false);
				}

			}

		}

	}

}


/*************************************************************************
	event triggered internally when the select state of the button changes.
*************************************************************************/
void RadioButton::onSelectStateChanged(WindowEventArgs& e)
{
	fireEvent(EventSelectStateChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when mouse button gets released
*************************************************************************/
void RadioButton::onMouseButtonUp(MouseEventArgs& e)
{
	if ((e.button == LeftButton) && isPushed())
	{
		Window* sheet = System::getSingleton().getGUISheet();

		if (sheet != NULL)
		{
			// if mouse was released over this widget
			if (this == sheet->getChildAtPosition(e.position))
			{
				// select this button & deselect all others in the same group.
				setSelected(true);
			}

		}

		e.handled = true;
	}

	// default handling
	ButtonBase::onMouseButtonUp(e);
}


/*************************************************************************
	Return a pointer to the RadioButton object within the same group as
	this RadioButton, that is currently selected.
*************************************************************************/
RadioButton* RadioButton::getSelectedButtonInGroup(void) const
{
	// Only search we we are a child window
	if (d_parent != NULL)
	{
		int child_count = d_parent->getChildCount();

		// scan all children
		for (int child = 0; child < child_count; ++child)
		{
			// is this child same type as we are?
			if (d_parent->getChildAtIdx(child)->getType() == getType())
			{
				RadioButton* rb = (RadioButton*)d_parent->getChildAtIdx(child);

				// is child same group and selected?
				if (rb->isSelected() && (rb->getGroupID() == d_groupID))
				{
					// return the matching RadioButton pointer (may even be 'this').
					return rb;
				}

			}

		}

	}

	// no selected button attached to this window is in same group
	return NULL;
}

/*************************************************************************
	Add properties for radio button
*************************************************************************/
void RadioButton::addRadioButtonProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_selectedProperty);
        addProperty(&d_groupIDProperty);
    }
}


} // End of  CEGUI namespace section

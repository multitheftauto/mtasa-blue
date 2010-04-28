/************************************************************************
	filename: 	CEGUIComboDropList.cpp
	created:	13/6/2004
	author:		Paul D Turner
	
	purpose:	Implements the Combobox Drop-List widget base class
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
#include "elements/CEGUIComboDropList.h"
#include "elements/CEGUIScrollbar.h"


// Start of CEGUI namespace section
namespace CEGUI
{
const String ComboDropList::EventNamespace("ComboDropList");

/*************************************************************************
	Constants
*************************************************************************/
// Event names
const String ComboDropList::EventListSelectionAccepted( (utf8*)"ListSelectionAccepted" );


/*************************************************************************
	Constructor for ComboDropList base class
*************************************************************************/
ComboDropList::ComboDropList(const String& type, const String& name) :
	Listbox(type, name)
{
	d_autoArm = false;
	d_armed = false;

	addComboDropListEvents();
	hide();

    // pass captured inputs to children to enable scrollbars
    setDistributesCapturedInputs(true);
}


/*************************************************************************
	Destructor for ComboDropList base class
*************************************************************************/
ComboDropList::~ComboDropList(void)
{
}


/*************************************************************************
	Initialise the Window based object ready for use.	
*************************************************************************/
void ComboDropList::initialise(void)
{
	Listbox::initialise();

	// set-up scroll bars so they return capture to us.
	d_vertScrollbar->setRestoreCapture(true);
	d_horzScrollbar->setRestoreCapture(true);
}


/*************************************************************************
	Add drop-list specific events
*************************************************************************/
void ComboDropList::addComboDropListEvents(bool bCommon)
{
	if ( bCommon == true )	addEvent(EventListSelectionAccepted);
}


/*************************************************************************
	Handler for when list selection is confirmed.
*************************************************************************/
void ComboDropList::onListSelectionAccepted(WindowEventArgs& e)
{
	fireEvent(EventListSelectionAccepted, e, EventNamespace);
}


/*************************************************************************
	Handler for mouse movement events
*************************************************************************/
void ComboDropList::onMouseMove(MouseEventArgs& e)
{
	Listbox::onMouseMove(e);

	// if mouse is within our area (but not our children)
	if (isHit(e.position))
	{
		if (getChildAtPosition(e.position) == NULL)
		{
			// handle auto-arm
			if (d_autoArm)
			{
				d_armed = true;
			}

			if (d_armed)
			{
				//
				// Convert mouse position to absolute window pixels
				//
				Point localPos(screenToWindow(e.position));

				if (getMetricsMode() == Relative)
				{
					localPos = relativeToAbsolute(localPos);
				}

				// check for an item under the mouse
				ListboxItem* selItem = getItemAtPoint(localPos);

				// if an item is under mouse, select it
				if (selItem != NULL)
				{
					setItemSelectState(selItem, true);
				}
				else
				{
					clearAllSelections();
				}

			}
		}

		e.handled = true;
	}
	// not within the list area
	else
	{
		// if left mouse button is down, clear any selection
		if (e.sysKeys & LeftMouse)
		{
			clearAllSelections();
		}

	}

}


/*************************************************************************
	Handler for mouse button pressed events
*************************************************************************/
void ComboDropList::onMouseButtonDown(MouseEventArgs& e)
{
	Listbox::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		if (!isHit(e.position))
		{
			clearAllSelections();
			releaseInput();
		}
		else
		{
			d_armed = true;
		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for mouse button release events
*************************************************************************/
void ComboDropList::onMouseButtonUp(MouseEventArgs& e)
{
	Listbox::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		if (d_armed && (getChildAtPosition(e.position) == NULL))
		{
			releaseInput();

			// if something was selected, confirm that selection.
			if (getSelectedCount() > 0)
			{
				WindowEventArgs args(this);
				onListSelectionAccepted(args);
			}

		}
		// if we are not already armed, in response to a left button up event, we auto-arm.
		else
		{
			d_armed = true;
		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for when input capture is lost
*************************************************************************/
void ComboDropList::onCaptureLost(WindowEventArgs& e)
{
	Listbox::onCaptureLost(e);
	d_armed = false;
	hide();
	e.handled = true;
}


/*************************************************************************
	Handler for when window is activated
*************************************************************************/
void ComboDropList::onActivated(ActivationEventArgs& e)
{
	Listbox::onActivated(e);
}

} // End of  CEGUI namespace section

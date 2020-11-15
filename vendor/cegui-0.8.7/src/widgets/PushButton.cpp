/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner

	purpose:	Implementation of PushButton widget base class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/widgets/PushButton.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	constants
*************************************************************************/
// event strings
const String PushButton::EventNamespace("PushButton");
const String PushButton::WidgetTypeName("CEGUI/PushButton");
const String PushButton::EventClicked( "Clicked" );


/*************************************************************************
	Constructor
*************************************************************************/
PushButton::PushButton(const String& type, const String& name) :
	ButtonBase(type, name)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
PushButton::~PushButton(void)
{
}


/*************************************************************************
	handler invoked internally when the button is clicked.
*************************************************************************/
void PushButton::onClicked(WindowEventArgs& e)
{
	fireEvent(EventClicked, e, EventNamespace);
}


/*************************************************************************
	Handler for mouse button release events
*************************************************************************/
void PushButton::onMouseButtonUp(MouseEventArgs& e)
{
	if ((e.button == LeftButton) && isPushed())
	{
		Window* sheet = getGUIContext().getRootWindow();

		if (sheet)
		{
			// if mouse was released over this widget
            // (use position from mouse, as e.position has been unprojected)
			if (this == sheet->getTargetChildAtPosition(
                getGUIContext().getMouseCursor().getPosition()))
			{
				// fire event
				WindowEventArgs args(this);
				onClicked(args);
			}

		}

		++e.handled;
	}

	// default handling
	ButtonBase::onMouseButtonUp(e);
}

} // End of  CEGUI namespace section

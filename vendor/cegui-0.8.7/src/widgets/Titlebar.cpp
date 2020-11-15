/***********************************************************************
	created:	25/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of common Titlebar parts.
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
#include "CEGUI/widgets/Titlebar.h"
#include "CEGUI/widgets/FrameWindow.h"
#include "CEGUI/MouseCursor.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String Titlebar::EventNamespace("Titlebar");
const String Titlebar::WidgetTypeName("CEGUI/Titlebar");

/*************************************************************************
	Constructor
*************************************************************************/
Titlebar::Titlebar(const String& type, const String& name) :
	Window(type, name)
{
	addTitlebarProperties();
	setAlwaysOnTop(true);

	// basic initialisation
	d_dragging = false;
	d_dragEnabled = true;
}

/*************************************************************************
	Destructor
*************************************************************************/
Titlebar::~Titlebar(void)
{
}


/*************************************************************************
	Return whether this title bar will respond to dragging.
*************************************************************************/
bool Titlebar::isDraggingEnabled(void) const
{
	return d_dragEnabled;
}


/*************************************************************************
	Set whether this title bar widget will respond to dragging.
*************************************************************************/
void Titlebar::setDraggingEnabled(bool setting)
{
	if (d_dragEnabled != setting)
	{
		d_dragEnabled = setting;

		// stop dragging now if the setting has been disabled.
		if ((!d_dragEnabled) && d_dragging)
		{
			releaseInput();
		}

		// call event handler.
		WindowEventArgs args(this);
		onDraggingModeChanged(args);
	}

}

bool Titlebar::isDragged() const
{
    return d_dragging;
}

const Vector2f& Titlebar::getDragPoint() const
{
    return d_dragPoint;
}

/*************************************************************************
	Handler for mouse movement events
*************************************************************************/
void Titlebar::onMouseMove(MouseEventArgs& e)
{
	// Base class processing.
	Window::onMouseMove(e);

	if (d_dragging && (d_parent != 0))
	{
		Vector2f delta(CoordConverter::screenToWindow(*this, e.position));

		// calculate amount that window has been moved
		delta -= d_dragPoint;

		// move the window.  *** Again: Titlebar objects should only be attached to FrameWindow derived classes. ***
		((FrameWindow*)d_parent)->offsetPixelPosition(delta);

		++e.handled;
	}
}


/*************************************************************************
	Handler for mouse button press events
*************************************************************************/
void Titlebar::onMouseButtonDown(MouseEventArgs& e)
{
	// Base class processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		if ((d_parent != 0) && d_dragEnabled)
		{
			// we want all mouse inputs from now on
			if (captureInput())
			{
				// initialise the dragging state
				d_dragging = true;
				d_dragPoint = CoordConverter::screenToWindow(*this, e.position);

                // store old constraint area
                d_oldCursorArea = getGUIContext().
                    getMouseCursor().getConstraintArea();

				// setup new constraint area to be the intersection of the old area and our grand-parent's clipped inner-area
				Rectf constrainArea;

				if ((d_parent == 0) || (getParent()->getParent() == 0))
				{
                    Rectf screen(Vector2f(0, 0), getRootContainerSize());
					constrainArea = screen.getIntersection(d_oldCursorArea);
				}
				else 
				{
					constrainArea = getParent()->getParent()->getInnerRectClipper().getIntersection(d_oldCursorArea);
				}

                getGUIContext().getMouseCursor().
                    setConstraintArea(&constrainArea);
			}
		}

		++e.handled;
	}
}


/*************************************************************************
	Handler for mouse button release events
*************************************************************************/
void Titlebar::onMouseButtonUp(MouseEventArgs& e)
{
	// Base class processing
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		releaseInput();
		++e.handled;
	}

}


/*************************************************************************
	Handler for mouse button double-click events
*************************************************************************/
void Titlebar::onMouseDoubleClicked(MouseEventArgs& e)
{
	// Base class processing
	Window::onMouseDoubleClicked(e);

	if (e.button == LeftButton)
	{
		// if we do not have a parent window, then obviously nothing should happen.
		if (d_parent)
		{
			// we should only ever be attached to a FrameWindow (or derived) class
			((FrameWindow*)d_parent)->toggleRollup();
		}

		++e.handled;
	}

}


/*************************************************************************
	Handler for if the window loses capture of the mouse.
*************************************************************************/
void Titlebar::onCaptureLost(WindowEventArgs& e)
{
	// Base class processing
	Window::onCaptureLost(e);

	// when we lose out hold on the mouse inputs, we are no longer dragging.
	d_dragging = false;

	// restore old constraint area
	getGUIContext().
        getMouseCursor().setConstraintArea(&d_oldCursorArea);
}


/*************************************************************************
	Handler for when the font for this Window is changed
*************************************************************************/
void Titlebar::onFontChanged(WindowEventArgs& e)
{
	Window::onFontChanged(e);

	if (d_parent)
	{
		getParent()->performChildWindowLayout();
	}
}


/*************************************************************************
	Add title bar specific properties
*************************************************************************/
void Titlebar::addTitlebarProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(Titlebar, bool,
        "DraggingEnabled", "Property to get/set the state of the dragging enabled setting for the Titlebar.  Value is either \"true\" or \"false\".",
        &Titlebar::setDraggingEnabled, &Titlebar::isDraggingEnabled, true
    );
}


} // End of  CEGUI namespace section

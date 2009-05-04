/************************************************************************
	filename: 	CEGUITitlebar.cpp
	created:	25/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of common Titlebar parts.
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
#include "elements/CEGUITitlebar.h"
#include "elements/CEGUIFrameWindow.h"
#include "CEGUIMouseCursor.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String Titlebar::EventNamespace("Titlebar");

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
TitlebarProperties::DraggingEnabled	Titlebar::d_dragEnabledProperty;
TitlebarProperties::CaptionColour	Titlebar::d_captionColourProperty;


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


/*************************************************************************
	Handler for mouse movement events
*************************************************************************/
void Titlebar::onMouseMove(MouseEventArgs& e)
{
	// Base class processing.
	Window::onMouseMove(e);

	if (d_dragging && (d_parent != NULL))
	{
		Vector2 delta(screenToWindow(e.position));

		if (getMetricsMode() == Relative)
		{
			delta = relativeToAbsolute(delta);
		}

		// calculate amount that window has been moved
		delta -= d_dragPoint;

		// move the window.  *** Again: Titlebar objects should only be attached to FrameWindow derived classes. ***
		((FrameWindow*)d_parent)->offsetPixelPosition(delta);

		e.handled = true;
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
		if ((d_parent != NULL) && d_dragEnabled)
		{
			// we want all mouse inputs from now on
			if (captureInput())
			{
				// initialise the dragging state
				d_dragging = true;
				d_dragPoint = screenToWindow(e.position);

				if (getMetricsMode() == Relative)
				{
					d_dragPoint = relativeToAbsolute(d_dragPoint);
				}

				// store old constraint area
				d_oldCursorArea = MouseCursor::getSingleton().getConstraintArea();

				// setup new constraint area to be the intersection of the old area and our grand-parent's clipped inner-area
				Rect constrainArea;

				if ((d_parent == NULL) || (d_parent->getParent() == NULL))
				{
					constrainArea = System::getSingleton().getRenderer()->getRect().getIntersection(d_oldCursorArea);
				}
				else 
				{
					constrainArea = d_parent->getParent()->getInnerRect().getIntersection(d_oldCursorArea);
				}

				MouseCursor::getSingleton().setConstraintArea(&constrainArea);
			}

		}

		e.handled = true;
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
		e.handled = true;
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
		if (d_parent != NULL)
		{
			// we should only ever be attached to a FrameWindow (or derived) class
			((FrameWindow*)d_parent)->toggleRollup();
		}

		e.handled = true;
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
	MouseCursor::getSingleton().setConstraintArea(&d_oldCursorArea);
}


/*************************************************************************
	Handler for when the font for this Window is changed
*************************************************************************/
void Titlebar::onFontChanged(WindowEventArgs& e)
{
	Window::onFontChanged(e);

	if (d_parent != NULL)
	{
		d_parent->performChildWindowLayout();
	}
}


/*************************************************************************
	Add title bar specific properties
*************************************************************************/
void Titlebar::addTitlebarProperties( bool bCommon )
{
	if ( bCommon == false )   addProperty(&d_dragEnabledProperty);
	if ( bCommon == true )   addProperty(&d_captionColourProperty);
}


/*************************************************************************
	Return the current colour used for rendering the caption text
*************************************************************************/
colour	Titlebar::getCaptionColour(void) const
{
	return d_captionColour;
}


/*************************************************************************
	Sets the colour to be used for rendering the caption text.
*************************************************************************/
void Titlebar::setCaptionColour(const colour& col)
{
	d_captionColour = col;
	requestRedraw();
}

} // End of  CEGUI namespace section

/************************************************************************
	filename: 	CEGUIButtonBase.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of ButtonBase widget
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
#include "elements/CEGUIButtonBase.h"
#include "CEGUIMouseCursor.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
ButtonBaseProperties::NormalTextColour		ButtonBase::d_normalTextColourProperty;
ButtonBaseProperties::HoverTextColour		ButtonBase::d_hoverTextColourProperty;
ButtonBaseProperties::PushedTextColour		ButtonBase::d_pushedTextColourProperty;
ButtonBaseProperties::DisabledTextColour	ButtonBase::d_disabledTextColourProperty;


/*************************************************************************
	Constants
*************************************************************************/
// default colours for text label rendering
const colour	ButtonBase::DefaultNormalLabelColour	= 0xFFFFFFFF;
const colour	ButtonBase::DefaultHoverLabelColour		= 0xFFFFFFFF;
const colour	ButtonBase::DefaultPushedLabelColour	= 0xFFFFFFFF;
const colour	ButtonBase::DefaultDisabledLabelColour	= 0xFF7F7F7F;


/*************************************************************************
	Constructor
*************************************************************************/
ButtonBase::ButtonBase(const String& type, const String& name) :
	Window(type, name),
	d_pushed(false),
	d_hovering(false),
	d_normalColour(DefaultNormalLabelColour),
	d_hoverColour(DefaultHoverLabelColour),
	d_pushedColour(DefaultPushedLabelColour),
	d_disabledColour(DefaultDisabledLabelColour)
{
	addButtonBaseProperties();
}


/*************************************************************************
	Destructor
*************************************************************************/
ButtonBase::~ButtonBase(void)
{
}


/*************************************************************************
	Update the internal state of the Widget
*************************************************************************/
void ButtonBase::updateInternalState(const Point& mouse_pos)
{
	bool oldstate = d_hovering;

	// assume not hovering 
	d_hovering = false;

	// if input is captured, but not by 'this', then we never hover highlight
	const Window* capture_wnd = getCaptureWindow();

	if ((capture_wnd == NULL) || (capture_wnd == this))
	{
		Window* sheet = System::getSingleton().getGUISheet();

		if (sheet != NULL)
		{
			// check if hovering highlight is required, which is basically ("mouse over widget" XOR "widget pushed").
			if ((this == sheet->getChildAtPosition(mouse_pos)) != d_pushed)
			{
				d_hovering = true;
			}

		}

	}

	// if state has changed, trigger a re-draw
	if (oldstate != d_hovering)
	{
		requestRedraw();
	}

}


/*************************************************************************
	Set the colour to use for the label text when rendering in the
	normal state.	
*************************************************************************/
void ButtonBase::setNormalTextColour(const colour& colour)
{
	if (d_normalColour != colour)
	{
		d_normalColour = colour;
		requestRedraw();
	}

}


/*************************************************************************
	Set the colour to use for the label text when rendering in the
	hover / highlighted states.	
*************************************************************************/
void ButtonBase::setHoverTextColour(const colour& colour)
{
	if (d_hoverColour != colour)
	{
		d_hoverColour = colour;
		requestRedraw();
	}

}


/*************************************************************************
	Set the colour to use for the label text when rendering in the
	pushed state.
*************************************************************************/
void ButtonBase::setPushedTextColour(const colour& colour)
{
	if (d_pushedColour != colour)
	{
		d_pushedColour = colour;
		requestRedraw();
	}

}


/*************************************************************************
	Set the colour to use for the label text when rendering in the
	disabled state.	
*************************************************************************/
void ButtonBase::setDisabledTextColour(const colour& colour)
{
	if (d_disabledColour != colour)
	{
		d_disabledColour = colour;
		requestRedraw();
	}

}


/*************************************************************************
	Perform the rendering for this widget.	
*************************************************************************/
void ButtonBase::drawSelf(float z)
{
	if (isHovering())
	{
		drawHover(z);
	}
	else if (isPushed())
	{
		drawPushed(z);
	}
	else if (isDisabled())
	{
		drawDisabled(z);
	}
	else
	{
		drawNormal(z);
	}

}


/*************************************************************************
	Handler for when the mouse moves
*************************************************************************/
void ButtonBase::onMouseMove(MouseEventArgs& e)
{
	// this is needed to discover whether mouse is in the widget area or not.
	// The same thing used to be done each frame in the rendering method,
	// but in this version the rendering method may not be called every frame
	// so we must discover the internal widget state here - which is actually
	// more efficient anyway.

	// base class processing
	Window::onMouseMove(e);

	updateInternalState(e.position);
	e.handled = true;
}


/*************************************************************************
	Handler for mouse button pressed events
*************************************************************************/
void ButtonBase::onMouseButtonDown(MouseEventArgs& e)
{
	// default processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		if (captureInput())
		{
			d_pushed = true;
			updateInternalState(e.position);
			requestRedraw();
		}

		// event was handled by us.
		e.handled = true;
	}

}


/*************************************************************************
	Handler for mouse button release events
*************************************************************************/
void ButtonBase::onMouseButtonUp(MouseEventArgs& e)
{
	// default processing
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		releaseInput();

		// event was handled by us.
		e.handled = true;
	}

}

/*************************************************************************
	Handler for when mouse capture is lost
*************************************************************************/
void ButtonBase::onCaptureLost(WindowEventArgs& e)
{
	// Default processing
	Window::onCaptureLost(e);

	d_pushed = false;
	updateInternalState(MouseCursor::getSingleton().getPosition());
	requestRedraw();

	// event was handled by us.
	e.handled = true;
}


/*************************************************************************
	Handler for when mouse leaves the widget
*************************************************************************/
void ButtonBase::onMouseLeaves(MouseEventArgs& e)
{
	// deafult processing
	Window::onMouseLeaves(e);

	d_hovering = false;
	requestRedraw();

	e.handled = true;
}


/*************************************************************************
	Add properties for this class
*************************************************************************/
void ButtonBase::addButtonBaseProperties( bool bCommon )
{
    if ( bCommon == true )
    {
        addProperty(&d_normalTextColourProperty);
        addProperty(&d_hoverTextColourProperty);
        addProperty(&d_pushedTextColourProperty);
        addProperty(&d_disabledTextColourProperty);
    }
}

} // End of  CEGUI namespace section

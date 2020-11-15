/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of FrameWindow base class
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
#include "CEGUI/widgets/FrameWindow.h"
#include "CEGUI/widgets/Titlebar.h"
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/MouseCursor.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/CoordConverter.h"

// URGENT FIXME: I commented instances of PixelAligned in here, I think they are not necessary but it should be checked!

// Start of CEGUI namespace section
namespace CEGUI
{
const String FrameWindow::EventNamespace("FrameWindow");
const String FrameWindow::WidgetTypeName("CEGUI/FrameWindow");

/*************************************************************************
	Constants
*************************************************************************/
// additional event names for this window
const String FrameWindow::EventRollupToggled( "RollupToggled" );
const String FrameWindow::EventCloseClicked( "CloseClicked" );
const String FrameWindow::EventDragSizingStarted("DragSizingStarted");
const String FrameWindow::EventDragSizingEnded("DragSizingEnded");

// other bits
const float FrameWindow::DefaultSizingBorderSize	= 8.0f;

/*************************************************************************
    Child Widget name constants
*************************************************************************/
const String FrameWindow::TitlebarName( "__auto_titlebar__" );
const String FrameWindow::CloseButtonName( "__auto_closebutton__" );


/*************************************************************************
	Constructor
*************************************************************************/
FrameWindow::FrameWindow(const String& type, const String& name) :
	Window(type, name)
{
	d_frameEnabled		= true;
	d_rollupEnabled		= true;
	d_rolledup			= false;
	d_sizingEnabled		= true;
	d_beingSized		= false;
	d_dragMovable		= true;

	d_borderSize		= DefaultSizingBorderSize;

	d_nsSizingCursor = d_ewSizingCursor = d_neswSizingCursor = d_nwseSizingCursor = 0;

	addFrameWindowProperties();
}


/*************************************************************************
	Destructor
*************************************************************************/
FrameWindow::~FrameWindow(void)
{
}


/*************************************************************************
	Initialises the Window based object ready for use.
*************************************************************************/
void FrameWindow::initialiseComponents(void)
{
    // get component windows
    Titlebar* titlebar = getTitlebar();
    PushButton* closeButton = getCloseButton();

    // configure titlebar
    titlebar->setDraggingEnabled(d_dragMovable);
    titlebar->setText(getText());

    // ban some properties on components, since they are linked to settings
    // defined here.
    titlebar->banPropertyFromXML("Text");
    titlebar->banPropertyFromXML("Visible");
    titlebar->banPropertyFromXML("Disabled");
    closeButton->banPropertyFromXML("Visible");
    closeButton->banPropertyFromXML("Disabled");

    // bind handler to close button 'Click' event
    closeButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUI::FrameWindow::closeClickHandler, this));

    performChildWindowLayout();
}


/*************************************************************************
    Return whether the title bar for this window is enabled.
*************************************************************************/
bool FrameWindow::isTitleBarEnabled(void) const
{
    return !getTitlebar()->isDisabled();
}


/*************************************************************************
    Return whether this close button for this window is enabled.
*************************************************************************/
bool FrameWindow::isCloseButtonEnabled(void) const
{
    return !getCloseButton()->isDisabled();
}


/*************************************************************************
	Enables or disables sizing for this window.	
*************************************************************************/
void FrameWindow::setSizingEnabled(bool setting)
{
	d_sizingEnabled = setting;
}


/*************************************************************************
	Enables or disables the frame for this window.	
*************************************************************************/
void FrameWindow::setFrameEnabled(bool setting)
{
	d_frameEnabled = setting;
	invalidate();
}


/*************************************************************************
	Enables or disables the title bar for the frame window.	
*************************************************************************/
void FrameWindow::setTitleBarEnabled(bool setting)
{
    Window* titlebar = getTitlebar();
    titlebar->setEnabled(setting);
    titlebar->setVisible(setting);
}


/*************************************************************************
	Enables or disables the close button for the frame window.	
*************************************************************************/
void FrameWindow::setCloseButtonEnabled(bool setting)
{
    Window* closebtn = getCloseButton();
    closebtn->setEnabled(setting);
    closebtn->setVisible(setting);
}


/*************************************************************************
	Enables or disables roll-up (shading) for this window.	
*************************************************************************/
void FrameWindow::setRollupEnabled(bool setting)
{
	if ((setting == false) && isRolledup())
	{
		toggleRollup();
	}

	d_rollupEnabled = setting;
}


/*************************************************************************
	Toggles the state of the window between rolled-up (shaded) and normal
	sizes.  This requires roll-up to be enabled.	
*************************************************************************/
void FrameWindow::toggleRollup(void)
{
    if (isRollupEnabled())
    {
        d_rolledup ^= true;
        
        // event notification.
        WindowEventArgs args(this);
        onRollupToggled(args);

        getGUIContext().updateWindowContainingMouse();
    }

}

void FrameWindow::setRolledup(bool val)
{
    if(val != isRolledup())
    {
        toggleRollup();
    }
}

/*************************************************************************
	Move the window by the pixel offsets specified in 'offset'.	
*************************************************************************/
void FrameWindow::offsetPixelPosition(const Vector2f& offset)
{
    UVector2 uOffset(cegui_absdim(/*PixelAligned(*/offset.d_x/*)*/),
                     cegui_absdim(/*PixelAligned(*/offset.d_y/*)*/));

    setPosition(d_area.getPosition() + uOffset);
}


/*************************************************************************	
	check local pixel co-ordinate point 'pt' and return one of the
	SizingLocation enumerated values depending where the point falls on
	the sizing border.
*************************************************************************/
FrameWindow::SizingLocation FrameWindow::getSizingBorderAtPoint(const Vector2f& pt) const
{
	Rectf frame(getSizingRect());

	// we can only size if the frame is enabled and sizing is on
	if (isSizingEnabled() && isFrameEnabled())
	{
		// point must be inside the outer edge
		if (frame.isPointInRect(pt))
		{
			// adjust rect to get inner edge
			frame.d_min.d_x += d_borderSize;
			frame.d_min.d_y += d_borderSize;
			frame.d_max.d_x -= d_borderSize;
			frame.d_max.d_y -= d_borderSize;

			// detect which edges we are on
			bool top = (pt.d_y < frame.d_min.d_y);
			bool bottom = (pt.d_y >= frame.d_max.d_y);
			bool left = (pt.d_x < frame.d_min.d_x);
			bool right = (pt.d_x >= frame.d_max.d_x);

			// return appropriate 'SizingLocation' value
			if (top && left)
			{
				return SizingTopLeft;
			}
			else if (top && right)
			{
				return SizingTopRight;
			}
			else if (bottom && left)
			{
				return SizingBottomLeft;
			}
			else if (bottom && right)
			{
				return SizingBottomRight;
			}
			else if (top)
			{
				return SizingTop;
			}
			else if (bottom)
			{
				return SizingBottom;
			}
			else if (left)
			{
				return SizingLeft;
			}
			else if (right)
			{
				return SizingRight;
			}

		}

	}

	// deafult: None.
	return SizingNone;
}


/*************************************************************************
	move the window's left edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.	
*************************************************************************/
bool FrameWindow::moveLeftEdge(float delta, URect& out_area)
{
    float orgWidth = d_pixelSize.d_width;

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxWidth(CoordConverter::asAbsolute(d_maxSize.d_width, getRootContainerSize().d_width));
    float minWidth(CoordConverter::asAbsolute(d_minSize.d_width, getRootContainerSize().d_width));
    float newWidth = orgWidth - delta;

    if (maxWidth != 0.0f && newWidth > maxWidth)
        delta = orgWidth - maxWidth;
    else if (newWidth < minWidth)
        delta = orgWidth - minWidth;

    // ensure adjustment will be whole pixel
    float adjustment = /*PixelAligned(*/delta/*)*/;

    if (d_horizontalAlignment == HA_RIGHT)
    {
        out_area.d_max.d_x.d_offset -= adjustment;
    }
    else if (d_horizontalAlignment == HA_CENTRE)
    {
        out_area.d_max.d_x.d_offset -= adjustment * 0.5f;
        out_area.d_min.d_x.d_offset += adjustment * 0.5f;
    }
    else
    {
        out_area.d_min.d_x.d_offset += adjustment;
    }

    return d_horizontalAlignment == HA_LEFT;
}
/*************************************************************************
	move the window's right edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.
*************************************************************************/
bool FrameWindow::moveRightEdge(float delta, URect& out_area)
{
    // store this so we can work out how much size actually changed
    float orgWidth = d_pixelSize.d_width;

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxWidth(CoordConverter::asAbsolute(d_maxSize.d_width, getRootContainerSize().d_width));
    float minWidth(CoordConverter::asAbsolute(d_minSize.d_width, getRootContainerSize().d_width));
    float newWidth = orgWidth + delta;

    if (maxWidth != 0.0f && newWidth > maxWidth)
        delta = maxWidth - orgWidth;
    else if (newWidth < minWidth)
        delta = minWidth - orgWidth;

    // ensure adjustment will be whole pixel
    float adjustment = /*PixelAligned(*/delta/*)*/;

    out_area.d_max.d_x.d_offset += adjustment;

    if (d_horizontalAlignment == HA_RIGHT)
    {
        out_area.d_max.d_x.d_offset += adjustment;
        out_area.d_min.d_x.d_offset += adjustment;
    }
    else if (d_horizontalAlignment == HA_CENTRE)
    {
        out_area.d_max.d_x.d_offset += adjustment * 0.5f;
        out_area.d_min.d_x.d_offset += adjustment * 0.5f;
    }

    // move the dragging point so mouse remains 'attached' to edge of window
    d_dragPoint.d_x += adjustment;

    return d_horizontalAlignment == HA_RIGHT;
}

/*************************************************************************
	move the window's top edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.
*************************************************************************/
bool FrameWindow::moveTopEdge(float delta, URect& out_area)
{
    float orgHeight = d_pixelSize.d_height;

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxHeight(CoordConverter::asAbsolute(d_maxSize.d_height, getRootContainerSize().d_height));
    float minHeight(CoordConverter::asAbsolute(d_minSize.d_height, getRootContainerSize().d_height));
    float newHeight = orgHeight - delta;

    if (maxHeight != 0.0f && newHeight > maxHeight)
        delta = orgHeight - maxHeight;
    else if (newHeight < minHeight)
        delta = orgHeight - minHeight;

    // ensure adjustment will be whole pixel
    float adjustment = /*PixelAligned(*/delta/*)*/;

    if (d_verticalAlignment == VA_BOTTOM)
    {
        out_area.d_max.d_y.d_offset -= adjustment;
    }
    else if (d_verticalAlignment == VA_CENTRE)
    {
        out_area.d_max.d_y.d_offset -= adjustment * 0.5f;
        out_area.d_min.d_y.d_offset += adjustment * 0.5f;
    }
    else
    {
        out_area.d_min.d_y.d_offset += adjustment;
    }

    return d_verticalAlignment == VA_TOP;
}


/*************************************************************************
	move the window's bottom edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.	
*************************************************************************/
bool FrameWindow::moveBottomEdge(float delta, URect& out_area)
{
    // store this so we can work out how much size actually changed
    float orgHeight = d_pixelSize.d_height;

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxHeight(CoordConverter::asAbsolute(d_maxSize.d_height, getRootContainerSize().d_height));
    float minHeight(CoordConverter::asAbsolute(d_minSize.d_height, getRootContainerSize().d_height));
    float newHeight = orgHeight + delta;

    if (maxHeight != 0.0f && newHeight > maxHeight)
        delta = maxHeight - orgHeight;
    else if (newHeight < minHeight)
        delta = minHeight - orgHeight;

    // ensure adjustment will be whole pixel
    float adjustment = /*PixelAligned(*/delta/*)*/;

    out_area.d_max.d_y.d_offset += adjustment;

    if (d_verticalAlignment == VA_BOTTOM)
    {
        out_area.d_max.d_y.d_offset += adjustment;
        out_area.d_min.d_y.d_offset += adjustment;
    }
    else if (d_verticalAlignment == VA_CENTRE)
    {
        out_area.d_max.d_y.d_offset += adjustment * 0.5f;
        out_area.d_min.d_y.d_offset += adjustment * 0.5f;
    }

    // move the dragging point so mouse remains 'attached' to edge of window
    d_dragPoint.d_y += adjustment;

    return d_verticalAlignment == VA_BOTTOM;
}


/*************************************************************************
	Handler to map close button clicks to FrameWindow 'CloseCliked' events
*************************************************************************/
bool FrameWindow::closeClickHandler(const EventArgs&)
{
    WindowEventArgs args(this);
	onCloseClicked(args);

	return true;
}


/*************************************************************************
	Set the appropriate mouse cursor for the given window-relative pixel
	point.
*************************************************************************/
void FrameWindow::setCursorForPoint(const Vector2f& pt) const
{
	switch(getSizingBorderAtPoint(pt))
	{
	case SizingTop:
	case SizingBottom:
		getGUIContext().
            getMouseCursor().setImage(d_nsSizingCursor);
		break;

	case SizingLeft:
	case SizingRight:
		getGUIContext().
            getMouseCursor().setImage(d_ewSizingCursor);
		break;

	case SizingTopLeft:
	case SizingBottomRight:
		getGUIContext().
            getMouseCursor().setImage(d_nwseSizingCursor);
		break;

	case SizingTopRight:
	case SizingBottomLeft:
		getGUIContext().
            getMouseCursor().setImage(d_neswSizingCursor);
		break;

	default:
		getGUIContext().
            getMouseCursor().setImage(getMouseCursor());
		break;
	}

}


/*************************************************************************
	Event generated internally whenever the roll-up / shade state of the
	window changes.
*************************************************************************/
void FrameWindow::onRollupToggled(WindowEventArgs& e)
{
    invalidate(true);
    notifyScreenAreaChanged();
    ElementEventArgs size_args(e.window);
    onSized(size_args);

	fireEvent(EventRollupToggled, e, EventNamespace);
}


/*************************************************************************
	Event generated internally whenever the close button is clicked.	
*************************************************************************/
void FrameWindow::onCloseClicked(WindowEventArgs& e)
{
	fireEvent(EventCloseClicked, e, EventNamespace);
}


/*************************************************************************
	Handler for mouse move events
*************************************************************************/
void FrameWindow::onMouseMove(MouseEventArgs& e)
{
	// default processing (this is now essential as it controls event firing).
	Window::onMouseMove(e);

	// if we are not the window containing the mouse, do NOT change the cursor
	if (getGUIContext().getWindowContainingMouse() != this)
	{
		return;
	}

	if (isSizingEnabled())
	{
		Vector2f localMousePos(CoordConverter::screenToWindow(*this, e.position));

		if (d_beingSized)
		{
			SizingLocation dragEdge = getSizingBorderAtPoint(d_dragPoint);

			// calculate sizing deltas...
			float	deltaX = localMousePos.d_x - d_dragPoint.d_x;
			float	deltaY = localMousePos.d_y - d_dragPoint.d_y;

            URect new_area(d_area);
            bool top_left_sizing = false;
			// size left or right edges
			if (isLeftSizingLocation(dragEdge))
			{
				top_left_sizing |= moveLeftEdge(deltaX, new_area);
			}
			else if (isRightSizingLocation(dragEdge))
			{
				top_left_sizing |= moveRightEdge(deltaX, new_area);
			}

			// size top or bottom edges
			if (isTopSizingLocation(dragEdge))
			{
				top_left_sizing |= moveTopEdge(deltaY, new_area);
			}
			else if (isBottomSizingLocation(dragEdge))
			{
				top_left_sizing |= moveBottomEdge(deltaY, new_area);
			}

            setArea_impl(new_area.d_min, new_area.getSize(), top_left_sizing);
		}
		else
		{
			setCursorForPoint(localMousePos);
		}

	}

	// mark event as handled
	++e.handled;
}


/*************************************************************************
	Handler for mouse button down events
*************************************************************************/
void FrameWindow::onMouseButtonDown(MouseEventArgs& e)
{
	// default processing (this is now essential as it controls event firing).
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		if (isSizingEnabled())
		{
			// get position of mouse as co-ordinates local to this window.
			Vector2f localPos(CoordConverter::screenToWindow(*this, e.position));

			// if the mouse is on the sizing border
			if (getSizingBorderAtPoint(localPos) != SizingNone)
			{
				// ensure all inputs come to us for now
				if (captureInput())
				{
					// setup the 'dragging' state variables
					d_beingSized = true;
					d_dragPoint = localPos;

                    // do drag-sizing started notification
                    WindowEventArgs args(this);
                    onDragSizingStarted(args);

					++e.handled;
				}

			}

		}

	}

}


/*************************************************************************
	Handler for mouse button up events
*************************************************************************/
void FrameWindow::onMouseButtonUp(MouseEventArgs& e)
{
	// default processing (this is now essential as it controls event firing).
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton && isCapturedByThis())
	{
		// release our capture on the input data
		releaseInput();
		++e.handled;
	}

}


/*************************************************************************
	Handler for when mouse capture is lost
*************************************************************************/
void FrameWindow::onCaptureLost(WindowEventArgs& e)
{
	// default processing (this is now essential as it controls event firing).
	Window::onCaptureLost(e);

	// reset sizing state
	d_beingSized = false;

    // do drag-sizing ended notification
    WindowEventArgs args(this);
    onDragSizingEnded(args);

	++e.handled;
}


/*************************************************************************
    Handler for when text changes
*************************************************************************/
void FrameWindow::onTextChanged(WindowEventArgs& e)
{
    Window::onTextChanged(e);
    // pass this onto titlebar component.
    getTitlebar()->setText(getText());
    // maybe the user is using a fontdim for titlebar dimensions ;)
    performChildWindowLayout();
}


/*************************************************************************
    Handler for when this Window is activated
*************************************************************************/
void FrameWindow::onActivated(ActivationEventArgs& e)
{
	Window::onActivated(e);
	getTitlebar()->invalidate();
}


/*************************************************************************
    Handler for when this Window is deactivated
*************************************************************************/
void FrameWindow::onDeactivated(ActivationEventArgs& e)
{
	Window::onDeactivated(e);
	getTitlebar()->invalidate();
}


/*************************************************************************
	Set whether this FrameWindow can be moved by dragging the title bar.	
*************************************************************************/
void FrameWindow::setDragMovingEnabled(bool setting)
{
	if (d_dragMovable != setting)
	{
		d_dragMovable = setting;

        getTitlebar()->setDraggingEnabled(setting);
    }

}


/*************************************************************************
	Add properties for this class
*************************************************************************/
void FrameWindow::addFrameWindowProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "SizingEnabled", "Property to get/set the state of the sizable setting for the FrameWindow. Value is either \"true\" or \"false\".",
        &FrameWindow::setSizingEnabled, &FrameWindow::isSizingEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "FrameEnabled", "Property to get/set the setting for whether the window frame will be displayed. Value is either \"true\" or \"false\".",
        &FrameWindow::setFrameEnabled, &FrameWindow::isFrameEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "TitlebarEnabled", "Property to get/set the setting for whether the window title-bar will be enabled (or displayed depending upon choice of final widget type). Value is either \"true\" or \"false\".",
        &FrameWindow::setTitleBarEnabled, &FrameWindow::isTitleBarEnabled, true
    ); // TODO: Inconsistency between Titlebar and TitleBar

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "CloseButtonEnabled", "Property to get/set the setting for whether the window close button will be enabled (or displayed depending upon choice of final widget type). Value is either \"true\" or \"false\".",
        &FrameWindow::setCloseButtonEnabled, &FrameWindow::isCloseButtonEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "RollUpEnabled", "Property to get/set the setting for whether the user is able to roll-up / shade the window. Value is either \"true\" or \"false\".",
        &FrameWindow::setRollupEnabled, &FrameWindow::isRollupEnabled, true
    ); // TODO: Inconsistency between RollUp and Rollup

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "RollUpState", "Property to get/set the roll-up / shade state of the window.  Value is either \"true\" or \"false\".",
        &FrameWindow::setRolledup, &FrameWindow::isRolledup, false /* TODO: Inconsistency */
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, bool,
        "DragMovingEnabled", "Property to get/set the setting for whether the user may drag the window around by its title bar. Value is either \"true\" or \"false\".",
        &FrameWindow::setDragMovingEnabled, &FrameWindow::isDragMovingEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, float,
        "SizingBorderThickness", "Property to get/set the setting for the sizing border thickness. Value is a float specifying the border thickness in pixels.",
        &FrameWindow::setSizingBorderThickness, &FrameWindow::getSizingBorderThickness, 8.0f
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, Image*,
        "NSSizingCursorImage", "Property to get/set the N-S (up-down) sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\".",
        &FrameWindow::setNSSizingCursorImage, &FrameWindow::getNSSizingCursorImage, 0
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, Image*,
        "EWSizingCursorImage", "Property to get/set the E-W (left-right) sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\".",
        &FrameWindow::setEWSizingCursorImage, &FrameWindow::getEWSizingCursorImage, 0
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, Image*,
        "NWSESizingCursorImage", "Property to get/set the NW-SE diagonal sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\".",
        &FrameWindow::setNWSESizingCursorImage, &FrameWindow::getNWSESizingCursorImage, 0
    );

    CEGUI_DEFINE_PROPERTY(FrameWindow, Image*,
        "NESWSizingCursorImage", "Property to get/set the NE-SW diagonal sizing cursor image for the FramwWindow. Value should be \"set:[imageset name] image:[image name]\".",
        &FrameWindow::setNESWSizingCursorImage, &FrameWindow::getNESWSizingCursorImage, 0
    );
}

/*************************************************************************
    return the image used for the north-south sizing cursor.
*************************************************************************/
const Image* FrameWindow::getNSSizingCursorImage() const
{
    return d_nsSizingCursor;
}

/*************************************************************************
    return the image used for the east-west sizing cursor.
*************************************************************************/
const Image* FrameWindow::getEWSizingCursorImage() const
{
    return d_ewSizingCursor;
}

/*************************************************************************
    return the image used for the northwest-southeast sizing cursor.
*************************************************************************/
const Image* FrameWindow::getNWSESizingCursorImage() const
{
    return d_nwseSizingCursor;
}

/*************************************************************************
    return the image used for the northeast-southwest sizing cursor.
*************************************************************************/
const Image* FrameWindow::getNESWSizingCursorImage() const
{
    return d_neswSizingCursor;
}

/*************************************************************************
    set the image used for the north-south sizing cursor.
*************************************************************************/
void FrameWindow::setNSSizingCursorImage(const Image* image)
{
    d_nsSizingCursor = image;
}

/*************************************************************************
    set the image used for the east-west sizing cursor.
*************************************************************************/
void FrameWindow::setEWSizingCursorImage(const Image* image)
{
    d_ewSizingCursor = image;
}

/*************************************************************************
    set the image used for the northwest-southeast sizing cursor.
*************************************************************************/
void FrameWindow::setNWSESizingCursorImage(const Image* image)
{
    d_nwseSizingCursor = image;
}

/*************************************************************************
    set the image used for the northeast-southwest sizing cursor.
*************************************************************************/
void FrameWindow::setNESWSizingCursorImage(const Image* image)
{
    d_neswSizingCursor = image;
}

/*************************************************************************
    set the image used for the north-south sizing cursor.
*************************************************************************/
void FrameWindow::setNSSizingCursorImage(const String& name)
{
    d_nsSizingCursor = &ImageManager::getSingleton().get(name);
}

/*************************************************************************
    set the image used for the east-west sizing cursor.
*************************************************************************/
void FrameWindow::setEWSizingCursorImage(const String& name)
{
    d_ewSizingCursor = &ImageManager::getSingleton().get(name);
}

/*************************************************************************
    set the image used for the northwest-southeast sizing cursor.
*************************************************************************/
void FrameWindow::setNWSESizingCursorImage(const String& name)
{
    d_nwseSizingCursor = &ImageManager::getSingleton().get(name);
}

/*************************************************************************
    set the image used for the northeast-southwest sizing cursor.
*************************************************************************/
void FrameWindow::setNESWSizingCursorImage(const String& name)
{
    d_neswSizingCursor = &ImageManager::getSingleton().get(name);
}

/*************************************************************************
    Return a pointer to the Titlebar component widget for this FrameWindow.
*************************************************************************/
Titlebar* FrameWindow::getTitlebar() const
{
    return static_cast<Titlebar*>(getChild(TitlebarName));
}

/*************************************************************************
    Return a pointer to the close button component widget for this
    FrameWindow.
*************************************************************************/
PushButton* FrameWindow::getCloseButton() const
{
    return static_cast<PushButton*>(getChild(CloseButtonName));
}

//----------------------------------------------------------------------------//
void FrameWindow::onDragSizingStarted(WindowEventArgs& e)
{
	fireEvent(EventDragSizingStarted, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void FrameWindow::onDragSizingEnded(WindowEventArgs& e)
{
	fireEvent(EventDragSizingEnded, e, EventNamespace);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section


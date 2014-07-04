/************************************************************************
	filename: 	CEGUIFrameWindow.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of FrameWindow base class
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
#include "elements/CEGUIFrameWindow.h"
#include "elements/CEGUITitlebar.h"
#include "elements/CEGUIPushButton.h"
#include "CEGUIMouseCursor.h"
#include "CEGUIWindowManager.h"
#include "CEGUIExceptions.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String FrameWindow::EventNamespace("FrameWindow");

/*************************************************************************
	Definitions for Properties
*************************************************************************/
FrameWindowProperties::SizingEnabled			FrameWindow::d_sizingEnabledProperty;
FrameWindowProperties::FrameEnabled				FrameWindow::d_frameEnabledProperty;
FrameWindowProperties::TitlebarEnabled			FrameWindow::d_titlebarEnabledProperty;
FrameWindowProperties::CloseButtonEnabled		FrameWindow::d_closeButtonEnabledProperty;
FrameWindowProperties::RollUpState				FrameWindow::d_rollUpStateProperty;
FrameWindowProperties::RollUpEnabled			FrameWindow::d_rollUpEnabledProperty;
FrameWindowProperties::DragMovingEnabled		FrameWindow::d_dragMovingEnabledProperty;
FrameWindowProperties::SizingBorderThickness	FrameWindow::d_sizingBorderThicknessProperty;
FrameWindowProperties::TitlebarFont				FrameWindow::d_titlebarFontProperty;
FrameWindowProperties::CaptionColour			FrameWindow::d_captionColourProperty;
FrameWindowProperties::NSSizingCursorImage      FrameWindow::d_nsSizingCursorProperty;
FrameWindowProperties::EWSizingCursorImage      FrameWindow::d_ewSizingCursorProperty;
FrameWindowProperties::NWSESizingCursorImage    FrameWindow::d_nwseSizingCursorProperty;
FrameWindowProperties::NESWSizingCursorImage    FrameWindow::d_neswSizingCursorProperty;


/*************************************************************************
	Constants
*************************************************************************/
// additional event names for this window
const String FrameWindow::EventRollupToggled( (utf8*)"RollupToggled" );
const String FrameWindow::EventCloseClicked( (utf8*)"CloseClicked" );

// other bits
const float FrameWindow::DefaultSizingBorderSize	= 8.0f;


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

	d_nsSizingCursor = d_ewSizingCursor = d_neswSizingCursor = d_nwseSizingCursor = NULL;

	addFrameWindowEvents();
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
void FrameWindow::initialise(void)
{
	// create child windows
	d_titlebar		= createTitlebar(getName() + "__auto_titlebar__");
	d_closeButton	= createCloseButton(getName() + "__auto_closebutton__");

	// add child controls
	if (d_titlebar != NULL)
	{
		d_titlebar->setDraggingEnabled(d_dragMovable);
		addChildWindow(d_titlebar);
	}

	if (d_closeButton != NULL)
	{
		addChildWindow(d_closeButton);

		// bind handler to close button 'Click' event
		d_closeButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUI::FrameWindow::closeClickHandler, this));
	}

	performChildWindowLayout();
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
	requestRedraw();
}


/*************************************************************************
	Enables or disables the title bar for the frame window.	
*************************************************************************/
void FrameWindow::setTitleBarEnabled(bool setting)
{
    try
    {
        Window* titlebar = WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__");
        titlebar->setEnabled(setting);
        titlebar->setVisible(setting);
    }
    catch (UnknownObjectException)
    {}
}


/*************************************************************************
	Enables or disables the close button for the frame window.	
*************************************************************************/
void FrameWindow::setCloseButtonEnabled(bool setting)
{
    try
    {
        Window* closebtn = WindowManager::getSingleton().getWindow(getName() + "__auto_closebutton__");
        closebtn->setEnabled(setting);
        closebtn->setVisible(setting);
    }
    catch (UnknownObjectException)
    {}
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
    }

}


/*************************************************************************
	Set the font to use for the title bar text	
*************************************************************************/
void FrameWindow::setTitlebarFont(const String& name)
{
    try
    {
        WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__")->setFont(name);
    }
    catch (UnknownObjectException)
    {}
}


/*************************************************************************
	Set the font to use for the title bar text	
*************************************************************************/
void FrameWindow::setTitlebarFont(Font* font)
{
    try
    {
        WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__")->setFont(font);
    }
    catch (UnknownObjectException)
    {}
}


/*************************************************************************
	Move the window by the pixel offsets specified in 'offset'.	
*************************************************************************/
void FrameWindow::offsetPixelPosition(const Vector2& offset)
{
    UVector2 uOffset;

    if (getMetricsMode() == Relative)
    {
        Size sz = getParentSize();

        uOffset.d_x = cegui_reldim((sz.d_width != 0) ? offset.d_x / sz.d_width : 0);
        uOffset.d_y = cegui_reldim((sz.d_height != 0) ? offset.d_y / sz.d_height : 0);
    }
    else
    {
        uOffset.d_x = cegui_absdim(PixelAligned(offset.d_x));
        uOffset.d_y = cegui_absdim(PixelAligned(offset.d_y));
    }

    setWindowPosition(d_area.getPosition() + uOffset);
}


/*************************************************************************	
	check local pixel co-ordinate point 'pt' and return one of the
	SizingLocation enumerated values depending where the point falls on
	the sizing border.
*************************************************************************/
FrameWindow::SizingLocation FrameWindow::getSizingBorderAtPoint(const Point& pt) const
{
	Rect	frame(getSizingRect());

	// we can only size if the frame is enabled and sizing is on
	if (isSizingEnabled() && isFrameEnabled())
	{
		// point must be inside the outer edge
		if (frame.isPointInRect(pt))
		{
			// adjust rect to get inner edge
			frame.d_left	+= d_borderSize;
			frame.d_top		+= d_borderSize;
			frame.d_right	-= d_borderSize;
			frame.d_bottom	-= d_borderSize;

			// detect which edges we are on
			bool top	= (pt.d_y < frame.d_top);
			bool bottom = (pt.d_y >= frame.d_bottom);
			bool left	= (pt.d_x < frame.d_left);
			bool right	= (pt.d_x >= frame.d_right);

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
void FrameWindow::moveLeftEdge(float delta)
{
    float orgWidth = getAbsoluteWidth();
    float adjustment;
    float* minDim;
    float* maxDim;
    URect area(d_area);

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxWidth(d_maxSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float minWidth(d_minSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float newWidth = orgWidth - delta;

    if (newWidth > maxWidth)
        delta = orgWidth - maxWidth;
    else if (newWidth < minWidth)
        delta = orgWidth - minWidth;

    // we use the active metrics mode to decide which component of the window edge to modify
    if (getMetricsMode() == Relative)
    {
        float base = getParentWidth();
        adjustment = (base != 0) ? delta / base : 0;
        minDim = &area.d_min.d_x.d_scale;
        maxDim = &area.d_max.d_x.d_scale;
    }
    else
    {
        adjustment = PixelAligned(delta);
        minDim = &area.d_min.d_x.d_offset;
        maxDim = &area.d_max.d_x.d_offset;
    }

    if (d_horzAlign == HA_RIGHT)
    {
        *maxDim -= adjustment;
    }
    else if (d_horzAlign == HA_CENTRE)
    {
        *maxDim -= adjustment * 0.5f;
        *minDim += adjustment * 0.5f;
    }
    else
    {
        *minDim += adjustment;
    }

    setWindowArea_impl(area.d_min, area.getSize(), d_horzAlign == HA_LEFT);
}
/*************************************************************************
	move the window's right edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.
*************************************************************************/
void FrameWindow::moveRightEdge(float delta)
{
    // store this so we can work out how much size actually changed
    float orgWidth = getAbsoluteWidth();
    float adjustment;
    float* minDim;
    float* maxDim;
    URect area(d_area);

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxWidth(d_maxSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float minWidth(d_minSize.d_x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
    float newWidth = orgWidth + delta;

    if (newWidth > maxWidth)
        delta = maxWidth - orgWidth;
    else if (newWidth < minWidth)
        delta = minWidth - orgWidth;
    
    // we use the active metrics mode to decide which component of the window edge to modify
    if (getMetricsMode() == Relative)
    {
        float base = getParentWidth();
        adjustment = (base != 0) ? delta / base : 0;
        minDim = &area.d_min.d_x.d_scale;
        maxDim = &area.d_max.d_x.d_scale;
    }
    else
    {
        adjustment = PixelAligned(delta);
        minDim = &area.d_min.d_x.d_offset;
        maxDim = &area.d_max.d_x.d_offset;
    }

    *maxDim += adjustment;

    if (d_horzAlign == HA_RIGHT)
    {
        *maxDim += adjustment;
        *minDim += adjustment;
    }
    else if (d_horzAlign == HA_CENTRE)
    {
        *maxDim += adjustment * 0.5f;
        *minDim += adjustment * 0.5f;
    }

    setWindowArea_impl(area.d_min, area.getSize(), d_horzAlign == HA_RIGHT);

    // move the dragging point so mouse remains 'attached' to edge of window
    d_dragPoint.d_x += getAbsoluteWidth() - orgWidth;
}

/*************************************************************************
	move the window's top edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.
*************************************************************************/
void FrameWindow::moveTopEdge(float delta)
{
    float orgHeight = getAbsoluteHeight();
    float adjustment;
    float* minDim;
    float* maxDim;
    URect area(d_area);

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxHeight(d_maxSize.d_y.asAbsolute(System::getSingleton().getRenderer()->getHeight()));
    float minHeight(d_minSize.d_y.asAbsolute(System::getSingleton().getRenderer()->getHeight()));
    float newHeight = orgHeight - delta;

    if (newHeight > maxHeight)
        delta = orgHeight - maxHeight;
    else if (newHeight < minHeight)
        delta = orgHeight - minHeight;

    // we use the active metrics mode to decide which component of the window edge to modify
    if (getMetricsMode() == Relative)
    {
        float base = getParentHeight();
        adjustment = (base != 0) ? delta / base : 0;
        minDim = &area.d_min.d_y.d_scale;
        maxDim = &area.d_max.d_y.d_scale;
    }
    else
    {
        adjustment = PixelAligned(delta);
        minDim = &area.d_min.d_y.d_offset;
        maxDim = &area.d_max.d_y.d_offset;
    }

    if (d_vertAlign == VA_BOTTOM)
    {
        *maxDim -= adjustment;
    }
    else if (d_vertAlign == VA_CENTRE)
    {
        *maxDim -= adjustment * 0.5f;
        *minDim += adjustment * 0.5f;
    }
    else
    {
        *minDim += adjustment;
    }

    setWindowArea_impl(area.d_min, area.getSize(), d_vertAlign == VA_TOP);
}


/*************************************************************************
	move the window's bottom edge by 'delta'.  The rest of the window
	does not move, thus this changes the size of the Window.	
*************************************************************************/
void FrameWindow::moveBottomEdge(float delta)
{
    // store this so we can work out how much size actually changed
    float orgHeight = getAbsoluteHeight();
    float adjustment;
    float* minDim;
    float* maxDim;
    URect area(d_area);

    // ensure that we only size to the set constraints.
    //
    // NB: We are required to do this here due to our virtually unique sizing nature; the
    // normal system for limiting the window size is unable to supply the information we
    // require for updating our internal state used to manage the dragging, etc.
    float maxHeight(d_maxSize.d_y.asAbsolute(System::getSingleton().getRenderer()->getHeight()));
    float minHeight(d_minSize.d_y.asAbsolute(System::getSingleton().getRenderer()->getHeight()));
    float newHeight = orgHeight + delta;

    if (newHeight > maxHeight)
        delta = maxHeight - orgHeight;
    else if (newHeight < minHeight)
        delta = minHeight - orgHeight;
    
    // we use the active metrics mode to decide which component of the window edge to modify
    if (getMetricsMode() == Relative)
    {
        float base = getParentHeight();
        adjustment = (base != 0) ? delta / base : 0;
        minDim = &area.d_min.d_y.d_scale;
        maxDim = &area.d_max.d_y.d_scale;
    }
    else
    {
        adjustment = PixelAligned(delta);
        minDim = &area.d_min.d_y.d_offset;
        maxDim = &area.d_max.d_y.d_offset;
    }

    *maxDim += adjustment;

    if (d_vertAlign == VA_BOTTOM)
    {
        *maxDim += adjustment;
        *minDim += adjustment;
    }
    else if (d_vertAlign == VA_CENTRE)
    {
        *maxDim += adjustment * 0.5f;
        *minDim += adjustment * 0.5f;
    }

    setWindowArea_impl(area.d_min, area.getSize(), d_vertAlign == VA_BOTTOM);

    // move the dragging point so mouse remains 'attached' to edge of window
    d_dragPoint.d_y += getAbsoluteHeight() - orgHeight;
}


/*************************************************************************
	Add frame window specific events	
*************************************************************************/
void FrameWindow::addFrameWindowEvents(bool bCommon)
{
	if ( bCommon == false )	addEvent(EventRollupToggled);
	if ( bCommon == true )	addEvent(EventCloseClicked);
}


/*************************************************************************
	Handler to map close button clicks to FrameWindow 'CloseCliked' events
*************************************************************************/
bool FrameWindow::closeClickHandler(const EventArgs& e)
{
    WindowEventArgs args(this);
	onCloseClicked(args);

	return true;
}


/*************************************************************************
	Set the appropriate mouse cursor for the given window-relative pixel
	point.
*************************************************************************/
void FrameWindow::setCursorForPoint(const Point& pt) const
{
	switch(getSizingBorderAtPoint(pt))
	{
	case SizingTop:
	case SizingBottom:
		MouseCursor::getSingleton().setImage(d_nsSizingCursor);
		break;

	case SizingLeft:
	case SizingRight:
		MouseCursor::getSingleton().setImage(d_ewSizingCursor);
		break;

	case SizingTopLeft:
	case SizingBottomRight:
		MouseCursor::getSingleton().setImage(d_nwseSizingCursor);
		break;

	case SizingTopRight:
	case SizingBottomLeft:
		MouseCursor::getSingleton().setImage(d_neswSizingCursor);
		break;

	default:
		MouseCursor::getSingleton().setImage(getMouseCursor());
		break;
	}

}


/*************************************************************************
	Event generated internally whenever the roll-up / shade state of the
	window changes.
*************************************************************************/
void FrameWindow::onRollupToggled(WindowEventArgs& e)
{
    requestRedraw();

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
	if (System::getSingleton().getWindowContainingMouse() != this)
	{
		return;
	}

	if (isSizingEnabled())
	{
		Point localMousePos(screenToWindow(e.position));

		if (getMetricsMode() == Relative)
		{
			localMousePos = relativeToAbsolute(localMousePos);
		}

		if (d_beingSized)
		{
			SizingLocation dragEdge = getSizingBorderAtPoint(d_dragPoint);

			// calculate sizing deltas...
			float	deltaX = localMousePos.d_x - d_dragPoint.d_x;
			float	deltaY = localMousePos.d_y - d_dragPoint.d_y;

			// size left or right edges
			if (isLeftSizingLocation(dragEdge))
			{
				moveLeftEdge(deltaX);
			}
			else if (isRightSizingLocation(dragEdge))
			{
				moveRightEdge(deltaX);
			}

			// size top or bottom edges
			if (isTopSizingLocation(dragEdge))
			{
				moveTopEdge(deltaY);
			}
			else if (isBottomSizingLocation(dragEdge))
			{
				moveBottomEdge(deltaY);
			}

		}
		else
		{
			setCursorForPoint(localMousePos);
		}

	}

	// mark event as handled
	e.handled = true;
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
			Point localPos(screenToWindow(e.position));

			if (getMetricsMode() == Relative)
			{
				localPos = relativeToAbsolute(localPos);
			}

			// if the mouse is on the sizing border
			if (getSizingBorderAtPoint(localPos) != SizingNone)
			{
				// ensure all inputs come to us for now
				if (captureInput())
				{
					// setup the 'dragging' state variables
					d_beingSized = true;
					d_dragPoint = localPos;
				}

			}

		}

		e.handled = true;
	}

}


/*************************************************************************
	Handler for mouse button up events
*************************************************************************/
void FrameWindow::onMouseButtonUp(MouseEventArgs& e)
{
	// default processing (this is now essential as it controls event firing).
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		// release our capture on the input data
		releaseInput();
		e.handled = true;
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

	e.handled = true;
}


/*************************************************************************
    Handler for when text changes
*************************************************************************/
void FrameWindow::onTextChanged(WindowEventArgs& e)
{
    // pass this onto titlebar component.
    WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__")->setText(getText());
    // redo child layout
    performChildWindowLayout();
}


/*************************************************************************
    Handler for when this Window is activated
*************************************************************************/
void FrameWindow::onActivated(ActivationEventArgs& e)
{
	Window::onActivated(e);
	d_titlebar->requestRedraw();
}


/*************************************************************************
    Handler for when this Window is deactivated
*************************************************************************/
void FrameWindow::onDeactivated(ActivationEventArgs& e)
{
	Window::onDeactivated(e);
	d_titlebar->requestRedraw();
}


/*************************************************************************
	Set whether this FrameWindow can be moved by dragging the title bar.	
*************************************************************************/
void FrameWindow::setDragMovingEnabled(bool setting)
{
	if (d_dragMovable != setting)
	{
		d_dragMovable = setting;

        try
        {
            static_cast<Titlebar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__"))->setDraggingEnabled(setting);
        }
        catch (UnknownObjectException)
        {}
    }

}


/*************************************************************************
	Return the font being used for the title bar text
*************************************************************************/
const Font* FrameWindow::getTitlebarFont(void) const
{
    try
    {
        return WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__")->getFont();
    }
    catch (UnknownObjectException)
    {
        return 0;
    }
}


/*************************************************************************
	Add properties for this class
*************************************************************************/
void FrameWindow::addFrameWindowProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_sizingEnabledProperty);
        addProperty(&d_frameEnabledProperty);
        addProperty(&d_titlebarEnabledProperty);
        addProperty(&d_closeButtonEnabledProperty);
        addProperty(&d_rollUpStateProperty);
        addProperty(&d_dragMovingEnabledProperty);
        addProperty(&d_sizingBorderThicknessProperty);
        addProperty(&d_titlebarFontProperty);
        addProperty(&d_captionColourProperty);
    }
    else
    {
        addProperty(&d_rollUpEnabledProperty);
        addProperty(&d_nsSizingCursorProperty);
        addProperty(&d_ewSizingCursorProperty);
        addProperty(&d_nwseSizingCursorProperty);
        addProperty(&d_neswSizingCursorProperty);
    }
}


/*************************************************************************
	Return the current colour used for rendering the caption text
*************************************************************************/
colour FrameWindow::getCaptionColour(void) const
{
    return static_cast<Titlebar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__"))->getCaptionColour();
}


/*************************************************************************
	Sets the colour to be used for rendering the caption text.
*************************************************************************/
void FrameWindow::setCaptionColour(colour col)
{
    static_cast<Titlebar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__"))->setCaptionColour(col);
}


const Image* FrameWindow::getNSSizingCursorImage() const
{
    return d_nsSizingCursor;
}

const Image* FrameWindow::getEWSizingCursorImage() const
{
    return d_ewSizingCursor;
}

const Image* FrameWindow::getNWSESizingCursorImage() const
{
    return d_nwseSizingCursor;
}

const Image* FrameWindow::getNESWSizingCursorImage() const
{
    return d_neswSizingCursor;
}

void FrameWindow::setNSSizingCursorImage(const Image* image)
{
    d_nsSizingCursor = image;
}

void FrameWindow::setEWSizingCursorImage(const Image* image)
{
    d_ewSizingCursor = image;
}

void FrameWindow::setNWSESizingCursorImage(const Image* image)
{
    d_nwseSizingCursor = image;
}

void FrameWindow::setNESWSizingCursorImage(const Image* image)
{
    d_neswSizingCursor = image;
}

void FrameWindow::setNSSizingCursorImage(const String& imageset, const String& image)
{
    d_nsSizingCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
}

void FrameWindow::setEWSizingCursorImage(const String& imageset, const String& image)
{
    d_ewSizingCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
}

void FrameWindow::setNWSESizingCursorImage(const String& imageset, const String& image)
{
    d_nwseSizingCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
}

void FrameWindow::setNESWSizingCursorImage(const String& imageset, const String& image)
{
    d_neswSizingCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
}

} // End of  CEGUI namespace section

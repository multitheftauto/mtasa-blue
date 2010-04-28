/************************************************************************
	filename: 	CEGUIWindow.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the Window base class
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
#include "CEGUIWindow.h"
#include "CEGUIExceptions.h"
#include "CEGUIWindowManager.h"
#include "CEGUISystem.h"
#include "CEGUIFontManager.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIMouseCursor.h"
#include "elements/CEGUITooltip.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include <algorithm>
#include <cmath>
#include <stdio.h>

// Start of CEGUI namespace section
namespace CEGUI
{
const String Window::EventNamespace("Window");

/*************************************************************************
	Definitions for Window base class Properties
*************************************************************************/
WindowProperties::AbsoluteHeight	Window::d_absHeightProperty;
WindowProperties::AbsoluteMaxSize	Window::d_absMaxSizeProperty;
WindowProperties::AbsoluteMinSize	Window::d_absMinSizeProperty;
WindowProperties::AbsolutePosition	Window::d_absPositionProperty;
WindowProperties::AbsoluteRect		Window::d_absRectProperty;
WindowProperties::AbsoluteSize		Window::d_absSizeProperty;
WindowProperties::AbsoluteWidth		Window::d_absWidthProperty;
WindowProperties::AbsoluteXPosition	Window::d_absXPosProperty;
WindowProperties::AbsoluteYPosition	Window::d_absYPosProperty;
WindowProperties::Alpha				Window::d_alphaProperty;
WindowProperties::AlwaysOnTop		Window::d_alwaysOnTopProperty;
WindowProperties::ClippedByParent	Window::d_clippedByParentProperty;
WindowProperties::DestroyedByParent	Window::d_destroyedByParentProperty;
WindowProperties::Disabled			Window::d_disabledProperty;
WindowProperties::Font				Window::d_fontProperty;
WindowProperties::Height			Window::d_heightProperty;
WindowProperties::ID				Window::d_IDProperty;
WindowProperties::InheritsAlpha		Window::d_inheritsAlphaProperty;
WindowProperties::MetricsMode		Window::d_metricsModeProperty;
WindowProperties::MouseCursorImage	Window::d_mouseCursorProperty;
WindowProperties::Position			Window::d_positionProperty;
WindowProperties::Rect				Window::d_rectProperty;
WindowProperties::RelativeHeight	Window::d_relHeightProperty;
WindowProperties::RelativeMaxSize	Window::d_relMaxSizeProperty;
WindowProperties::RelativeMinSize	Window::d_relMinSizeProperty;
WindowProperties::RelativePosition	Window::d_relPositionProperty;
WindowProperties::RelativeRect		Window::d_relRectProperty;
WindowProperties::RelativeSize		Window::d_relSizeProperty;
WindowProperties::RelativeWidth		Window::d_relWidthProperty;
WindowProperties::RelativeXPosition	Window::d_relXPosProperty;
WindowProperties::RelativeYPosition	Window::d_relYPosProperty;
WindowProperties::RestoreOldCapture	Window::d_restoreOldCaptureProperty;
WindowProperties::Size				Window::d_sizeProperty;
WindowProperties::Text				Window::d_textProperty;
WindowProperties::Visible			Window::d_visibleProperty;
WindowProperties::Width				Window::d_widthProperty;
WindowProperties::XPosition			Window::d_xPosProperty;
WindowProperties::YPosition			Window::d_yPosProperty;
WindowProperties::ZOrderChangeEnabled	Window::d_zOrderChangeProperty;
WindowProperties::WantsMultiClickEvents Window::d_wantsMultiClicksProperty;
WindowProperties::MouseButtonDownAutoRepeat Window::d_autoRepeatProperty;
WindowProperties::AutoRepeatDelay   Window::d_autoRepeatDelayProperty;
WindowProperties::AutoRepeatRate    Window::d_autoRepeatRateProperty;
WindowProperties::DistributeCapturedInputs Window::d_distInputsProperty;
WindowProperties::CustomTooltipType Window::d_tooltipTypeProperty;
WindowProperties::Tooltip           Window::d_tooltipProperty;
WindowProperties::InheritsTooltipText Window::d_inheritsTooltipProperty;
WindowProperties::RiseOnClick       Window::d_riseOnClickProperty;
WindowProperties::VerticalAlignment   Window::d_vertAlignProperty;
WindowProperties::HorizontalAlignment Window::d_horzAlignProperty;
WindowProperties::UnifiedAreaRect	Window::d_unifiedAreaRectProperty;
WindowProperties::UnifiedPosition	Window::d_unifiedPositionProperty;
WindowProperties::UnifiedXPosition	Window::d_unifiedXPositionProperty;
WindowProperties::UnifiedYPosition	Window::d_unifiedYPositionProperty;
WindowProperties::UnifiedSize		Window::d_unifiedSizeProperty;
WindowProperties::UnifiedWidth		Window::d_unifiedWidthProperty;
WindowProperties::UnifiedHeight		Window::d_unifiedHeightProperty;
WindowProperties::UnifiedMinSize	Window::d_unifiedMinSizeProperty;
WindowProperties::UnifiedMaxSize	Window::d_unifiedMaxSizeProperty;
WindowProperties::MousePassThroughEnabled   Window::d_mousePassThroughEnabledProperty;

/*************************************************************************
	static data definitions
*************************************************************************/
Window*	Window::d_captureWindow		= NULL;


/*************************************************************************
	Event name constants
*************************************************************************/
const String Window::EventParentSized( (utf8*)"ParentSized" );
const String Window::EventSized( (utf8*)"Sized" );
const String Window::EventMoved( (utf8*)"Moved" );
const String Window::EventTextChanged( (utf8*)"TextChanged" );
const String Window::EventFontChanged( (utf8*)"FontChanged" );
const String Window::EventAlphaChanged( (utf8*)"AlphaChanged" );
const String Window::EventIDChanged( (utf8*)"IDChanged" );
const String Window::EventActivated( (utf8*)"Activated" );
const String Window::EventDeactivated( (utf8*)"Deactivated" );
const String Window::EventShown( (utf8*)"Shown" );
const String Window::EventHidden( (utf8*)"Hidden" );
const String Window::EventEnabled( (utf8*)"Enabled" );
const String Window::EventDisabled( (utf8*)"Disabled" );
const String Window::EventMetricsModeChanged( (utf8*)"MetricsChanged" );
const String Window::EventClippedByParentChanged( (utf8*)"ClippingChanged" );
const String Window::EventDestroyedByParentChanged( (utf8*)"DestroyedByParentChanged" );
const String Window::EventInheritsAlphaChanged( (utf8*)"InheritAlphaChanged" );
const String Window::EventAlwaysOnTopChanged( (utf8*)"AlwaysOnTopChanged" );
const String Window::EventInputCaptureGained( (utf8*)"CaptureGained" );
const String Window::EventInputCaptureLost( (utf8*)"CaptureLost" );
const String Window::EventRenderingStarted( (utf8*)"StartRender" );
const String Window::EventRenderingEnded( (utf8*)"EndRender" );
const String Window::EventChildAdded( (utf8*)"AddedChild" );
const String Window::EventChildRemoved( (utf8*)"RemovedChild" );
const String Window::EventDestructionStarted( (utf8*)"DestructStart" );
const String Window::EventZOrderChanged( (utf8*)"ZChanged" );
const String Window::EventDragDropItemEnters("DragDropItemEnters");
const String Window::EventDragDropItemLeaves("DragDropItemLeaves");
const String Window::EventDragDropItemDropped("DragDropItemDropped");
const String Window::EventVerticalAlignmentChanged("VerticalAlignmentChanged");
const String Window::EventHorizontalAlignmentChanged("HorizontalAlignmentChanged");
const String Window::EventMouseEnters( (utf8*)"MouseEnter" );
const String Window::EventMouseLeaves( (utf8*)"MouseLeave" );
const String Window::EventMouseMove( (utf8*)"MouseMove" );
const String Window::EventMouseWheel( (utf8*)"MouseWheel" );
const String Window::EventMouseButtonDown( (utf8*)"MouseButtonDown" );
const String Window::EventMouseButtonUp( (utf8*)"MouseButtonUp" );
const String Window::EventMouseClick( (utf8*)"MouseClick" );
const String Window::EventMouseDoubleClick( (utf8*)"MouseDoubleClick" );
const String Window::EventMouseTripleClick( (utf8*)"MouseTripleClick" );
const String Window::EventKeyDown( (utf8*)"KeyDown" );
const String Window::EventKeyUp( (utf8*)"KeyUp" );
const String Window::EventCharacterKey( (utf8*)"CharacterKey" );
const String Window::EventRedrawRequested( (utf8*)"RedrawRequested" );
	
	
/*************************************************************************
	Constructor
*************************************************************************/
Window::Window(const String& type, const String& name) :
	d_type(type),
	d_name(name)
{
	// basic set-up
	d_metricsMode	= Relative;
	d_parent		= NULL;
	d_font			= NULL;
	d_ID			= 0;
	d_alpha			= 1.0f;
	d_mouseCursor	= (const Image*)DefaultMouseCursor;
	d_userData		= NULL;
	d_needsRedraw   = true;

	// basic settings
	d_enabled			= true;
	d_visible			= true;
	d_active			= false;
	d_clippedByParent	= true;
	d_destroyedByParent	= true;
	d_alwaysOnTop		= false;
	d_inheritsAlpha		= true;
	d_restoreOldCapture	= false;
	d_zOrderingEnabled	= true;
    d_wantsMultiClicks  = true;
    d_distCapturedInputs = false;
    d_riseOnClick       = true;

    // initialise mouse button auto-repeat state
    d_repeatButton = NoButton;
    d_autoRepeat   = false;
    d_repeating    = false;
    d_repeatDelay  = 0.3f;
    d_repeatRate   = 0.06f;

    // Tooltip setup
    d_customTip = 0;
    d_weOwnTip = false;
    d_inheritsTipText = false;

    // add events
    addStandardEvents();

    // set initial min/max sizes.  These should normally be re-set in derived classes to something appropriate.
    d_minSize = UVector2(cegui_reldim(0), cegui_reldim(0));
    d_maxSize = UVector2(cegui_reldim(1), cegui_reldim(1));

    // set initial window area.
    d_area = URect(cegui_reldim(0), cegui_reldim(0), cegui_reldim(0), cegui_reldim(0));
    d_pixelSize = Size(0, 0);

    // set initial alignments
    d_horzAlign = HA_LEFT;
    d_vertAlign = VA_TOP;

    // event pass through
    d_mousePassThroughEnabled = false;

	// add properties
	addStandardProperties();
}

/*************************************************************************
	Destructor
*************************************************************************/
Window::~Window(void)
{
    // cleanup events actually happened earlier.
}


/*************************************************************************
	return type of this window.
*************************************************************************/
const String& Window::getType(void) const
{
    return d_falagardType.empty() ? d_type : d_falagardType;
}


/*************************************************************************
	return true if the Window is currently disabled	
*************************************************************************/
bool Window::isDisabled(bool localOnly) const
{
	bool parDisabled = ((d_parent == NULL) || localOnly) ? false : d_parent->isDisabled();

	return (!d_enabled) || parDisabled;
}


/*************************************************************************
	return true if the Window is currently visible.
*************************************************************************/
bool Window::isVisible(bool localOnly) const
{
	bool parVisible = ((d_parent == NULL) || localOnly) ? true : d_parent->isVisible();

	return d_visible && parVisible;
}


/*************************************************************************
	return true if this is the active Window
	(the window that receives inputs)	
*************************************************************************/
bool Window::isActive(void) const
{
	bool parActive = (d_parent == NULL) ? true : d_parent->isActive();

	return d_active && parActive;
}


/*************************************************************************
	returns whether a Window with the specified name is currently
	attached to this Window as a child.
*************************************************************************/
bool Window::isChild(const String& name) const
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getName() == name)
		{
			return true;
		}

	}

	return false;
}

/*************************************************************************
	returns whether at least one window with the given ID code is
	attached as a child.
*************************************************************************/
bool Window::isChild(uint ID) const
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getID() == ID)
		{
			return true;
		}

	}

	return false;
}


/*************************************************************************
	return true if the given Window is a child of this window.
*************************************************************************/
bool Window::isChild(const Window* window) const
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i] == window)
		{
			return true;
		}

	}

	return false;
}


/*************************************************************************
	return a pointer to the child window with the specified name.
*************************************************************************/
Window* Window::getChild(const String& name) const
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getName() == name)
		{
			return d_children[i];
		}

	}

	throw UnknownObjectException((utf8*)"Window::getChild - The Window object named '" + name +"' is not attached to Window '" + d_name + "'.");
}


/*************************************************************************
	return a pointer to the first attached child window with the
	specified ID.
*************************************************************************/
Window* Window::getChild(uint ID) const
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getID() == ID)
		{
			return d_children[i];
		}

	}

	// TODO: Update exception to include ID code
	char strbuf[16];
	sprintf(strbuf, "%X", ID);
	throw UnknownObjectException("Window::getChild - The Window with ID: '" + std::string(strbuf) + "' is not attached to Window '" + d_name + "'.");
}


/*************************************************************************
	return a pointer to the Window that currently has input focus
	starting with this Window.
*************************************************************************/
Window* Window::getActiveChild(void)
{
	return const_cast<Window*>(static_cast<const Window*>(this)->getActiveChild());
}


/*************************************************************************
	return a pointer to the Window that currently has input focus
	starting with this Window.
*************************************************************************/
const Window* Window::getActiveChild(void) const
{
	// are children can't be active if we are not
	if (!isActive())
	{
		return NULL;
	}

	uint pos = getChildCount();

	while (pos-- > 0)
	{
		// don't need full backward scan for activeness as we already know 'this' is active
		// NB: This uses the draw-ordered child list, as that should be quicker in most cases.
		if (d_drawList[pos]->d_active)
			return d_drawList[pos]->getActiveChild();
	}

	// no child was active, therefore we are the topmost active window
	return this;
}


/*************************************************************************
	return true if the specified Window is some ancestor of this Window
*************************************************************************/
bool Window::isAncestor(const String& name) const
{
	// if we have no ancestor then 'name' can't be ancestor
	if (d_parent == NULL)
	{
		return false;
	}

	// check our immediate parent
	if (d_parent->getName() == name)
	{
		return true;
	}

	// not out parent, check back up the family line
	return d_parent->isAncestor(name);
}


/*************************************************************************
	return true if any Window with the given ID is some ancestor of
	this Window.	
*************************************************************************/
bool Window::isAncestor(uint ID) const
{
	// return false if we have no ancestor
	if (d_parent == NULL)
	{
		return false;
	}

	// check our immediate parent
	if (d_parent->getID() == ID)
	{
		return true;
	}

	// not our parent, check back up the family line
	return d_parent->isAncestor(ID);
}


/*************************************************************************
	return true if the specified Window is some ancestor of this Window.	
*************************************************************************/
bool Window::isAncestor(const Window* window) const
{
	// if we have no parent, then return false
	if (d_parent == NULL)
	{
		return false;
	}

	// check our immediate parent
	if (d_parent == window)
	{
		return true;
	}

	// not our parent, check back up the family line
	return d_parent->isAncestor(window);
}


/*************************************************************************
	return the Font object active for the Window.
*************************************************************************/
const Font* Window::getFont(bool useDefault) const
{
	if (d_font == NULL)
	{
		return useDefault ? System::getSingleton().getDefaultFont() : 0;
	}

	return d_font;
}


/*************************************************************************
	return the effective alpha value that will be used when rendering
	this window, taking into account inheritance of parent window(s)
	alpha.
*************************************************************************/
float Window::getEffectiveAlpha(void) const
{
	if ((d_parent == NULL) || (!inheritsAlpha()))
	{
		return d_alpha;
	}

	return d_alpha * d_parent->getEffectiveAlpha();
}


/*************************************************************************
	return a Rect object that describes the Window area.	
*************************************************************************/
Rect Window::getRect(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeRect() : getAbsoluteRect();
}


/*************************************************************************
	return a Rect object describing the Window area in screen space.
*************************************************************************/
Rect Window::getPixelRect(void) const
{
	// clip to parent?
	if (isClippedByParent() && (d_parent != NULL))
	{
		return getUnclippedPixelRect().getIntersection(d_parent->getInnerRect());
	}
	// else, clip to screen
	else
	{
		return getUnclippedPixelRect().getIntersection(System::getSingleton().getRenderer()->getRect());
	}

}


/*************************************************************************
	return a Rect object describing the clipping area for this window.
*************************************************************************/
Rect Window::getInnerRect(void) const
{
	// clip to parent?
	if (isClippedByParent() && (d_parent != NULL))
	{
		return getUnclippedInnerRect().getIntersection(d_parent->getInnerRect());
	}
	// else, clip to screen
	else
	{
		return getUnclippedInnerRect().getIntersection(System::getSingleton().getRenderer()->getRect());
	}

}


/*************************************************************************
	return a Rect object describing the Window area unclipped, in
	screen space.	
*************************************************************************/
Rect Window::getUnclippedPixelRect(void) const
{
	if (getMetricsMode() == Relative)
	{
		return windowToScreen(Rect(0, 0, 1, 1));
	}
	else
	{
		return windowToScreen(Rect(0, 0, getAbsoluteWidth(), getAbsoluteHeight()));
	}
}


/*************************************************************************
	Return a Rect object that describes, unclipped, the inner rectangle
	for this window.  The inner rectangle is typically an area that
	excludes some frame or other rendering that should not be touched by
	subsequent rendering.
*************************************************************************/
Rect Window::getUnclippedInnerRect(void) const
{
	return getUnclippedPixelRect();
}


/*************************************************************************
	check if the given position would hit this window.	
*************************************************************************/
bool Window::isHit(const Point& position) const
{
	// cannot be hit if we are disabled.
	if (isDisabled())
	{
		return false;
	}

	Rect clipped_area(getPixelRect());

	if (clipped_area.getWidth() == 0)
	{
		return false;
	}

	return clipped_area.isPointInRect(position);
}

/*************************************************************************
	return the child Window that is 'hit' by the given position
*************************************************************************/
Window* Window::getChildAtPosition(const Point& position) const
{
	ChildList::const_reverse_iterator	child, end;

	end = d_drawList.rend();

	for (child = d_drawList.rbegin(); child != end; ++child)
	{
		if ((*child)->isVisible())
		{
			// recursively scan children of this child windows...
			Window* wnd = (*child)->getChildAtPosition(position);

			// return window pointer if we found a 'hit' down the chain somewhere
			if (wnd != NULL)
			{
				return wnd;
			}
			// none of our childs children were hit, 
			else
			{
				// see if this child is hit and return it's pointer if it is
				if ((*child)->isHit(position))
				{
					return (*child);
				}

			}

		}

	}

	// nothing hit
	return NULL;
}


/*************************************************************************
    return the child Window that is 'hit' by the given position and
    is allowed to handle mouse events
*************************************************************************/
Window* Window::getTargetChildAtPosition(const Point& position) const
{
    ChildList::const_reverse_iterator child;
    ChildList::const_reverse_iterator end = d_drawList.rend();

    for (child = d_drawList.rbegin(); child != end; ++child)
    {
        if ((*child)->isVisible())
        {
            // recursively scan children of this child windows...
            Window* wnd = (*child)->getTargetChildAtPosition(position);

            // return window pointer if we found a 'hit' down the chain somewhere
            if (wnd != NULL)
            {
                return wnd;
            }
            // none of our childs children were hit, 
            else
            {
                // see if this child is accepting mouse input and is hit,
                // and return it's pointer if it is
                if (!(*child)->isMousePassThroughEnabled() && (*child)->isHit(position))
                {
                    return (*child);
                }
            }
        }
    }

    // nothing hit
    return NULL;
}


/*************************************************************************
	return the current metrics mode employed by the Window
*************************************************************************/
MetricsMode Window::getMetricsMode(void) const
{
	if (d_metricsMode == Inherited)
	{
		return getInheritedMetricsMode();
	}

	return d_metricsMode;
}


/*************************************************************************
	return the x position of the window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
float Window::getXPosition(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeXPosition() : getAbsoluteXPosition();
}


/*************************************************************************
	return the y position of the window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
float Window::getYPosition(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeYPosition() : getAbsoluteYPosition();
}


/*************************************************************************
	return the position of the window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
Point Window::getPosition(void) const
{
    return (getMetricsMode() == Relative) ? getRelativePosition() : getAbsolutePosition();
}


/*************************************************************************
	return the width of the Window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
float Window::getWidth(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeWidth() : getAbsoluteWidth();
}


/*************************************************************************
	return the height of the Window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
float Window::getHeight(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeHeight() : getAbsoluteHeight();
}


/*************************************************************************
	return the size of the Window.  Interpretation of return value
	depends upon the metric type in use by this window.
*************************************************************************/
Size Window::getSize(void) const
{
    return (getMetricsMode() == Relative) ? getRelativeSize() : getAbsoluteSize();
}


/*************************************************************************
	true to have the Window appear on top of all other non always on top
	windows, or false to allow the window to be covered by other windows.
*************************************************************************/
void Window::setAlwaysOnTop(bool setting)
{
	// only react to an actual change
	if (isAlwaysOnTop() != setting)
	{
		d_alwaysOnTop = setting;

		// move us in front of sibling windows with the same 'always-on-top' setting as we have.
		if (d_parent != NULL)
		{
			Window* org_parent = d_parent;

			org_parent->removeChild_impl(this);
			org_parent->addChild_impl(this);

			onZChange_impl();
		}

		WindowEventArgs args(this);
		onAlwaysOnTopChanged(args);
	}

}


/*************************************************************************
	Set whether this window is enabled or disabled.  A disabled window
	normally can not be interacted with, and may have different rendering.
*************************************************************************/
void Window::setEnabled(bool setting)
{
	// only react if setting has changed
	if (d_enabled != setting)
	{
		d_enabled = setting;
        WindowEventArgs args(this);

        if (d_enabled)
        {
            // check to see if the window is actually enabled (which depends upon all ancestor windows being enabled)
            // we do this so that events we fire give an accurate indication of the state of a window.
            if ((d_parent && !d_parent->isDisabled()) || !d_parent)
                onEnabled(args);
        }
        else
        {
            onDisabled(args);
        }
    }
}


/*************************************************************************
	Set whether the Window is visible or hidden.
*************************************************************************/
void Window::setVisible(bool setting)
{
	// only react if setting has changed
	if (d_visible != setting)
	{
		d_visible = setting;
        WindowEventArgs args(this);
		d_visible ? onShown(args) : onHidden(args);
	}

}


/*************************************************************************
	Activate the Window giving it input focus and bringing it to the top
	of all non always-on-top Windows.
*************************************************************************/
void Window::activate(void)
{
	// force complete release of input capture.
	// NB: This is not done via releaseCapture() because that has
	// different behaviour depending on the restoreOldCapture setting.
	if ((d_captureWindow != NULL) && (d_captureWindow != this))
	{
		Window* tmpCapture = d_captureWindow;
		d_captureWindow = NULL;

		WindowEventArgs args(NULL);
		tmpCapture->onCaptureLost(args);
	}

	moveToFront();
}


/*************************************************************************
	Deactivate the window.  No further inputs will be received by the
	window until it is re-activated either programmatically or by the
	user interacting with the gui.
*************************************************************************/
void Window::deactivate(void)
{
	ActivationEventArgs args(this);
	args.otherWindow = NULL;
	onDeactivated(args);
}


/*************************************************************************
	Set whether this Window will be clipped by its parent window(s).
*************************************************************************/
void Window::setClippedByParent(bool setting)
{
	// only react if setting has changed
	if (d_clippedByParent != setting)
	{
		d_clippedByParent = setting;
        WindowEventArgs args(this);
		onClippingChanged(args);
	}

}


/*************************************************************************
	Set the current text string for the Window.
*************************************************************************/
void Window::setText(const String& text)
{
	d_text = text;
    WindowEventArgs args(this);
	onTextChanged(args);
}


/*************************************************************************
	Set the current width of the Window.  Interpretation of the input
	value is dependant upon the current metrics system set for the Window.
*************************************************************************/
void Window::setWidth(float width)
{
	setSize(Size(width, getHeight()));
}


/*************************************************************************
	Set the current height of the Window.  Interpretation of the input
	value is dependant upon the current metrics system set for the Window.
*************************************************************************/
void Window::setHeight(float height)
{
	setSize(Size(getWidth(), height));
}


/*************************************************************************
	Set the current size of the Window.  Interpretation of the input value
	is dependant upon the current metrics system set for the Window.
*************************************************************************/
void Window::setSize(const Size& size)
{
	setSize(getMetricsMode(), size);
}


/*************************************************************************
	Set the current 'x' position of the Window.  Interpretation of the
	input value is dependant upon the current metrics system set for the
	Window.
*************************************************************************/
void Window::setXPosition(float x)
{
	setPosition(Point(x, getYPosition()));
}


/*************************************************************************
	Set the current 'y' position of the Window.  Interpretation of the
	input value is dependant upon the current metrics system set for the
	Window.
*************************************************************************/
void Window::setYPosition(float y)
{
	setPosition(Point(getXPosition(), y));
}


/*************************************************************************
	Set the current position of the Window.  Interpretation of the input
	value is dependant upon the current metrics system set for the Window.
*************************************************************************/
void Window::setPosition(const Point& position)
{
	setPosition(getMetricsMode(), position);
}


/*************************************************************************
	Set the current area for the Window, this allows for setting of
	position and size at the same time.  Interpretation of the input
	value is dependant upon the current metrics system set for the Window.	
*************************************************************************/
void Window::setAreaRect(const Rect& area)
{
	setRect(getMetricsMode(), area);
}


/*************************************************************************
	Set the font used by this Window.
*************************************************************************/
void Window::setFont(const Font* font)
{
	d_font = font;
    WindowEventArgs args(this);
	onFontChanged(args);
}


/*************************************************************************
	Set the font used by this Window.
*************************************************************************/
void Window::setFont(const String& name)
{
	if (name.empty())
	{
		setFont(NULL);
	}
	else
	{
		setFont(FontManager::getSingleton().getFont(name));
	}
	
}


/*************************************************************************
	Add the named Window as a child of this Window.  If the Window is
	already attached to a Window, it is detached before being added to
	this Window.	
*************************************************************************/
void Window::addChildWindow(const String& name)
{
	addChildWindow(WindowManager::getSingleton().getWindow(name));
}


/*************************************************************************
	Add the specified Window as a child of this Window.  If the Window
	is already attached to a Window, it is detached before being added
	to this Window.	
*************************************************************************/
void Window::addChildWindow(Window* window)
{
	// dont add ourselves as a child
	if (window == this)
	{
		return;
	}
	addChild_impl(window);
    WindowEventArgs args(window);
	onChildAdded(args);
	window->onZChange_impl();
}


/*************************************************************************
	Remove the named Window from this windows child list.
*************************************************************************/
void Window::removeChildWindow(const String& name)
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getName() == name)
		{
			removeChildWindow(d_children[i]);
			return;
		}

	}

}


/*************************************************************************
	Remove the specified Window form this windows child list.
*************************************************************************/
void Window::removeChildWindow(Window* window)
{
	removeChild_impl(window);
    WindowEventArgs args(window);
	onChildRemoved(args);
	window->onZChange_impl();
}


/*************************************************************************
	Remove the first child Window with the specified ID.  If there is more
	than one attached Window objects with the specified ID, only the fist
	one encountered will be removed.	
*************************************************************************/
void Window::removeChildWindow(uint ID)
{
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->getID() == ID)
		{
			removeChildWindow(d_children[i]);
			return;
		}

	}

}


/*************************************************************************
	Move the Window to the top of the z order.
*************************************************************************/
void Window::moveToFront()
{
    moveToFront_impl(false);
}


/*************************************************************************
	Implementation of move to front
*************************************************************************/
void Window::moveToFront_impl(bool wasClicked)
{
	// if the window has no parent then we can have no siblings
	if (d_parent == NULL)
	{
		// perform initial activation if required.
		if (!isActive())
		{
            ActivationEventArgs args(this);
			args.otherWindow = NULL;
			onActivated(args);
		}

		return;
	}

	// bring parent window to front of it's siblings
    wasClicked ? d_parent->doRiseOnClick() : d_parent->moveToFront_impl(false);

    // get immediate child of parent that is currently active (if any)
    Window* activeWnd = getActiveSibling();

    // if a change in active window has occurred
    if (activeWnd != this)
    {
        // notify ourselves that we have become active
        ActivationEventArgs args(this);
        args.otherWindow = activeWnd;
        onActivated(args);

        // notify any previously active window that it is no longer active
        if (activeWnd)
        {
            args.window = activeWnd;
            args.otherWindow = this;
            args.handled = false;
            activeWnd->onDeactivated(args);
        }
    }

    // bring us to the front of our siblings
    if (d_zOrderingEnabled)
    {
        // remove us from our parent's draw list
        d_parent->removeWindowFromDrawList(*this);
        // re-attach ourselves to our parent's draw list which will move us in front of
        // sibling windows with the same 'always-on-top' setting as we have.
        d_parent->addWindowToDrawList(*this);
        // notify relevant windows about the z-order change.
        onZChange_impl();
    }
}


/*************************************************************************
	Move the Window to the bottom of the Z order.
*************************************************************************/
void Window::moveToBack()
{
	// if the window is active, de-activate it.
	if (isActive())
	{
        ActivationEventArgs args(this);
		args.otherWindow = NULL;
		onDeactivated(args);
	}

    // we only need to proceed if we have a parent (otherwise we have no siblings)
    if (d_parent)
    {
        if (d_zOrderingEnabled)
        {
            // remove us from our parent's draw list
            d_parent->removeWindowFromDrawList(*this);
            // re-attach ourselves to our parent's draw list which will move us in behind
            // sibling windows with the same 'always-on-top' setting as we have.
            d_parent->addWindowToDrawList(*this, true);
            // notify relevant windows about the z-order change.
            onZChange_impl();
        }

        d_parent->moveToBack();
    }
}


/*************************************************************************
	Captures input to this window
*************************************************************************/
bool Window::captureInput(void)
{
	// we can only capture if we are the active window
	if (!isActive()) {
		return false;
	}

	Window* current_capture = d_captureWindow;
	d_captureWindow = this;
    WindowEventArgs args(this);

	// inform any window which previously had capture that it doesn't anymore!
	if ((current_capture != NULL) && (current_capture != this) && (!d_restoreOldCapture)) {
		current_capture->onCaptureLost(args);
	}

	if (d_restoreOldCapture) {
		d_oldCapture = current_capture;
	}

	onCaptureGained(args);

	return true;
}


/*************************************************************************
	Releases input capture from this Window.  If this Window does not
	have inputs captured, nothing happens.
*************************************************************************/
void Window::releaseInput(void)
{
	// if we are not the window that has capture, do nothing
	if (!isCapturedByThis()) {
		return;
	}

	// restore old captured window if that mode is set
	if (d_restoreOldCapture) {
		d_captureWindow = d_oldCapture;

		// check for case when there was no previously captured window
		if (d_oldCapture != NULL) {
			d_oldCapture = NULL;
			d_captureWindow->moveToFront();
		}

	}
	else {
		d_captureWindow = NULL;
	}

    WindowEventArgs args(this);
	onCaptureLost(args);
}


/*************************************************************************
	Set whether this window will remember and restore the previous window
	that had inputs captured.
*************************************************************************/
void Window::setRestoreCapture(bool setting)
{
	d_restoreOldCapture = setting;

	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		d_children[i]->setRestoreCapture(setting);
	}

}


/*************************************************************************
	Set the current alpha value for this window.
*************************************************************************/
void Window::setAlpha(float alpha)
{
	d_alpha = alpha;
	WindowEventArgs args(this);
	onAlphaChanged(args);
}


/*************************************************************************
	Sets whether this Window will inherit alpha from its parent windows.
*************************************************************************/
void Window::setInheritsAlpha(bool setting)
{
	if (d_inheritsAlpha != setting)
	{
		// store old effective alpha so we can test if alpha value changes due to new setting.
		float oldAlpha = getEffectiveAlpha();

		// notify about the setting change.
		d_inheritsAlpha = setting;

		WindowEventArgs args(this);
		onInheritsAlphaChanged(args);

		// if effective alpha has changed fire notification about that too
		if (oldAlpha != getEffectiveAlpha())
		{
			args.handled = false;
			onAlphaChanged(args);
		}

	}

}


/*************************************************************************
	Signal the System object to redraw (at least) this Window on the next
	render cycle.

    lil_Toady: A bit hacky here, had to clone the const function,
               so it doesn't give linker errors in mta, there must
               be a cleaner way to do that
*************************************************************************/
void Window::requestRedraw(void)
{
    WindowEventArgs args(this);
    onRedrawRequested(args);

    /* lil_Toady: This was the original cegui code
    d_needsRedraw = true;
	System::getSingleton().signalRedraw();
    */
}


void Window::requestRedraw(void) const
{
    //d_needsRedraw = true;
	//System::getSingleton().signalRedraw();
}


void Window::forceRedraw(void)
{
    d_needsRedraw = true;
	System::getSingleton().signalRedraw();
}

/*************************************************************************
	Convert the given X co-ordinate from absolute to relative metrics.
*************************************************************************/
float Window::absoluteToRelativeX(float val) const
{
	return absoluteToRelativeX_impl(this, val);
}


/*************************************************************************
	Convert the given Y co-ordinate from absolute to relative metrics.
*************************************************************************/
float Window::absoluteToRelativeY(float val) const
{
	return absoluteToRelativeY_impl(this, val);
}


/*************************************************************************
	Convert the given position from absolute to relative metrics.
*************************************************************************/
Point Window::absoluteToRelative(const Point& pt) const
{
	return absoluteToRelative_impl(this, pt);
}


/*************************************************************************
	Convert the given size from absolute to relative metrics.
*************************************************************************/
Size Window::absoluteToRelative(const Size& sze) const
{
	return absoluteToRelative_impl(this, sze);
}


/*************************************************************************
	Convert the given area from absolute to relative metrics.
*************************************************************************/
Rect Window::absoluteToRelative(const Rect& rect) const
{
	return absoluteToRelative_impl(this, rect);
}


/*************************************************************************
	Convert the given X co-ordinate from relative to absolute metrics.
*************************************************************************/
float Window::relativeToAbsoluteX(float val) const
{
	return relativeToAbsoluteX_impl(this, val);
}


/*************************************************************************
	Convert the given Y co-ordinate from relative to absolute metrics.
*************************************************************************/
float Window::relativeToAbsoluteY(float val) const
{
	return relativeToAbsoluteY_impl(this, val);
}


/*************************************************************************
	Convert the given position from relative to absolute metrics.
*************************************************************************/
Point Window::relativeToAbsolute(const Point& pt) const
{
	return relativeToAbsolute_impl(this, pt);
}


/*************************************************************************
	Convert the given size from relative to absolute metrics.
*************************************************************************/
Size Window::relativeToAbsolute(const Size& sze) const
{
	return relativeToAbsolute_impl(this, sze);
}


/*************************************************************************
	Convert the given area from relative to absolute metrics.
*************************************************************************/
Rect Window::relativeToAbsolute(const Rect& rect) const
{
		return relativeToAbsolute_impl(this, rect);
}


/*************************************************************************
	Convert a window co-ordinate value, specified in whichever metrics
	mode is active, to a screen relative pixel co-ordinate.
*************************************************************************/
float Window::windowToScreenX(float x) const
{
    float baseX = 0;

    if (d_parent)
    {
        baseX = d_parent->windowToScreenX(baseX);
    }

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            baseX += getAbsoluteXPosition() + ((getParentWidth() - getAbsoluteWidth()) * 0.5f);
            break;
        case HA_RIGHT:
            baseX += getAbsoluteXPosition() + (getParentWidth() - getAbsoluteWidth());
            break;
        default:
            baseX += getAbsoluteXPosition();
            break;
    }

	if (getMetricsMode() == Relative)
	{
		return baseX + relativeToAbsoluteX(x);
	}
	else
	{
		return baseX + x;
	}

}


/*************************************************************************
	Convert a window co-ordinate value, specified in whichever metrics
	mode is active, to a screen relative pixel co-ordinate.
*************************************************************************/
float Window::windowToScreenY(float y) const
{
    float baseY = 0;

    if (d_parent)
    {
        baseY = d_parent->windowToScreenY(baseY);
    }

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            baseY += getAbsoluteYPosition() + ((getParentHeight() - getAbsoluteHeight()) * 0.5f);
            break;
        case VA_BOTTOM:
            baseY += getAbsoluteYPosition() + (getParentHeight() - getAbsoluteHeight());
            break;
        default:
            baseY += getAbsoluteYPosition();
            break;
    }

	if (getMetricsMode() == Relative)
	{
		return baseY + relativeToAbsoluteY(y);
	}
	else
	{
		return baseY + y;
	}

}


/*************************************************************************
	Convert a window co-ordinate position, specified in whichever metrics
	mode is active, to a screen relative pixel co-ordinate position.
*************************************************************************/
Point Window::windowToScreen(const Point& pt) const
{
	Point base(0, 0);
    
    if (d_parent)
    {
        base = d_parent->windowToScreen(base);
    }

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            base.d_x += getAbsoluteXPosition() + ((getParentWidth() - getAbsoluteWidth()) * 0.5f);
            break;
        case HA_RIGHT:
            base.d_x += getAbsoluteXPosition() + (getParentWidth() - getAbsoluteWidth());
            break;
        default:
            base.d_x += getAbsoluteXPosition();
            break;
    }

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            base.d_y += getAbsoluteYPosition() + ((getParentHeight() - getAbsoluteHeight()) * 0.5f);
            break;
        case VA_BOTTOM:
            base.d_y += getAbsoluteYPosition() + (getParentHeight() - getAbsoluteHeight());
            break;
        default:
            base.d_y += getAbsoluteYPosition();
            break;
    }

	if (getMetricsMode() == Relative)
	{
		return base + relativeToAbsolute(pt);
	}
	else
	{
		return base + pt;
	}

}


/*************************************************************************
	Convert a window size value, specified in whichever metrics mode is
	active, to a size in pixels.
*************************************************************************/
Size Window::windowToScreen(const Size& sze) const
{
	if (getMetricsMode() == Relative)
	{
		return Size(sze.d_width * getAbsoluteWidth(), sze.d_height * getAbsoluteHeight());
	}
	else
	{
		return sze;
	}

}


/*************************************************************************
	Convert a window area, specified in whichever metrics mode is
	active, to a screen area.
*************************************************************************/
Rect Window::windowToScreen(const Rect& rect) const
{
    Point base(0, 0);

    if (d_parent)
    {
        base = d_parent->windowToScreen(base);
    }

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            base.d_x += getAbsoluteXPosition() + ((getParentWidth() - getAbsoluteWidth()) * 0.5f);
            break;
        case HA_RIGHT:
            base.d_x += getAbsoluteXPosition() + (getParentWidth() - getAbsoluteWidth());
            break;
        default:
            base.d_x += getAbsoluteXPosition();
            break;
    }

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            base.d_y += getAbsoluteYPosition() + ((getParentHeight() - getAbsoluteHeight()) * 0.5f);
            break;
        case VA_BOTTOM:
            base.d_y += getAbsoluteYPosition() + (getParentHeight() - getAbsoluteHeight());
            break;
        default:
            base.d_y += getAbsoluteYPosition();
            break;
    }

	if (getMetricsMode() == Relative)
	{
		return relativeToAbsolute(rect).offset(base);
	}
	else
	{
		Rect tmp(rect);
		return tmp.offset(base);
	}

}


/*************************************************************************
	Convert a screen relative pixel co-ordinate value to a window
	co-ordinate value, specified in whichever metrics mode is active.
*************************************************************************/
float Window::screenToWindowX(float x) const
{
	x -= windowToScreenX(0);

	if (getMetricsMode() == Relative)
	{
		x /= getAbsoluteWidth();
	}

	return x;
}


/*************************************************************************
	Convert a screen relative pixel co-ordinate value to a window
	co-ordinate value, specified in whichever metrics mode is active.
*************************************************************************/
float Window::screenToWindowY(float y) const
{
	y -= windowToScreenY(0);

	if (getMetricsMode() == Relative)
	{
		y /= getAbsoluteHeight();
	}

	return y;
}


/*************************************************************************
	Convert a screen relative pixel position to a window co-ordinate
	position, specified in whichever metrics mode is active.
*************************************************************************/
Point Window::screenToWindow(const Point& pt) const
{
	Point tmp(pt);

	tmp.d_x -= windowToScreenX(0);
	tmp.d_y -= windowToScreenY(0);

	if (getMetricsMode() == Relative)
	{
		tmp.d_x /= getAbsoluteWidth();
		tmp.d_y /= getAbsoluteHeight();
	}

	return tmp;
}


/*************************************************************************
	Convert a screen size to a window based size
*************************************************************************/
Size Window::screenToWindow(const Size& sze) const
{
	Size tmp(sze);

	if (getMetricsMode() == Relative)
	{
		tmp.d_width		/= getAbsoluteWidth();
		tmp.d_height	/= getAbsoluteHeight();
	}

	return tmp;
}


/*************************************************************************
	Convert a screen area to a window area, specified in whichever
	metrics mode is active.
*************************************************************************/
Rect Window::screenToWindow(const Rect& rect) const
{
	Rect tmp(rect);

	tmp.d_left		-= windowToScreenX(0);
	tmp.d_top		-= windowToScreenY(0);
	tmp.d_right		-= windowToScreenX(0);
	tmp.d_bottom	-= windowToScreenY(0);

	if (getMetricsMode() == Relative)
	{
		tmp.d_left		/= getAbsoluteWidth();
		tmp.d_top		/= getAbsoluteHeight();
		tmp.d_right		/= getAbsoluteWidth();
		tmp.d_bottom	/= getAbsoluteHeight();
	}

	return tmp;
}


/*************************************************************************
    Convert the given X co-ordinate from unified to relative metrics.
*************************************************************************/
float Window::unifiedToRelativeX(const UDim& val) const
{
    return val.asRelative(d_pixelSize.d_width);
}

/*************************************************************************
    Convert the given Y co-ordinate from unified to relative metrics.
*************************************************************************/
float Window::unifiedToRelativeY(const UDim& val) const
{
    return val.asRelative(d_pixelSize.d_height);
}

/*************************************************************************
    Convert the given UVector2 value from unified to relative metrics.
*************************************************************************/
Vector2 Window::unifiedToRelative(const UVector2& val) const
{
    return val.asRelative(d_pixelSize);
}

/*************************************************************************
    Convert the given area from unfied to relative metrics.
*************************************************************************/
Rect Window::unifiedToRelative(const URect& val) const
{
    return val.asRelative(d_pixelSize);
}

/*************************************************************************
    Convert the given X co-ordinate from unified to absolute metrics.
*************************************************************************/
float Window::unifiedToAbsoluteX(const UDim& val) const
{
    return val.asAbsolute(d_pixelSize.d_width);
}

/*************************************************************************
    Convert the given Y co-ordinate from unified to absolute metrics.
*************************************************************************/
float Window::unifiedToAbsoluteY(const UDim& val) const
{
    return val.asAbsolute(d_pixelSize.d_height);
}

/*************************************************************************
    Convert the given UVector2 value from unified to absolute metrics.
*************************************************************************/
Vector2 Window::unifiedToAbsolute(const UVector2& val) const
{
    return val.asAbsolute(d_pixelSize);
}

/*************************************************************************
    Convert the given area from unfied to absolute metrics.
*************************************************************************/
Rect Window::unifiedToAbsolute(const URect& val) const
{
    return val.asAbsolute(d_pixelSize);
}

/*************************************************************************
    Convert a window co-ordinate value, specified as a UDim, to a screen
    relative pixel co-ordinate.
*************************************************************************/
float Window::windowToScreenX(const UDim& x) const
{
    float baseX = d_parent ?  d_parent->windowToScreenX(0) + getAbsoluteXPosition() : getAbsoluteXPosition();

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            baseX += (getParentWidth() - d_pixelSize.d_width) * 0.5f;
            break;
        case HA_RIGHT:
            baseX += getParentWidth() - d_pixelSize.d_width;
            break;
        default:
            break;
    }

    return baseX + x.asAbsolute(d_pixelSize.d_width);
}

/*************************************************************************
    Convert a window co-ordinate value, specified as a UDim, to a screen
    relative pixel co-ordinate.
*************************************************************************/
float Window::windowToScreenY(const UDim& y) const
{
    float baseY = d_parent ?  d_parent->windowToScreenY(0) + getAbsoluteYPosition() : getAbsoluteYPosition();

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            baseY += (getParentHeight() - d_pixelSize.d_height) * 0.5f;
            break;
        case VA_BOTTOM:
            baseY += getParentHeight() - d_pixelSize.d_height;
            break;
        default:
            break;
    }

    return baseY + y.asAbsolute(d_pixelSize.d_height);
}

/*************************************************************************
    Convert a window co-ordinate point, specified as a UVector2, to a
    screen relative pixel co-ordinate point.
*************************************************************************/
Vector2 Window::windowToScreen(const UVector2& vec) const
{
    Vector2 base = d_parent ? d_parent->windowToScreen(base) + getAbsolutePosition() : getAbsolutePosition();

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            base.d_x += (getParentWidth() - d_pixelSize.d_width) * 0.5f;
            break;
        case HA_RIGHT:
            base.d_x += getParentWidth() - d_pixelSize.d_width;
            break;
        default:
            break;
    }

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            base.d_y += (getParentHeight() - d_pixelSize.d_height) * 0.5f;
            break;
        case VA_BOTTOM:
            base.d_y += getParentHeight() - d_pixelSize.d_height;
            break;
        default:
            break;
    }

    return base + vec.asAbsolute(d_pixelSize);
}

/*************************************************************************
    Convert a window area, specified as a URect, to a screen area.
*************************************************************************/
Rect Window::windowToScreen(const URect& rect) const
{
    Vector2 base = d_parent ? d_parent->windowToScreen(base) + getAbsolutePosition() : getAbsolutePosition();

    switch(d_horzAlign)
    {
        case HA_CENTRE:
            base.d_x += (getParentWidth() - d_pixelSize.d_width) * 0.5f;
            break;
        case HA_RIGHT:
            base.d_x += getParentWidth() - d_pixelSize.d_width;
            break;
        default:
            break;
    }

    switch(d_vertAlign)
    {
        case VA_CENTRE:
            base.d_y += (getParentHeight() - d_pixelSize.d_height) * 0.5f;
            break;
        case VA_BOTTOM:
            base.d_y += getParentHeight() - d_pixelSize.d_height;
            break;
        default:
            break;
    }

    Rect tmp(rect.asAbsolute(d_pixelSize));
    return tmp.offset(base);
}

/*************************************************************************
    Convert a screen relative UDim co-ordinate value to a window
    co-ordinate value, specified in whichever metrics mode is active.
*************************************************************************/
float Window::screenToWindowX(const UDim& x) const
{
    return screenToWindowX(x.asAbsolute(System::getSingleton().getRenderer()->getWidth()));
}

/*************************************************************************
    Convert a screen relative UDim co-ordinate value to a window
    co-ordinate value, specified in whichever metrics mode is active.
*************************************************************************/
float Window::screenToWindowY(const UDim& y) const
{
    return screenToWindowY(y.asAbsolute(System::getSingleton().getRenderer()->getHeight()));
}

/*************************************************************************
    Convert a screen relative UVector2 point to a window co-ordinate
    point, specified in whichever metrics mode is active.
*************************************************************************/
Vector2 Window::screenToWindow(const UVector2& vec) const
{
    return screenToWindow(vec.asAbsolute(System::getSingleton().getRenderer()->getSize()));
}

/*************************************************************************
    Convert a URect screen area to a window area, specified in whichever
    metrics mode is active.
*************************************************************************/
Rect Window::screenToWindow(const URect& rect) const
{
    return screenToWindow(rect.asAbsolute(System::getSingleton().getRenderer()->getSize()));
}

/*************************************************************************
	Causes the Window object to render itself.
*************************************************************************/
void Window::render(void)
{
	// don't do anything if window is not visible
	if (!isVisible()) {
		return;
	}

	// signal rendering started
	WindowEventArgs args(this);
	onRenderingStarted(args);

	// perform drawing for 'this' Window
	Renderer* renderer = System::getSingleton().getRenderer();
	drawSelf(renderer->getCurrentZ());
	renderer->advanceZValue();

	// render any child windows
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		d_drawList[i]->render();
	}

	// signal rendering ended
	onRenderingEnded(args);
}


/*************************************************************************
    Perform the actual rendering for this Window.
*************************************************************************/
void Window::drawSelf(float z)
{
    if (d_needsRedraw)
    {
        // dispose of already cached imagery.
        d_renderCache.clearCachedImagery();
        // get derived class to re-populate cache.
        populateRenderCache();
        // mark ourselves as no longer needed a redraw.
        d_needsRedraw = false;
    }

    // if render cache contains imagery.
    if (d_renderCache.hasCachedImagery())
    {
		Point absPos(getUnclippedPixelRect().getPosition());
        // calculate clipping area for this window
        Rect clipper(getPixelRect());
        // If window is not totally clipped.
        if (clipper.getWidth())
        {
            // send cached imagery to the renderer.
            d_renderCache.render(absPos, z, clipper);
        }
    }
}


/*************************************************************************
	Set the parent window for this window object.
*************************************************************************/
void Window::setParent(Window* parent)
{
	d_parent = parent;
}


/*************************************************************************
	Return the pixel Width of the parent element.
	This always returns a valid number.
*************************************************************************/
float Window::getParentWidth(void) const
{
	if (d_parent == NULL)
	{
		return System::getSingleton().getRenderer()->getWidth();
	}

	return d_parent->getAbsoluteWidth();
}


/*************************************************************************
	Return the pixel Height of the parent element.
	This always returns a valid number.
*************************************************************************/
float Window::getParentHeight(void) const
{
	if (d_parent == NULL)
	{
		return System::getSingleton().getRenderer()->getHeight();
	}

	return d_parent->getAbsoluteHeight();
}


/*************************************************************************
	Return the pixel size of the parent element.
	This always returns a valid object.
*************************************************************************/
Size Window::getParentSize(void) const
{
	return getWindowSize_impl(d_parent);
}


/*************************************************************************
	Add standard Window events
*************************************************************************/
void Window::addStandardEvents( bool bCommon )
{
    if ( bCommon == false )
    {
        // window events
        addEvent(EventFontChanged);
        addEvent(EventAlphaChanged);
        addEvent(EventIDChanged);
        addEvent(EventActivated);
        addEvent(EventDeactivated);
        addEvent(EventShown);
        addEvent(EventHidden);
        addEvent(EventEnabled);
        addEvent(EventDisabled);
        addEvent(EventMetricsModeChanged);
        addEvent(EventClippedByParentChanged);
        addEvent(EventDestroyedByParentChanged);
        addEvent(EventInheritsAlphaChanged);
        addEvent(EventAlwaysOnTopChanged);
        addEvent(EventInputCaptureGained);
        addEvent(EventInputCaptureLost);
        addEvent(EventRenderingStarted);
        addEvent(EventRenderingEnded);
        addEvent(EventChildAdded);
        addEvent(EventChildRemoved);
        addEvent(EventDestructionStarted);
        addEvent(EventZOrderChanged);
        addEvent(EventParentSized);
        addEvent(EventDragDropItemEnters);
        addEvent(EventDragDropItemLeaves);
        addEvent(EventDragDropItemDropped);
        addEvent(EventVerticalAlignmentChanged);
        addEvent(EventHorizontalAlignmentChanged);

        // general input handling
        addEvent(EventMouseMove);
        addEvent(EventMouseWheel);
        addEvent(EventMouseButtonUp);
        addEvent(EventMouseDoubleClick);
        addEvent(EventMouseTripleClick);
        addEvent(EventKeyUp);
        addEvent(EventCharacterKey);

        // MTA event
        addEvent(EventRedrawRequested);
    }
    else
    {
        // window events
        addEvent(EventSized);
        addEvent(EventMoved);
        addEvent(EventTextChanged);

        // general input handling
        addEvent(EventMouseEnters);
        addEvent(EventMouseLeaves);
        addEvent(EventMouseButtonDown);
        addEvent(EventMouseClick);
        addEvent(EventKeyDown);
    }
}


/*************************************************************************
	Cleanup child windows
*************************************************************************/
void Window::cleanupChildren(void)
{
	while(getChildCount() != 0)
	{
		Window* wnd = d_children[0];

		// always remove child
		removeChildWindow(wnd);

		// destroy child if that is required
		if (wnd->isDestroyedByParent())
		{
			WindowManager::getSingleton().destroyWindow(wnd);
		}

	}

}


/*************************************************************************
	Add given window to child list at an appropriate position
*************************************************************************/
void Window::addChild_impl(Window* wnd)
{
	// if window is already attached, detach it first (will fire normal events)
	if (wnd->getParent() != NULL)
		wnd->getParent()->removeChildWindow(wnd);
    addWindowToDrawList(*wnd);

    // add window to child list
    d_children.push_back(wnd);

	// set the parent window
	wnd->setParent(this);

	// Force and update for the area Rects for 'wnd' so they're correct for it's new parent.
    WindowEventArgs args(this);
	wnd->onParentSized(args);
}


/*************************************************************************
	Remove given window from child list
*************************************************************************/
void Window::removeChild_impl(Window* wnd)
{
    // remove from draw list
    removeWindowFromDrawList(*wnd);

    // if window has children
    if (!d_children.empty())
    {
        // find this window in the child list
        ChildList::iterator	position = std::find(d_children.begin(), d_children.end(), wnd);

        // if the window was found in the child list
        if (position != d_children.end())
        {
            // remove window from child list
            d_children.erase(position);
            // reset windows parent so it's no longer this window.
            wnd->setParent(0);
        }
    }
}


/*************************************************************************
	Notify 'this' and all siblings of a ZOrder change event
*************************************************************************/
void Window::onZChange_impl(void)
{
	if (d_parent == NULL)
	{
        WindowEventArgs args(this);
		onZChanged(args);
	}
	else
	{
		uint child_count = d_parent->getChildCount();

		for (uint i = 0; i < child_count; ++i)
		{
            WindowEventArgs args(d_parent->d_children[i]);
			d_parent->d_children[i]->onZChanged(args);
		}

	}

}


/*************************************************************************
	
*************************************************************************/
Rect Window::absoluteToRelative_impl(const Window* window, const Rect& rect) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	Rect tmp;

	if (sz.d_width)
	{
		tmp.d_left	= PixelAligned(rect.d_left) / sz.d_width;
		tmp.d_right = PixelAligned(rect.d_right) / sz.d_width;
	}
	else
	{
		tmp.d_left = tmp.d_right = 0;
	}

	if (sz.d_height)
	{
		tmp.d_top		= PixelAligned(rect.d_top) / sz.d_height;
		tmp.d_bottom	= PixelAligned(rect.d_bottom) / sz.d_height;
	}
	else
	{
		tmp.d_top = tmp.d_bottom= 0;
	}

	return tmp;
}


/*************************************************************************

*************************************************************************/
Size Window::absoluteToRelative_impl(const Window* window, const Size& sz) const
{
	// get size object for whatever we are using as a base for the conversion
	Size wndsz = getWindowSize_impl(window);

	Size tmp;

	if (wndsz.d_width)
	{
		tmp.d_width = PixelAligned(sz.d_width) / wndsz.d_width;
	}
	else
	{
		tmp.d_width = 0;
	}

	if (wndsz.d_height)
	{
		tmp.d_height = PixelAligned(sz.d_height) / wndsz.d_height;
	}
	else
	{
		tmp.d_height = 0;
	}

	return tmp;
}


/*************************************************************************

*************************************************************************/
Point Window::absoluteToRelative_impl(const Window* window, const Point& pt) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	Point tmp;

	if (sz.d_width)
	{
		tmp.d_x = PixelAligned(pt.d_x) / sz.d_width;
	}
	else
	{
		tmp.d_x = 0;
	}

	if (sz.d_height)
	{
		tmp.d_y = PixelAligned(pt.d_y) / sz.d_height;
	}
	else
	{
		tmp.d_y = 0;
	}

	return tmp;
}


/*************************************************************************

*************************************************************************/
float Window::absoluteToRelativeX_impl(const Window* window, float x) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	if (sz.d_width)
	{
		return PixelAligned(x) / sz.d_width;
	}
	else
	{
		return 0;
	}
}


/*************************************************************************

*************************************************************************/
float Window::absoluteToRelativeY_impl(const Window* window, float y) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	if (sz.d_height)
	{
		return PixelAligned(y) / sz.d_height;
	}
	else
	{
		return 0;
	}
}


/*************************************************************************

*************************************************************************/
Rect Window::relativeToAbsolute_impl(const Window* window, const Rect& rect) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	return Rect(
		PixelAligned(rect.d_left * sz.d_width),
		PixelAligned(rect.d_top * sz.d_height),
		PixelAligned(rect.d_right * sz.d_width),
		PixelAligned(rect.d_bottom * sz.d_height)
		);
}


/*************************************************************************

*************************************************************************/
Size Window::relativeToAbsolute_impl(const Window* window, const Size& sz) const
{
	// get size object for whatever we are using as a base for the conversion
	Size wndsz = getWindowSize_impl(window);

	return Size(
		PixelAligned(sz.d_width * wndsz.d_width),
		PixelAligned(sz.d_height * wndsz.d_height)
		);
}


/*************************************************************************

*************************************************************************/
Point Window::relativeToAbsolute_impl(const Window* window, const Point& pt) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	return Point(
		PixelAligned(pt.d_x * sz.d_width),
		PixelAligned(pt.d_y * sz.d_height)
		);
}


/*************************************************************************

*************************************************************************/
float Window::relativeToAbsoluteX_impl(const Window* window, float x) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	return PixelAligned(x * sz.d_width);
}


/*************************************************************************

*************************************************************************/
float Window::relativeToAbsoluteY_impl(const Window* window, float y) const
{
	// get size object for whatever we are using as a base for the conversion
	Size sz = getWindowSize_impl(window);

	return PixelAligned(y * sz.d_height);
}


/*************************************************************************
	Return size of window.  If window is NULL return size of display.
*************************************************************************/
Size Window::getWindowSize_impl(const Window* window) const
{
	if (window == NULL)
	{
		return System::getSingleton().getRenderer()->getSize();
	}
	else
	{
        return window->getAbsoluteSize();
	}

}


/*************************************************************************
	Return the current maximum size for this window.
*************************************************************************/
Size Window::getMaximumSize(void) const
{
	if (getMetricsMode() == Absolute)
	{
        return d_maxSize.asAbsolute(System::getSingleton().getRenderer()->getSize()).asSize();
	}
	else
	{
        return d_maxSize.asRelative(System::getSingleton().getRenderer()->getSize()).asSize();
    }

}


/*************************************************************************
	Return the current minimum size for this window.
*************************************************************************/
Size Window::getMinimumSize(void) const
{
	if (getMetricsMode() == Absolute)
    {
        return d_minSize.asAbsolute(System::getSingleton().getRenderer()->getSize()).asSize();
    }
    else
    {
        return d_minSize.asRelative(System::getSingleton().getRenderer()->getSize()).asSize();
    }

}


/*************************************************************************
	Set the minimum size for this window.
*************************************************************************/
void Window::setMinimumSize(const Size& sz)
{
    UVector2 usz;
    
    if (getMetricsMode() == Absolute)
    {
        usz.d_x = cegui_absdim(PixelAligned(sz.d_width));
        usz.d_y = cegui_absdim(PixelAligned(sz.d_height));
    }
    else
    {
        usz.d_x = cegui_reldim(sz.d_width);
        usz.d_y = cegui_reldim(sz.d_height);
    }

    setWindowMinSize(usz);
}


/*************************************************************************
	Set the maximum size for this window.
*************************************************************************/
void Window::setMaximumSize(const Size& sz)
{
    UVector2 usz;

    if (getMetricsMode() == Absolute)
    {
        usz.d_x = cegui_absdim(PixelAligned(sz.d_width));
        usz.d_y = cegui_absdim(PixelAligned(sz.d_height));
    }
    else
    {
        usz.d_x = cegui_reldim(sz.d_width);
        usz.d_y = cegui_reldim(sz.d_height);
    }

    setWindowMaxSize(usz);
}


/*************************************************************************
	Return a pointer to the mouse cursor image to use when the mouse is
	within this window.
*************************************************************************/
const Image* Window::getMouseCursor(bool useDefault) const
{
	if (d_mouseCursor != (const Image*)DefaultMouseCursor)
	{
		return d_mouseCursor;
	}
	else
	{
		return useDefault ? System::getSingleton().getDefaultMouseCursor() : 0;
	}

}


/*************************************************************************
	Set the mouse cursor image to be used when the mouse enters this
	window.	
*************************************************************************/
void Window::setMouseCursor(const String& imageset, const String& image_name)
{
	d_mouseCursor = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image_name);
}


/*************************************************************************
	Set the current ID for the Window.	
*************************************************************************/
void Window::setID(uint ID)
{
	if (d_ID != ID)
	{
		d_ID = ID;

		WindowEventArgs args(this);
		onIDChanged(args);
	}

}


/*************************************************************************
	set the current metrics mode employed by the Window	
*************************************************************************/
void Window::setMetricsMode(MetricsMode	mode)
{
	if (d_metricsMode != mode)
	{
		MetricsMode oldMode = d_metricsMode;
		d_metricsMode = mode;

		// only ever trigger the event if the mode is actually changed.
		if ((d_metricsMode != Inherited) || (oldMode != getMetricsMode()))
		{
			WindowEventArgs args(this);
			onMetricsChanged(args);
		}

	}

}


/*************************************************************************
	Set whether or not this Window will automatically be destroyed when
	its parent Window is destroyed.	
*************************************************************************/
void Window::setDestroyedByParent(bool setting)
{
	if (d_destroyedByParent != setting)
	{
		d_destroyedByParent = setting;

		WindowEventArgs args(this);
		onParentDestroyChanged(args);
	}

}


/*************************************************************************
	Return the inherited metrics mode.  This is either the metrics mode
	of our parent, or Relative if we have no parent.	
*************************************************************************/
MetricsMode Window::getInheritedMetricsMode(void) const
{
	return (d_parent == NULL) ? Relative : d_parent->getMetricsMode();
}


/*************************************************************************
	return the x position of the window using the specified metrics system.	
*************************************************************************/
float Window::getXPosition(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeXPosition() : getAbsoluteXPosition();
}


/*************************************************************************
	return the y position of the window using the specified metrics system.	
*************************************************************************/
float Window::getYPosition(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeYPosition() : getAbsoluteYPosition();
}


/*************************************************************************
	return the position of the window using the specified metrics system.	
*************************************************************************/
Point Window::getPosition(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativePosition() : getAbsolutePosition();
}


/*************************************************************************
	return the width of the Window using the specified metrics system.	
*************************************************************************/
float Window::getWidth(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeWidth() : getAbsoluteWidth();
}


/*************************************************************************
	return the height of the Window using the specified metrics system.	
*************************************************************************/
float Window::getHeight(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeHeight() : getAbsoluteHeight();
}


/*************************************************************************
	return the size of the Window using the specified metrics system.	
*************************************************************************/
Size Window::getSize(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeSize() : getAbsoluteSize();
}


/*************************************************************************
	return a Rect object that describes the Window area using the
	specified metrics system.
*************************************************************************/
Rect Window::getRect(MetricsMode mode) const
{
	// get proper mode to use for inherited.
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    return (mode == Relative) ? getRelativeRect() : getAbsoluteRect();
}


/*************************************************************************
	set the x position of the window using the specified metrics system.	
*************************************************************************/
void Window::setXPosition(MetricsMode mode, float x)
{
	setPosition(mode, Point(x, getYPosition(mode)));
}


/*************************************************************************
	set the y position of the window using the specified metrics system.	
*************************************************************************/
void Window::setYPosition(MetricsMode mode, float y)
{
	setPosition(mode, Point(getXPosition(mode), y));
}


/*************************************************************************
	set the position of the window using the specified metrics system.	
*************************************************************************/
void Window::setPosition(MetricsMode mode, const Point& position)
{
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

	if (mode == Relative)
	{
        setWindowPosition(UVector2(cegui_reldim(position.d_x), cegui_reldim(position.d_y)));
	}
	else
	{
        setWindowPosition(UVector2(cegui_absdim(PixelAligned(position.d_x)), cegui_absdim(PixelAligned(position.d_y))));
	}
}


/*************************************************************************
	set the width of the Window using the specified metrics system.	
*************************************************************************/
void Window::setWidth(MetricsMode mode, float width)
{
	setSize(mode, Size(width, getHeight(mode)));
}


/*************************************************************************
	set the height of the Window using the specified metrics system.	
*************************************************************************/
void Window::setHeight(MetricsMode mode, float height)
{
	setSize(mode, Size(getWidth(mode), height));
}


/*************************************************************************
	set the size of the Window using the specified metrics system.	
*************************************************************************/
void Window::setSize(MetricsMode mode, const Size& size)
{
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    UVector2 usz(((mode == Relative) ? UVector2(cegui_reldim(size.d_width), cegui_reldim(size.d_height)) : UVector2(cegui_absdim(PixelAligned(size.d_width)), cegui_absdim(PixelAligned(size.d_height)))));

    setWindowSize(usz);
}


/*************************************************************************
	set the Rect that describes the Window area using the specified
	metrics system.	
*************************************************************************/
void Window::setRect(MetricsMode mode, const Rect& area)
{
	if (mode == Inherited)
	{
		mode = getInheritedMetricsMode();
	}

    URect uarea;
    
	if (mode == Relative)
	{
        uarea = URect(
                cegui_reldim(area.d_left),
                cegui_reldim(area.d_top),
                cegui_reldim(area.d_right),
                cegui_reldim(area.d_bottom)
                      );
	}
	else
	{
        uarea = URect(
                cegui_absdim(PixelAligned(area.d_left)),
                cegui_absdim(PixelAligned(area.d_top)),
                cegui_absdim(PixelAligned(area.d_right)),
                cegui_absdim(PixelAligned(area.d_bottom))
                      );
    }

    setWindowArea(uarea);
}


/*************************************************************************
    Helper method to fire off a mouse button down event.
*************************************************************************/
void Window::generateAutoRepeatEvent(MouseButton button)
{
    MouseEventArgs ma(this);
    ma.position = MouseCursor::getSingleton().getPosition();
    ma.moveDelta = Vector2(0.0f, 0.0f);
    ma.button = button;
    ma.sysKeys = System::getSingleton().getSystemKeys();
    ma.wheelChange = 0;
    onMouseButtonDown(ma);
}


/*************************************************************************
	Add standard CEGUI::Window properties.
*************************************************************************/
void Window::addStandardProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_absHeightProperty);
        addProperty(&d_absMaxSizeProperty);
        addProperty(&d_absMinSizeProperty);
        addProperty(&d_absPositionProperty);
        addProperty(&d_absRectProperty);
        addProperty(&d_absSizeProperty);
        addProperty(&d_absWidthProperty);
        addProperty(&d_absXPosProperty);
        addProperty(&d_absYPosProperty);
        addProperty(&d_alphaProperty);
        addProperty(&d_clippedByParentProperty);
        addProperty(&d_destroyedByParentProperty);
        addProperty(&d_fontProperty);
        addProperty(&d_heightProperty);
        addProperty(&d_IDProperty);
        addProperty(&d_inheritsAlphaProperty);
        addProperty(&d_metricsModeProperty);
        addProperty(&d_mouseCursorProperty);
        addProperty(&d_positionProperty);
        addProperty(&d_rectProperty);
        addProperty(&d_relHeightProperty);
        addProperty(&d_relMaxSizeProperty);
        addProperty(&d_relMinSizeProperty);
        addProperty(&d_relPositionProperty);
        addProperty(&d_relRectProperty);
        addProperty(&d_relSizeProperty);
        addProperty(&d_relWidthProperty);
        addProperty(&d_relXPosProperty);
        addProperty(&d_relYPosProperty);
        addProperty(&d_restoreOldCaptureProperty);
        addProperty(&d_sizeProperty);
        addProperty(&d_textProperty);
        addProperty(&d_visibleProperty);
        addProperty(&d_widthProperty);
        addProperty(&d_xPosProperty);
        addProperty(&d_yPosProperty);
        addProperty(&d_zOrderChangeProperty);
        addProperty(&d_autoRepeatProperty);
        addProperty(&d_autoRepeatDelayProperty);
        addProperty(&d_autoRepeatRateProperty);
        addProperty(&d_distInputsProperty);
        addProperty(&d_tooltipTypeProperty);
        addProperty(&d_tooltipProperty);
        addProperty(&d_inheritsTooltipProperty);
        addProperty(&d_riseOnClickProperty);
        addProperty(&d_vertAlignProperty);
        addProperty(&d_horzAlignProperty);
        addProperty(&d_unifiedAreaRectProperty);
        addProperty(&d_unifiedPositionProperty);
        addProperty(&d_unifiedXPositionProperty);
        addProperty(&d_unifiedYPositionProperty);
        addProperty(&d_unifiedSizeProperty);
        addProperty(&d_unifiedWidthProperty);
        addProperty(&d_unifiedHeightProperty);
        addProperty(&d_unifiedMinSizeProperty);
        addProperty(&d_unifiedMaxSizeProperty);
        addProperty(&d_mousePassThroughEnabledProperty);
    }
    else
    {
        addProperty(&d_alwaysOnTopProperty);
        addProperty(&d_disabledProperty);
        addProperty(&d_wantsMultiClicksProperty);
    }
}


/*************************************************************************
	Return whether z-order changes are enabled.
*************************************************************************/
bool Window::isZOrderingEnabled(void) const
{
	return d_zOrderingEnabled;
}


/*************************************************************************
	Set whether z-order changes are enabled.
*************************************************************************/
void Window::setZOrderingEnabled(bool setting)
{
	if (d_zOrderingEnabled != setting)
	{
		d_zOrderingEnabled = setting;
	}

}


/*************************************************************************
    Return whether this window will receive multi-click events or
    multiple 'down' events instead.
*************************************************************************/
bool Window::wantsMultiClickEvents(void) const
{
    return d_wantsMultiClicks;
}


/*************************************************************************
    Set whether this window will receive multi-click events or
    multiple 'down' events instead.	
*************************************************************************/
void Window::setWantsMultiClickEvents(bool setting)
{
    if (d_wantsMultiClicks != setting)
    {
        d_wantsMultiClicks = setting;

        // TODO: Maybe add a 'setting changed' event for this?
    }

}


/*************************************************************************
    Return whether mouse button down event autorepeat is enabled for
    this window.
*************************************************************************/
bool Window::isMouseAutoRepeatEnabled(void) const
{
    return d_autoRepeat;
}


/*************************************************************************
    Return the current auto-repeat delay setting for this window.
*************************************************************************/
float Window::getAutoRepeatDelay(void) const
{
    return d_repeatDelay;
}

    
/*************************************************************************
    Return the current auto-repeat rate setting for this window.
*************************************************************************/
float Window::getAutoRepeatRate(void) const
{
    return d_repeatRate;
}


/*************************************************************************
    Set whether mouse button down event autorepeat is enabled for this
    window.
*************************************************************************/
void Window::setMouseAutoRepeatEnabled(bool setting)
{
    if (d_autoRepeat != setting)
    {
        d_autoRepeat = setting;
        d_repeatButton = NoButton;
 
        // TODO: Maybe add a 'setting changed' event for this?
    }

}


/*************************************************************************
    Set the current auto-repeat delay setting for this window.
*************************************************************************/
void Window::setAutoRepeatDelay(float delay)
{
    if (d_repeatDelay != delay)
    {
        d_repeatDelay = delay;

        // TODO: Maybe add a 'setting changed' event for this?
    }

}

    
/*************************************************************************
    Set the current auto-repeat rate setting for this window.
*************************************************************************/
void Window::setAutoRepeatRate(float rate)
{
    if (d_repeatRate != rate)
    {
        d_repeatRate = rate;

        // TODO: Maybe add a 'setting changed' event for this?
    }

}


/*************************************************************************
	Cause window to update itself and any attached children
*************************************************************************/
void Window::update(float elapsed)
{
	// perform update for 'this' Window
	updateSelf(elapsed);

	// update child windows
	uint child_count = getChildCount();

	for (uint i = 0; i < child_count; ++i)
	{
		d_children[i]->update(elapsed);
	}

}


/*************************************************************************
    Perform actual update processing for this Window.
*************************************************************************/
void Window::updateSelf(float elapsed)
{
    // Mouse button autorepeat processing.
    if (d_autoRepeat && d_repeatButton != NoButton)
    {
        d_repeatElapsed += elapsed;

        if (d_repeating)
        {
            if (d_repeatElapsed > d_repeatRate)
            {
                d_repeatElapsed -= d_repeatRate;
                // trigger the repeated event
                generateAutoRepeatEvent(d_repeatButton);
            }
        }
        else
        {
            if (d_repeatElapsed > d_repeatDelay)
            {
                d_repeatElapsed = 0;
                d_repeating = true;
                // trigger the repeated event
                generateAutoRepeatEvent(d_repeatButton);
            }
        }
    }
}

bool Window::distributesCapturedInputs(void) const
{
    return d_distCapturedInputs;
}

void Window::setDistributesCapturedInputs(bool setting)
{
    if (d_distCapturedInputs != setting)
    {
        d_distCapturedInputs = setting;

        // TODO: Maybe add a 'setting changed' event for this?
    }
}

void Window::notifyDragDropItemEnters(DragContainer* item)
{
    if (item)
    {
        DragDropEventArgs args(this);
        args.dragDropItem = item;
        onDragDropItemEnters(args);
    }
}

void Window::notifyDragDropItemLeaves(DragContainer* item)
{
    if (item)
    {
        DragDropEventArgs args(this);
        args.dragDropItem = item;
        onDragDropItemLeaves(args);
    }
}

void Window::notifyDragDropItemDropped(DragContainer* item)
{
    if (item)
    {
        DragDropEventArgs args(this);
        args.dragDropItem = item;
        onDragDropItemDropped(args);
    }
}

void Window::destroy(void)
{
    // because we know that people do not read the API ref properly,
    // here is some protection to ensure that WindowManager does the
    // destruction and not anyone else.
    WindowManager& wmgr = WindowManager::getSingleton();

    if (wmgr.isWindowPresent(this->getName()))
    {
        wmgr.destroyWindow(this);

        // now return, the rest of what we need to do will happen
        // once WindowManager re-calls this method.
        return;
    }

    releaseInput();

    // signal our imminent destruction
    WindowEventArgs args(this);
    onDestructionStarted(args);

    // double check we are detached from parent
    if (d_parent != NULL)
    {
        d_parent->removeChildWindow(this);
    }

    cleanupChildren();
}

bool Window::isUsingDefaultTooltip(void) const
{
    return d_customTip == 0;
}

Tooltip* Window::getTooltip(void) const
{
    return isUsingDefaultTooltip() ? System::getSingleton().getDefaultTooltip() : d_customTip;
}

void Window::setTooltip(Tooltip* tooltip)
{
    // destroy current custom tooltip if one exists and we created it
    if (d_customTip && d_weOwnTip)
        WindowManager::getSingleton().destroyWindow(d_customTip);

    // set new custom tooltip 
    d_weOwnTip = false;
    d_customTip = tooltip;
}

void Window::setTooltipType(const String& tooltipType)
{
    // destroy current custom tooltip if one exists and we created it
    if (d_customTip && d_weOwnTip)
        WindowManager::getSingleton().destroyWindow(d_customTip);

    if (tooltipType.empty())
    {
        d_customTip = 0;
        d_weOwnTip = false;
    }
    else
    {
        try
        {
            d_customTip = static_cast<Tooltip*>(WindowManager::getSingleton().createWindow(tooltipType, getName() + "__auto_tooltip__"));
            d_weOwnTip = true;
        }
        catch (UnknownObjectException x)
        {
            d_customTip = 0;
            d_weOwnTip = false;
        }
    }
}

String Window::getTooltipType(void) const
{
    return isUsingDefaultTooltip() ? String("") : d_customTip->getType();
}

void Window::setTooltipText(const String& tip)
{
    d_tooltipText = tip;

    Tooltip* tooltip = getTooltip();

    if (tooltip && tooltip->getTargetWindow() == this)
    {
        tooltip->setText(tip);
    }
}

const String& Window::getTooltipText(void) const
{
    if (d_inheritsTipText && d_parent && d_tooltipText.empty())
    {
        return d_parent->getTooltipText();
    }
    else
    {
        return d_tooltipText;
    }
}

bool Window::inheritsTooltipText(void) const
{
    return d_inheritsTipText;
}
   
void Window::setInheritsTooltipText(bool setting)
{
    if (d_inheritsTipText != setting)
    {
        d_inheritsTipText = setting;

        // TODO: Maybe add a 'setting changed' event for this?
    }
}

void Window::doRiseOnClick(void)
{
    // does this window rise on click?
    if (d_riseOnClick)
    {
        moveToFront_impl(true);
    }
    else if (d_parent)
    {
        d_parent->doRiseOnClick();
    }
}

void Window::setWindowArea_impl(const UVector2& pos, const UVector2& size, bool topLeftSizing, bool fireEvents)
{
    // notes of what we did
    bool moved = false, sized;
    
    // save original size so we can work out how to behave later on
    Size oldSize(d_pixelSize);

    // calculate pixel sizes for everything, so we have a common format for comparisons.
    Vector2 absMax(d_maxSize.asAbsolute(System::getSingleton().getRenderer()->getSize()));
    Vector2 absMin(d_minSize.asAbsolute(System::getSingleton().getRenderer()->getSize()));
    d_pixelSize = size.asAbsolute(getParentSize()).asSize();

    // limit new pixel size to: minSize <= newSize <= maxSize
    if (d_pixelSize.d_width < absMin.d_x)
        d_pixelSize.d_width = absMin.d_x;
    else if (d_pixelSize.d_width > absMax.d_x)
        d_pixelSize.d_width = absMax.d_x;
    if (d_pixelSize.d_height < absMin.d_y)
        d_pixelSize.d_height = absMin.d_y;
    else if (d_pixelSize.d_height > absMax.d_y)
        d_pixelSize.d_height = absMax.d_y;

    d_area.setSize(size);
    sized = (d_pixelSize != oldSize);

    // If this is a top/left edge sizing op, only modify position if the size actually changed.
    // If it is not a sizing op, then position may always change.
    if (!topLeftSizing || sized)
    {
        // only update position if a change has occurred.
        if (pos != d_area.d_min)
        {
            d_area.setPosition(pos);
            moved = true;
        }
    }

    // fire events as required
    if (fireEvents)
    {
        WindowEventArgs args(this);

        if (moved)
        {
            onMoved(args);
            // reset handled so 'sized' event can re-use (since  wo do not care about it)
            args.handled = false;
        }
    
        if (sized)
        {
            onSized(args);
        }
    }
}

void Window::setWindowArea(const UDim& xpos, const UDim& ypos, const UDim& width, const UDim& height)
{
    setWindowArea(UVector2(xpos, ypos), UVector2(width, height));
}

void Window::setWindowArea(const UVector2& pos, const UVector2& size)
{
    setWindowArea_impl(pos, size);
}

void Window::setWindowArea(const URect& area)
{
    setWindowArea(area.d_min, area.getSize());
}

void Window::setWindowPosition(const UVector2& pos)
{
    setWindowArea_impl(pos, d_area.getSize());
}

void Window::setWindowXPosition(const UDim& x)
{
    setWindowArea_impl(UVector2(x, d_area.d_min.d_y), d_area.getSize());
}

void Window::setWindowYPosition(const UDim& y)
{
    setWindowArea_impl(UVector2(d_area.d_min.d_x, y), d_area.getSize());
}

void Window::setWindowSize(const UVector2& size)
{
    setWindowArea_impl(d_area.getPosition(), size);
}

void Window::setWindowWidth(const UDim& width)
{
    setWindowArea_impl(d_area.getPosition(), UVector2(width, d_area.getSize().d_y));
}

void Window::setWindowHeight(const UDim& height)
{
    setWindowArea_impl(d_area.getPosition(), UVector2(d_area.getSize().d_x, height));
}

void Window::setWindowMaxSize(const UVector2& size)
{
    d_maxSize = size;

    // set window area back on itself to cause new maximum size to be applied if required.
    setWindowArea(d_area);
}

void Window::setWindowMinSize(const UVector2& size)
{
    d_minSize = size;

    // set window area back on itself to cause new minimum size to be applied if required.
    setWindowArea(d_area);
}

const URect& Window::getWindowArea() const
{
    return d_area;
}

const UVector2& Window::getWindowPosition() const
{
    return d_area.d_min;
}

const UDim& Window::getWindowXPosition() const
{
    return d_area.d_min.d_x;
}

const UDim& Window::getWindowYPosition() const
{
    return d_area.d_min.d_y;
}

UVector2 Window::getWindowSize() const
{
    return d_area.getSize();
}

UDim Window::getWindowWidth() const
{
    return d_area.getSize().d_x;
}

UDim Window::getWindowHeight() const
{
    return d_area.getSize().d_y;
}

const UVector2& Window::getWindowMaxSize() const
{
    return d_maxSize;
}

const UVector2& Window::getWindowMinSize() const
{
    return d_minSize;
}

void Window::setVerticalAlignment(const VerticalAlignment alignment)
{
    if (d_vertAlign != alignment)
    {
        d_vertAlign = alignment;

        WindowEventArgs args(this);
        onVerticalAlignmentChanged(args);
    }
}

void Window::setHorizontalAlignment(const HorizontalAlignment alignment)
{
    if (d_horzAlign != alignment)
    {
        d_horzAlign = alignment;

        WindowEventArgs args(this);
        onHorizontalAlignmentChanged(args);
    }
}

const String& Window::getLookNFeel()
{
    return d_lookName;
}

void Window::setLookNFeel(const String& falagardType, const String& look)
{
    if (d_lookName.empty())
    {
        d_falagardType = falagardType;
        d_lookName = look;
        Logger::getSingleton().logEvent("Assigning LookNFeel '" + look +"' to window '" + d_name + "'.", Informative);

        // Work to initialse the look and feel...
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(look);
        // Add property definitions, apply property initialisers and create child widgets.
        wlf.initialiseWidget(*this);
    }
    else
    {
        throw InvalidRequestException("Window::setLookNFeel - The window '" + d_name + "' already has a look assigned (" + d_lookName + ").");
    }
}

void Window::setModalState(bool state)
{
	bool already_modal = getModalState();

	// if going modal and not already the modal target
	if (state == true && !already_modal)
	{
		activate();
		System::getSingleton().setModalTarget(this);
	}
	// clear the modal target if we were it
	else if (already_modal)
	{
		System::getSingleton().setModalTarget(NULL);
	}
}

void Window::performChildWindowLayout()
{
    if (!d_lookName.empty())
    {
        // here we just grab the look and feel and get it to layout its defined children
        try
        {
            const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
            // get look'n'feel to layout any child windows it created.
            wlf.layoutChildWidgets(*this);
        }
        catch (UnknownObjectException)
        {
            Logger::getSingleton().logEvent("Window::performChildWindowLayout - assigned widget look was not found.", Errors);
        }
    }
}

const String& Window::getUserString(const String& name) const
{
    UserStringMap::const_iterator iter = d_userStrings.find(name);

    if (iter != d_userStrings.end())
    {
        return (*iter).second;
    }
    else
    {
        throw UnknownObjectException("Window::getUserString - a user string named '" + name + "' has not been set for this Window.");
    }
}

bool Window::isUserStringDefined(const String& name) const
{
     return d_userStrings.find(name) != d_userStrings.end();
}

void Window::setUserString(const String& name, const String& value)
{
    d_userStrings[name] = value;
}

void Window::writeXMLToStream(OutStream& out_stream) const
{
    // output opening Window tag
    out_stream << "<Window Type=\"" << getType() << "\" ";
    // write name if not auto-generated
    if (getName().compare(0, WindowManager::GeneratedWindowNameBase.length(), WindowManager::GeneratedWindowNameBase) != 0)
    {
        out_stream << "Name=\"" << getName() << "\" ";
    }
    // close opening tag
    out_stream << ">" << std::endl;

    // write out properties.
    writePropertiesXML(out_stream);
    // write out attached child windows.
    writeChildWindowsXML(out_stream);
    // now ouput closing Window tag
    out_stream << "</Window>" << std::endl;
}

int Window::writePropertiesXML(OutStream& out_stream) const
{
    int propertiesWritten = 0;
    PropertyIterator iter =  PropertySet::getIterator();

    while(!iter.isAtEnd())
    {
			try
			{
        // only write property if it's not at the default state
        if (!iter.getCurrentValue()->isDefault(this))
        {
            iter.getCurrentValue()->writeXMLToStream(this, out_stream);
            ++propertiesWritten;
        }
			}
			catch (InvalidRequestException)
			{
				// This catches error(s) from the MultiLineColumnList for example
				Logger::getSingleton().logEvent("Window::writePropertiesXML - property receiving failed. Continuing...", Errors);
			}

        ++iter;
    }

    return propertiesWritten;
}

int Window::writeChildWindowsXML(OutStream& out_stream) const
{
    int windowsWritten = 0;

    for (uint i = 0; i < getChildCount(); ++i)
    {
        Window* child = d_children[i];

        // conditional to ensure that auto created windows are not written.
        if (child->getName().find("__auto_") == String::npos)
        {
            child->writeXMLToStream(out_stream);
            ++windowsWritten;
        }
    }

    return windowsWritten;
}

void Window::addWindowToDrawList(Window& wnd, bool at_back)
{
    // add behind other windows in same group
    if (at_back)
    {
         // calculate position where window should be added for drawing
        ChildList::iterator pos = d_drawList.begin();
        if (wnd.isAlwaysOnTop())
        {
            // find first topmost window
            while ((pos != d_drawList.end()) && (!(*pos)->isAlwaysOnTop()))
                ++pos;
        }
        // add window to draw list
        d_drawList.insert(pos, &wnd);
    }
    // add in front of other windows in group
    else
    {
        // calculate position where window should be added for drawing
        ChildList::reverse_iterator	position = d_drawList.rbegin();
        if (!wnd.isAlwaysOnTop())
        {
            // find last non-topmost window
            while ((position != d_drawList.rend()) && ((*position)->isAlwaysOnTop()))
                ++position;
        }
        // add window to draw list
        d_drawList.insert(position.base(), &wnd);
    }
}

void Window::removeWindowFromDrawList(const Window& wnd)
{
    // if draw list is not empty
    if (!d_drawList.empty())
    {
        // attempt to find the window in the draw list
        ChildList::iterator	position = std::find(d_drawList.begin(), d_drawList.end(), &wnd);

        // remove the window if it was found in the draw list
        if (position != d_drawList.end())
            d_drawList.erase(position);
    }
}

Window* Window::getActiveSibling()
{
    // initialise with this if we are active, else 0
    Window* activeWnd = isActive() ? this : 0;

    // if active window not already known, and we have a parent window
    if (!activeWnd && d_parent)
    {
        // scan backwards through the draw list, as this will
        // usually result in the fastest result.
        uint idx = d_parent->getChildCount();
        while (idx-- > 0)
        {
            // if this child is active
            if (d_parent->d_drawList[idx]->isActive())
            {
                // set the return value
                activeWnd = d_parent->d_drawList[idx];
                // exit loop early, as we have found what we needed
                break;
            }
        }
    }

    // return whatever we discovered
    return activeWnd;
}

void Window::rename(const String& new_name)
{
    WindowManager& winMgr = WindowManager::getSingleton();
    /*
     * Client code should never call this, but again, since we know people do
     * not read and stick to the API reference, here is some built-in protection
     * which ensures that things are handled via the WindowManager anyway.
     */
    if (winMgr.isWindowPresent(d_name))
    {
        winMgr.renameWindow(this, new_name);
        // now we return, since the work was already done when WindowManager
        // re-called this function in the proper manner.
        return;
    }

    if (winMgr.isWindowPresent(new_name))
        throw AlreadyExistsException("Window::rename - a Window named '" +
                new_name + "' already exists within the system.");

    // rename Falagard created child windows
    if (!d_lookName.empty())
    {
        const WidgetLookFeel& wlf =
                WidgetLookManager::getSingleton().getWidgetLook(d_lookName);

        // get WidgetLookFeel to rename the children it created
        wlf.renameChildren(*this, new_name);
    }

    // how to detect other auto created windows.
    const String autoPrefix(d_name + "__auto_");
    // length of current name
    const size_t oldNameLength = d_name.length();

    // now rename all remaining auto-created windows attached
    for (size_t i = 0; i < getChildCount(); ++i)
    {
        // is this an auto created window that we created?
        if (!d_children[i]->d_name.compare(0, autoPrefix.length(), autoPrefix))
        {
            winMgr.renameWindow(d_children[i],
                                new_name +
                                d_children[i]->d_name.substr(oldNameLength));
        }
    }

    // log this under insane level
    Logger::getSingleton().logEvent("Renamed window: " + d_name +
                                    " as: " + new_name,
                                    Informative);

    // finally, set our new name
    d_name = new_name;
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Begin event triggers section

*************************************************************************/
//////////////////////////////////////////////////////////////////////////

void Window::onSized(WindowEventArgs& e)
{
	// inform children their parent has been re-sized
	uint child_count = getChildCount();
	for (uint i = 0; i < child_count; ++i)
	{
		WindowEventArgs args(this);
		d_children[i]->onParentSized(args);
	}

	performChildWindowLayout();

	requestRedraw();

	fireEvent(EventSized, e, EventNamespace);
}


void Window::onMoved(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    System::getSingleton().signalRedraw();
	fireEvent(EventMoved, e, EventNamespace);
}


void Window::onTextChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventTextChanged, e, EventNamespace);
}


void Window::onFontChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventFontChanged, e, EventNamespace);
}


void Window::onAlphaChanged(WindowEventArgs& e)
{
	// scan child list and call this method for all children that inherit alpha
	int child_count = getChildCount();

	for (int i = 0; i < child_count; ++i)
	{
		if (d_children[i]->inheritsAlpha())
		{
            WindowEventArgs args(d_children[i]);
			d_children[i]->onAlphaChanged(args);
		}

	}

	requestRedraw();
	fireEvent(EventAlphaChanged, e, EventNamespace);
}


void Window::onIDChanged(WindowEventArgs& e)
{
	fireEvent(EventIDChanged, e, EventNamespace);
}


void Window::onShown(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventShown, e, EventNamespace);
}


void Window::onHidden(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventHidden, e, EventNamespace);
}


void Window::onEnabled(WindowEventArgs& e)
{
    // signal all non-disabled children that they are now enabled (via inherited state)
    uint child_count = getChildCount();
    for (uint i = 0; i < child_count; ++i)
    {
        if (d_children[i]->d_enabled)
        {
            WindowEventArgs args(d_children[i]);
            d_children[i]->onEnabled(args);
        }
    }

    requestRedraw();
    fireEvent(EventEnabled, e, EventNamespace);
}


void Window::onDisabled(WindowEventArgs& e)
{
    // signal all non-disabled children that they are now disabled (via inherited state)
    uint child_count = getChildCount();
    for (uint i = 0; i < child_count; ++i)
    {
        if (d_children[i]->d_enabled)
        {
            WindowEventArgs args(d_children[i]);
            d_children[i]->onDisabled(args);
        }
    }

	requestRedraw();
	fireEvent(EventDisabled, e, EventNamespace);
}


void Window::onMetricsChanged(WindowEventArgs& e)
{
	fireEvent(EventMetricsModeChanged, e, EventNamespace);
}


void Window::onClippingChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventClippedByParentChanged, e, EventNamespace);
}


void Window::onParentDestroyChanged(WindowEventArgs& e)
{
	fireEvent(EventDestroyedByParentChanged, e, EventNamespace);
}


void Window::onInheritsAlphaChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventInheritsAlphaChanged, e, EventNamespace);
}


void Window::onAlwaysOnTopChanged(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    System::getSingleton().signalRedraw();
	fireEvent(EventAlwaysOnTopChanged, e, EventNamespace);
}


void Window::onCaptureGained(WindowEventArgs& e)
{
	fireEvent(EventInputCaptureGained, e, EventNamespace);
}


void Window::onCaptureLost(WindowEventArgs& e)
{
    // reset auto-repeat state
    d_repeatButton = NoButton;

	// handle restore of previous capture window as required.
	if (d_restoreOldCapture && (d_oldCapture != NULL)) {
		d_oldCapture->onCaptureLost(e);
		d_oldCapture = NULL;
	}

	// handle case where mouse is now in a different window
	// (this is a bit of a hack that uses the mouse input injector to handle this for us).
	System::getSingleton().injectMouseMove(0, 0);

	fireEvent(EventInputCaptureLost, e, EventNamespace);
}


void Window::onRenderingStarted(WindowEventArgs& e)
{
	fireEvent(EventRenderingStarted, e, EventNamespace);
}


void Window::onRenderingEnded(WindowEventArgs& e)
{
	fireEvent(EventRenderingEnded, e, EventNamespace);
}


void Window::onZChanged(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    System::getSingleton().signalRedraw();
	fireEvent(EventZOrderChanged, e, EventNamespace);
}


void Window::onDestructionStarted(WindowEventArgs& e)
{
	fireEvent(EventDestructionStarted, e, EventNamespace);
}


void Window::onActivated(ActivationEventArgs& e)
{
	d_active = true;
	requestRedraw();
	fireEvent(EventActivated, e, EventNamespace);
}


void Window::onDeactivated(ActivationEventArgs& e)
{
	// first de-activate all children
	uint child_count = getChildCount();
	for (uint i = 0; i < child_count; ++i)
	{
		if (d_children[i]->isActive())
		{
			d_children[i]->onDeactivated(e);
		}

	}

	d_active = false;
	requestRedraw();
	fireEvent(EventDeactivated, e, EventNamespace);
}


void Window::onParentSized(WindowEventArgs& e)
{
    // set window area back on itself to cause minimum and maximum size
    // constraints to be applied as required.  (fire no events though)
    setWindowArea_impl(d_area.getPosition(), d_area.getSize(), false, false);

    // now see if events should be fired.
    if ((d_area.d_min.d_x.d_scale != 0) || (d_area.d_min.d_y.d_scale != 0))
    {
        WindowEventArgs args(this);
        onMoved(args);
    }

    if ((d_area.d_max.d_x.d_scale != 0) || (d_area.d_max.d_y.d_scale != 0))
    {
        WindowEventArgs args(this);
        onSized(args);
    }
    
    fireEvent(EventParentSized, e, EventNamespace);
}


void Window::onChildAdded(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    System::getSingleton().signalRedraw();
	fireEvent(EventChildAdded, e, EventNamespace);
}


void Window::onChildRemoved(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    System::getSingleton().signalRedraw();
	fireEvent(EventChildRemoved, e, EventNamespace);
}


void Window::onMouseEnters(MouseEventArgs& e)
{
	// set the mouse cursor
	MouseCursor::getSingleton().setImage(getMouseCursor());

    // perform tooltip control
    Tooltip* tip = getTooltip();
    if (tip)
    {
        tip->setTargetWindow(this);
    }

    fireEvent(EventMouseEnters, e, EventNamespace);
}


void Window::onMouseLeaves(MouseEventArgs& e)
{
    // perform tooltip control
    Tooltip* tip = getTooltip();
    if (tip)
    {
        tip->setTargetWindow(0);
    }
    
    fireEvent(EventMouseLeaves, e, EventNamespace);
}


void Window::onMouseMove(MouseEventArgs& e)
{
    // perform tooltip control
    Tooltip* tip = getTooltip();
    if (tip)
    {
        tip->resetTimer();
    }

    fireEvent(EventMouseMove, e, EventNamespace);
}


void Window::onMouseWheel(MouseEventArgs& e)
{
	fireEvent(EventMouseWheel, e, EventNamespace);
}


void Window::onMouseButtonDown(MouseEventArgs& e)
{
    // perform tooltip control
    Tooltip* tip = getTooltip();
    if (tip)
    {
        tip->setTargetWindow(0);
    }

    if (e.button == LeftButton)
	{
		doRiseOnClick();
	}

    // if auto repeat is enabled and we are not currently tracking
    // the button that was just pushed (need this button check because
    // it could be us that generated this event via auto-repeat).
    if (d_autoRepeat && d_repeatButton != e.button)
    {
        d_repeatButton = e.button;
        d_repeatElapsed = 0;
        d_repeating = false;
    }

	fireEvent(EventMouseButtonDown, e, EventNamespace);
}


void Window::onMouseButtonUp(MouseEventArgs& e)
{
    // reset auto-repeat state
    d_repeatButton = NoButton;

	fireEvent(EventMouseButtonUp, e, EventNamespace);
}


void Window::onMouseClicked(MouseEventArgs& e)
{
	fireEvent(EventMouseClick, e, EventNamespace);
}


void Window::onMouseDoubleClicked(MouseEventArgs& e)
{
	fireEvent(EventMouseDoubleClick, e, EventNamespace);
}


void Window::onMouseTripleClicked(MouseEventArgs& e)
{
	fireEvent(EventMouseTripleClick, e, EventNamespace);
}


void Window::onKeyDown(KeyEventArgs& e)
{
	fireEvent(EventKeyDown, e, EventNamespace);
}


void Window::onKeyUp(KeyEventArgs& e)
{
	fireEvent(EventKeyUp, e, EventNamespace);
}


void Window::onCharacter(KeyEventArgs& e)
{
	fireEvent(EventCharacterKey, e, EventNamespace);
}

void Window::onDragDropItemEnters(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemEnters, e, EventNamespace);
}

void Window::onDragDropItemLeaves(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemLeaves, e, EventNamespace);
}

void Window::onDragDropItemDropped(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemDropped, e, EventNamespace);
}

void Window::onVerticalAlignmentChanged(WindowEventArgs& e)
{
    fireEvent(EventVerticalAlignmentChanged, e, EventNamespace);
}

void Window::onHorizontalAlignmentChanged(WindowEventArgs& e)
{
    fireEvent(EventHorizontalAlignmentChanged, e, EventNamespace);
}

void Window::onRedrawRequested(WindowEventArgs& e)
{
    fireEvent(EventRedrawRequested, e, EventNamespace);
}

} // End of  CEGUI namespace section

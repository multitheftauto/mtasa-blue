/************************************************************************
    filename:   CEGUITooltip.cpp
    created:    21/2/2005
    author:     Paul D Turner
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
#include "elements/CEGUITooltip.h"
#include "CEGUILogger.h"
#include "CEGUIFont.h"
#include "CEGUIImage.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    //////////////////////////////////////////////////////////////////////////
    // Event name constants
    const String Tooltip::EventNamespace("Tooltip");
    const String Tooltip::EventHoverTimeChanged("HoverTimeChanged");
    const String Tooltip::EventDisplayTimeChanged("DisplayTimeChanged");
    const String Tooltip::EventFadeTimeChanged("FadeTimeChanged");
    const String Tooltip::EventTooltipActive("TooltipActive");
    const String Tooltip::EventTooltipInactive("TooltipInactive");
    // Property objects
    TooltipProperties::HoverTime    Tooltip::d_hoverTimeProperty;
    TooltipProperties::DisplayTime  Tooltip::d_displayTimeProperty;
    TooltipProperties::FadeTime     Tooltip::d_fadeTimeProperty;
    //////////////////////////////////////////////////////////////////////////

    Tooltip::Tooltip(const String& type, const String& name) :
            Window(type, name)
    {
        d_hoverTime     = 0.4f;
        d_displayTime   = 7.5f;
        d_fadeTime      = 0.33f;

        addTooltipEvents();
        addTooltipProperties();

        setClippedByParent(false);
        setDestroyedByParent(false);
        setAlwaysOnTop(true);

        switchToInactiveState();
    }

    Tooltip::~Tooltip(void)
    {}

    void Tooltip::positionSelf(void)
    {
        MouseCursor& cursor = MouseCursor::getSingleton();
        Rect screen(System::getSingleton().getRenderer()->getRect());
        Rect tipRect(getUnclippedPixelRect());
        const Image* mouseImage = cursor.getImage();

        Point mousePos(cursor.getPosition());
        Size mouseSz(0,0);

        if (mouseImage)
        {
            mouseSz = mouseImage->getSize();
        }

        Point tmpPos(mousePos.d_x + mouseSz.d_width, mousePos.d_y + mouseSz.d_height);
        tipRect.setPosition(tmpPos);

        // if tooltip would be off the right of the screen,
        // reposition to the other side of the mouse cursor.
        if (screen.d_right < tipRect.d_right)
        {
            tmpPos.d_x = mousePos.d_x - tipRect.getWidth() - 5;
        }

        // if tooltip would be off the bottom of the screen,
        // reposition to the other side of the mouse cursor.
        if (screen.d_bottom < tipRect.d_bottom)
        {
            tmpPos.d_y = mousePos.d_y - tipRect.getHeight() - 5;
        }

        // set final position of tooltip window.
        setPosition(Absolute, tmpPos);
    }

    void Tooltip::setTargetWindow(Window* wnd)
    {
        if (wnd)
        {
            if (d_target != wnd)
            {
                System::getSingleton().getGUISheet()->addChildWindow(this);
            }

            // set text to that of the tooltip text of the target
            setText(wnd->getTooltipText());

            // set size and potition of the tooltip window.
            setSize(Absolute, getTextSize());
            positionSelf();
        }

        resetTimer();
        d_target = wnd;
    }

    const Window* Tooltip::getTargetWindow()
    {
        return d_target;
    }

    Size Tooltip::getTextSize() const
    {
        const Font* fnt = getFont();

        if (fnt)
        {
            Rect area(System::getSingleton().getRenderer()->getRect());

            // get required size of the tool tip according to the text extents.
            // TODO: Add a proprty to allow specification of text formatting.
            float height = fnt->getFormattedLineCount(d_text, area, LeftAligned) * fnt->getLineSpacing();
            float width = fnt->getFormattedTextExtent(d_text, area, LeftAligned);

            return Size(width, height);
        }
        else
        {
            return Size(0,0);
        }
    }

    void Tooltip::resetTimer(void)
    {
        // only do the reset in active / inactive states, this is so that
        // the fades are not messed up by having the timer reset on them.
        if (d_state == Active || d_state == Inactive)
        {
            d_elapsed = 0;
        }
    }

    float Tooltip::getHoverTime(void) const
    {
        return d_hoverTime;
    }

    void Tooltip::setHoverTime(float seconds)
    {
        if (d_hoverTime != seconds)
        {
            d_hoverTime = seconds;

            WindowEventArgs args(this);
            onHoverTimeChanged(args);
        }
    }

    float Tooltip::getDisplayTime(void) const
    {
        return d_displayTime;
    }

    void Tooltip::setDisplayTime(float seconds)
    {
        if (d_displayTime != seconds)
        {
            d_displayTime = seconds;

            WindowEventArgs args(this);
            onDisplayTimeChanged(args);
        }
    }

    float Tooltip::getFadeTime(void) const
    {
        return d_fadeTime;
    }

    void Tooltip::setFadeTime(float seconds)
    {
        if (d_fadeTime != seconds)
        {
            d_fadeTime = seconds;

            WindowEventArgs args(this);
            onFadeTimeChanged(args);
        }
    }

    void Tooltip::doActiveState(float elapsed)
    {
        // if no target, switch immediately to inactive state.
        if (!d_target || d_target->getTooltipText().empty())
        {
            switchToInactiveState();
        }
        // else see if display timeout has been reached
        else if ((d_displayTime > 0) && ((d_elapsed += elapsed) >= d_displayTime))
        {
            // display time is up, switch states
            switchToFadeOutState();
        }
    }

    void Tooltip::doInactiveState(float elapsed)
    {
        if (d_target && !d_target->getTooltipText().empty() && ((d_elapsed += elapsed) >= d_hoverTime))
        {
            switchToFadeInState();
        }
    }

    void Tooltip::doFadeInState(float elapsed)
    {
        // if no target, switch immediately to inactive state.
        if (!d_target || d_target->getTooltipText().empty())
        {
            switchToInactiveState();
        }
        else
        {
            if ((d_elapsed += elapsed) >= d_fadeTime)
            {
                setAlpha(1.0f);
                switchToActiveState();
            }
            else
            {
                setAlpha((1.0f / d_fadeTime) * d_elapsed);
            }
        }
    }

    void Tooltip::doFadeOutState(float elapsed)
    {
        // if no target, switch immediately to inactive state.
        if (!d_target || d_target->getTooltipText().empty())
        {
            switchToInactiveState();
        }
        else
        {
            if ((d_elapsed += elapsed) >= d_fadeTime)
            {
                setAlpha(0.0f);
                switchToInactiveState();
            }
            else
            {
                setAlpha(1.0f - (1.0f / d_fadeTime) * d_elapsed);
            }
        }
    }

    void Tooltip::switchToInactiveState(void)
    {
        setAlpha(0.0f);
        d_state = Inactive;
        d_elapsed = 0;

        if (d_parent)
            d_parent->removeChildWindow(this);

        // fire event before target gets reset in case that information is required in handler.
        WindowEventArgs args(this);
        onTooltipInactive(args);

        d_target = 0;
        hide();
    }

    void Tooltip::switchToActiveState(void)
    {
        d_state = Active;
        d_elapsed = 0;
    }

    void Tooltip::switchToFadeInState(void)
    {
        positionSelf();
        d_state = FadeIn;
        d_elapsed = 0;
        show();

        // fire event.  Not really active at the moment, but this is the "right" time
        // for this event (just prior to anything getting displayed).
        WindowEventArgs args(this);
        onTooltipActive(args);
    }

    void Tooltip::switchToFadeOutState(void)
    {
        d_state = FadeOut;
        d_elapsed = 0;
    }

    void Tooltip::updateSelf(float elapsed)
    {
        // base class processing.
        Window::updateSelf(elapsed);

        // do something based upon current Tooltip state.
        switch (d_state)
        {
        case Inactive:
            doInactiveState(elapsed);
            break;

        case Active:
            doActiveState(elapsed);
            break;

        case FadeIn:
            doFadeInState(elapsed);
            break;

        case FadeOut:
            doFadeOutState(elapsed);
            break;

        default:
            // This should never happen.
            Logger::getSingleton().logEvent("Tooltip (Name: " + getName() + "of Class: " + getType() + ") is in an unknown state.  Switching to Inactive state.", Errors);
            switchToInactiveState();
        }
    }

    void Tooltip::addTooltipEvents(void)
    {
        addEvent(EventHoverTimeChanged);
        addEvent(EventDisplayTimeChanged);
        addEvent(EventFadeTimeChanged);
        addEvent(EventTooltipActive);
        addEvent(EventTooltipInactive);
    }

    void Tooltip::addTooltipProperties(void)
    {
        addProperty(&d_hoverTimeProperty);
        addProperty(&d_displayTimeProperty);
        addProperty(&d_fadeTimeProperty);
    }

    void Tooltip::onMouseEnters(MouseEventArgs& e)
    {
        positionSelf();

        Window::onMouseEnters(e);
    }

    void Tooltip::onTextChanged(WindowEventArgs& e)
    {
        // base class processing
        Window::onTextChanged(e);

        // set size and potition of the tooltip window to consider new text
        setSize(Absolute, getTextSize());
        positionSelf();

        // we do not signal we handled it, in case user wants to hear
        // about text changes too.
    }

    void Tooltip::onHoverTimeChanged(WindowEventArgs& e)
    {
        fireEvent(EventHoverTimeChanged, e, EventNamespace);
    }

    void Tooltip::onDisplayTimeChanged(WindowEventArgs& e)
    {
        fireEvent(EventDisplayTimeChanged, e, EventNamespace);
    }

    void Tooltip::onFadeTimeChanged(WindowEventArgs& e)
    {
        fireEvent(EventFadeTimeChanged, e, EventNamespace);
    }

    void Tooltip::onTooltipActive(WindowEventArgs& e)
    {
        fireEvent(EventTooltipActive, e, EventNamespace);
    }

    void Tooltip::onTooltipInactive(WindowEventArgs& e)
    {
        fireEvent(EventTooltipInactive, e, EventNamespace);
    }

} // End of  CEGUI namespace section

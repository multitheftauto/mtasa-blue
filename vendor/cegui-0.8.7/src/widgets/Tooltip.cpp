/***********************************************************************
    created:    21/2/2005
    author:     Paul D Turner
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
#include "CEGUI/widgets/Tooltip.h"
#include "CEGUI/Logger.h"
#include "CEGUI/Font.h"
#include "CEGUI/Image.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String Tooltip::WidgetTypeName("CEGUI/Tooltip");

    //////////////////////////////////////////////////////////////////////////
    // Event name constants
    const String Tooltip::EventNamespace("Tooltip");
    const String Tooltip::EventHoverTimeChanged("HoverTimeChanged");
    const String Tooltip::EventDisplayTimeChanged("DisplayTimeChanged");
    const String Tooltip::EventFadeTimeChanged("FadeTimeChanged");
    const String Tooltip::EventTooltipActive("TooltipActive");
    const String Tooltip::EventTooltipInactive("TooltipInactive");
    const String Tooltip::EventTooltipTransition("TooltipTransition");
    //////////////////////////////////////////////////////////////////////////

    /*************************************************************************
        TooltipWindowRenderer
    *************************************************************************/
    TooltipWindowRenderer::TooltipWindowRenderer(const String& name) :
        WindowRenderer(name, Tooltip::EventNamespace)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    Tooltip::Tooltip(const String& type, const String& name) :
        Window(type, name),

        d_hoverTime(0.4f),
        d_displayTime(7.5f),
        d_inPositionSelf(false)
    {
        addTooltipProperties();

        setClippedByParent(false);
        setDestroyedByParent(false);
        setAlwaysOnTop(true);
        setMousePassThroughEnabled(true);
        
        // we need updates even when not visible
        setUpdateMode(WUM_ALWAYS);

        switchToInactiveState();
        hide();
    }

    Tooltip::~Tooltip(void)
    {}

    void Tooltip::positionSelf(void)
    {
        // no recusion allowed for this function!
        if (d_inPositionSelf)
            return;

        d_inPositionSelf = true;

        MouseCursor& cursor = getGUIContext().getMouseCursor();
        Rectf screen(Vector2f(0, 0), getRootContainerSize());
        Rectf tipRect(getUnclippedOuterRect().get());
        const Image* mouseImage = cursor.getImage();

        Vector2f mousePos(cursor.getPosition());
        Sizef mouseSz(0,0);

        if (mouseImage)
        {
            mouseSz = mouseImage->getRenderedSize();
        }

        Vector2f tmpPos(mousePos.d_x + mouseSz.d_width, mousePos.d_y + mouseSz.d_height);
        tipRect.setPosition(tmpPos);

        // if tooltip would be off the right of the screen,
        // reposition to the other side of the mouse cursor.
        if (screen.right() < tipRect.right())
        {
            tmpPos.d_x = mousePos.d_x - tipRect.getWidth() - 5;
        }

        // if tooltip would be off the bottom of the screen,
        // reposition to the other side of the mouse cursor.
        if (screen.bottom() < tipRect.bottom())
        {
            tmpPos.d_y = mousePos.d_y - tipRect.getHeight() - 5;
        }

        // set final position of tooltip window.
        setPosition(
            UVector2(cegui_absdim(tmpPos.d_x),
                     cegui_absdim(tmpPos.d_y)));

        d_inPositionSelf = false;
    }

    void Tooltip::sizeSelf(void)
    {
        Sizef textSize(getTextSize());

        setSize(USize(cegui_absdim(textSize.d_width),
                      cegui_absdim(textSize.d_height)));
    }

    void Tooltip::setTargetWindow(Window* wnd)
    {
        if (!wnd)
        {
            d_target = wnd;
        }
        else if (wnd != this)
        {
            if (d_target != wnd)
            {
                wnd->getGUIContext().getRootWindow()->addChild(this);
                d_target = wnd;
            }

            // set text to that of the tooltip text of the target
            setText(wnd->getTooltipTextIncludingInheritance());

            // set size and position of the tooltip window.
            sizeSelf();
            positionSelf();
        }

        resetTimer();

        if (d_active)
        {
            WindowEventArgs args(this);
            onTooltipTransition(args);
        }
    }

    const Window* Tooltip::getTargetWindow()
    {
        return d_target;
    }

    Sizef Tooltip::getTextSize() const
    {
        if (d_windowRenderer != 0)
        {
            TooltipWindowRenderer* wr = (TooltipWindowRenderer*)d_windowRenderer;
            return wr->getTextSize();
        }
        else
        {
            return getTextSize_impl();
        }
    }

    Sizef Tooltip::getTextSize_impl() const
    {
        const RenderedString& rs(getRenderedString());
        Sizef sz(0.0f, 0.0f);

        for (size_t i = 0; i < rs.getLineCount(); ++i)
        {
            const Sizef line_sz(rs.getPixelSize(this, i));
            sz.d_height += line_sz.d_height;

            if (line_sz.d_width > sz.d_width)
                sz.d_width = line_sz.d_width;
        }

        return sz;
    }

    void Tooltip::resetTimer(void)
    {
        d_elapsed = 0;
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

    void Tooltip::doActiveState(float elapsed)
    {
        // if no target, switch immediately to inactive state.
        if (!d_target || d_target->getTooltipTextIncludingInheritance().empty())
        {
            // hide immediately since the text is empty
            hide();

            switchToInactiveState();
        }
        // else see if display timeout has been reached
        else if ((d_displayTime > 0) && ((d_elapsed += elapsed) >= d_displayTime))
        {
            // display time is up, switch states
            switchToInactiveState();
        }
    }

    void Tooltip::doInactiveState(float elapsed)
    {
        if (d_target && !d_target->getTooltipTextIncludingInheritance().empty() && ((d_elapsed += elapsed) >= d_hoverTime))
        {
            switchToActiveState();
        }
    }

    void Tooltip::switchToInactiveState(void)
    {
        d_active = false;
        d_elapsed = 0;

        // fire event before target gets reset in case that information is required in handler.
        WindowEventArgs args(this);
        onTooltipInactive(args);

        d_target = 0;
    }

    void Tooltip::switchToActiveState(void)
    {
        positionSelf();
        show();

        d_active = true;
        d_elapsed = 0;

        WindowEventArgs args(this);
        onTooltipActive(args);
    }

    bool Tooltip::validateWindowRenderer(const WindowRenderer* renderer) const
	{
		return dynamic_cast<const TooltipWindowRenderer*>(renderer) != 0;
	}

    void Tooltip::updateSelf(float elapsed)
    {
        // base class processing.
        Window::updateSelf(elapsed);

        // do something based upon current Tooltip state.
        if (d_active)
        {
            doActiveState(elapsed);
        }
        else
        {
            doInactiveState(elapsed);
        }
    }

    void Tooltip::addTooltipProperties(void)
    {
        const String& propertyOrigin = WidgetTypeName;

        CEGUI_DEFINE_PROPERTY(Tooltip, float,
            "HoverTime", "Property to get/set the hover timeout value in seconds.  Value is a float.",
            &Tooltip::setHoverTime, &Tooltip::getHoverTime, 0.4f
        );

        CEGUI_DEFINE_PROPERTY(Tooltip, float,
            "DisplayTime", "Property to get/set the display timeout value in seconds.  Value is a float.",
            &Tooltip::setDisplayTime, &Tooltip::getDisplayTime, 7.5f
        );
    }

    void Tooltip::onHidden(WindowEventArgs& e)
    {
        Window::onHidden(e);

        // The animation will take care of fade out or whatnot,
        // at the end it will hide the tooltip to let us know the transition
        // is done. At this point we will remove the tooltip from the
        // previous parent.

        // NOTE: There has to be a fadeout animation! Even if it's a 0 second
        //       immediate hide animation.

        if (getParent())
        {
            getParent()->removeChild(this);
        }
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

        // set size and position of the tooltip window to consider new text
        sizeSelf();
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

    void Tooltip::onTooltipActive(WindowEventArgs& e)
    {
        fireEvent(EventTooltipActive, e, EventNamespace);
    }

    void Tooltip::onTooltipInactive(WindowEventArgs& e)
    {
        fireEvent(EventTooltipInactive, e, EventNamespace);
    }

    void Tooltip::onTooltipTransition(WindowEventArgs& e)
    {
        fireEvent(EventTooltipTransition, e, EventNamespace);
    }
} // End of  CEGUI namespace section

/***********************************************************************
    created:    Thu Jul 7 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/WindowRendererSets/Core/ScrollablePane.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardScrollablePane::TypeName("Core/ScrollablePane");


    FalagardScrollablePane::FalagardScrollablePane(const String& type) :
        ScrollablePaneWindowRenderer(type),
        d_widgetLookAssigned(false)
    {
    }

    Rectf FalagardScrollablePane::getViewableArea(void) const
    {
        ScrollablePane* w = (ScrollablePane*)d_window;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        bool v_visible = w->getVertScrollbar()->isVisible();
        bool h_visible = w->getHorzScrollbar()->isVisible();

        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            String area_name("ViewableArea");

            if (h_visible)
            {
                area_name += "H";
            }
            if (v_visible)
            {
                area_name += "V";
            }
            area_name += "Scroll";

            if (wlf.isNamedAreaDefined(area_name))
            {
                return wlf.getNamedArea(area_name).getArea().getPixelRect(*w);
            }
        }

        // default to plain ViewableArea
        return wlf.getNamedArea("ViewableArea").getArea().getPixelRect(*w);
    }

    void FalagardScrollablePane::render()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*d_window);
    }

    void FalagardScrollablePane::onLookNFeelAssigned()
    {
        d_widgetLookAssigned = true;
    }

    void FalagardScrollablePane::onLookNFeelUnassigned()
    {
        d_widgetLookAssigned = false;
    }

    Rectf FalagardScrollablePane::getUnclippedInnerRect() const
    {
        if (!d_widgetLookAssigned)
            return d_window->getUnclippedOuterRect().get();

        Rectf lr(getViewableArea());
        lr.offset(d_window->getUnclippedOuterRect().get().d_min);
        return lr;
    }

} // End of  CEGUI namespace section

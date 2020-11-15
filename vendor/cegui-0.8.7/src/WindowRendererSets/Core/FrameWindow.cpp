/***********************************************************************
    created:    Sat Jul 2 2005
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
#include "CEGUI/WindowRendererSets/Core/FrameWindow.h"
#include "CEGUI/widgets/FrameWindow.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/widgets/Titlebar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardFrameWindow::TypeName("Core/FrameWindow");

    FalagardFrameWindow::FalagardFrameWindow(const String& type) :
        WindowRenderer(type)
    {
    }

    void FalagardFrameWindow::render()
    {
        FrameWindow* w = (FrameWindow*)d_window;
        // do not render anything for the rolled-up state.
        if (w->isRolledup())
            return;

        // build state name
        String stateName(w->isEffectiveDisabled() ? "Disabled" : (w->isActive() ? "Active" : "Inactive"));
        stateName += w->isTitleBarEnabled() ? "WithTitle" : "NoTitle";
        stateName += w->isFrameEnabled() ? "WithFrame" : "NoFrame";

        const StateImagery* imagery;

        CEGUI_TRY
        {
            // get WidgetLookFeel for the assigned look.
            const WidgetLookFeel& wlf = getLookNFeel();
            // try and get imagery for our current state
            imagery = &wlf.getStateImagery(stateName);
        }
        CEGUI_CATCH (UnknownObjectException&)
        {
            // log error so we know imagery is missing, and then quit.
            return;
        }

        // peform the rendering operation.
        imagery->render(*w);
    }

    Rectf FalagardFrameWindow::getUnclippedInnerRect(void) const
    {
        FrameWindow* w = (FrameWindow*)d_window;
        if (w->isRolledup())
            return Rectf(0, 0, 0, 0);

        // build name of area to fetch
        String areaName("Client");
        areaName += w->isTitleBarEnabled() ? "WithTitle" : "NoTitle";
        areaName += w->isFrameEnabled() ? "WithFrame" : "NoFrame";

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        return wlf.getNamedArea(areaName).getArea().getPixelRect(*w, w->getUnclippedOuterRect().get());
    }

} // End of  CEGUI namespace section

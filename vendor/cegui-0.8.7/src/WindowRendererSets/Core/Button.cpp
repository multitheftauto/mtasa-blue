/***********************************************************************
    created:    Wed Jun 22 2005
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
#include "CEGUI/WindowRendererSets/Core/Button.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardButton::TypeName("Core/Button");

    FalagardButton::FalagardButton(const String& type) :
        WindowRenderer(type)
    {
    }

    void FalagardButton::render()
    {
        ButtonBase* w = (ButtonBase*)d_window;
        const WidgetLookFeel& wlf = getLookNFeel();

        bool norm = false;
        String state;

		if (w->isEffectiveDisabled())
		{
		    state = "Disabled";
		}
		else if (w->isPushed())
		{
            state = w->isHovering() ? "Pushed" : "PushedOff";
		}
        else if (w->isHovering())
		{
		    state = "Hover";
		}
		else
		{
		    state = "Normal";
		    norm = true;
		}

        if (!norm && !wlf.isStateImageryPresent(actualStateName(state)))
        {
            state = "Normal";
        }

        wlf.getStateImagery(actualStateName(state)).render(*w);
    }

} // End of  CEGUI namespace section

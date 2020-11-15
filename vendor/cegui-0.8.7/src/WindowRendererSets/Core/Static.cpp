/***********************************************************************
    created:    Sat Sep 17 2005
    author:     Tomas Lindquist Olsen (based on code by Paul D Turner)
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
#include "CEGUI/WindowRendererSets/Core/Static.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/TplWindowRendererProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardStatic::TypeName("Core/Static");

    FalagardStatic::FalagardStatic(const String& type) :
        WindowRenderer(type),
        d_frameEnabled(false),
        d_backgroundEnabled(false)
    {
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardStatic, bool,
            "FrameEnabled", "Property to get/set the state of the frame enabled setting for the FalagardStatic widget."
            "  Value is either \"true\" or \"false\".",
            &FalagardStatic::setFrameEnabled, &FalagardStatic::isFrameEnabled,
            true);
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardStatic, bool,
            "BackgroundEnabled", "Property to get/set the state of the frame background setting for the FalagardStatic widget."
            "  Value is either \"true\" or \"false\".",
            &FalagardStatic::setBackgroundEnabled, &FalagardStatic::isBackgroundEnabled,
            true);
    }

    void FalagardStatic::setFrameEnabled(bool setting)
    {
        if (d_frameEnabled != setting)
        {
            d_frameEnabled = setting;
            d_window->invalidate();
        }
    }

    void FalagardStatic::setBackgroundEnabled(bool setting)
    {
        if (d_backgroundEnabled != setting)
        {
            d_backgroundEnabled = setting;
            d_window->invalidate();
        }
    }

    void FalagardStatic::render()
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();

		bool is_enabled = !d_window->isEffectiveDisabled();

        // render frame section
        if (d_frameEnabled)
        {
            wlf.getStateImagery(is_enabled ? "EnabledFrame" : "DisabledFrame").render(*d_window);
        }

        // render background section
        if (d_backgroundEnabled)
        {
            const StateImagery* imagery;
            if (d_frameEnabled)
            {
                imagery = &wlf.getStateImagery(is_enabled ? "WithFrameEnabledBackground" : "WithFrameDisabledBackground");
            }
            else
            {
                imagery = &wlf.getStateImagery(is_enabled ? "NoFrameEnabledBackground" : "NoFrameDisabledBackground");
            }
            // peform the rendering operation.
            imagery->render(*d_window);
        }

        // render basic imagery
        wlf.getStateImagery(is_enabled ? "Enabled" : "Disabled").render(*d_window);
    }

} // End of  CEGUI namespace section

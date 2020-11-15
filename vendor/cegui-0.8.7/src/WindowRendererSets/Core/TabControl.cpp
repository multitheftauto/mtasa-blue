/***********************************************************************
    created:    Fri Jul 8 2005
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
#include "CEGUI/WindowRendererSets/Core/TabControl.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/TabButton.h"
#include "CEGUI/TplWindowRendererProperty.h"
// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardTabControl::TypeName("Core/TabControl");

    FalagardTabControl::FalagardTabControl(const String& type) :
        TabControlWindowRenderer(type)
    {
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardTabControl, String,
        "TabButtonType", "Property to get/set the widget type used when creating tab buttons.  Value should be \"[widgetTypeName]\".",
        &FalagardTabControl::setTabButtonType, &FalagardTabControl::getTabButtonType,
        "");
    }

    void FalagardTabControl::render()
    {
        const StateImagery* imagery;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // render basic imagery
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        imagery->render(*d_window);
    }

    TabButton* FalagardTabControl::createTabButton(const String& name) const
    {
        if (d_tabButtonType.empty())
        {
            CEGUI_THROW(InvalidRequestException(
                "d_tabButtonType has not been set!"));
        }

        Window* button = WindowManager::getSingleton().createWindow(d_tabButtonType, name);
        button->setAutoWindow(true);
        return static_cast<TabButton*>(button);
    }

    const String& FalagardTabControl::getTabButtonType() const
    {
        return d_tabButtonType;
    }

    void FalagardTabControl::setTabButtonType(const String& type)
    {
        d_tabButtonType = type;
    }

} // End of  CEGUI namespace section

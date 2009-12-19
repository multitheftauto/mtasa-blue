/************************************************************************
    filename:   FalStaticText.cpp
    created:    Tue Jul 5 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "FalStaticText.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIScrollbar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardStaticText::WidgetTypeName[] = "Falagard/StaticText";

    FalagardStaticText::FalagardStaticText(const String& type, const String& name) :
        StaticText(type, name)
    {
    }

    FalagardStaticText::~FalagardStaticText()
    {
    }

    void FalagardStaticText::populateRenderCache()
    {
        const StateImagery* imagery;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);

		bool is_enabled = !isDisabled();

        // render frame section
        if (d_frameEnabled)
        {
            imagery = &wlf.getStateImagery(is_enabled ? "EnabledFrame" : "DisabledFrame");
            // peform the rendering operation.
            imagery->render(*this);
        }

        // render background section
        if (d_backgroundEnabled)
        {
            imagery = &wlf.getStateImagery(is_enabled ? "EnabledBackground" : "DisabledBackground");
            // peform the rendering operation.
            imagery->render(*this);
        }

        // render basic imagery
        imagery = &wlf.getStateImagery(is_enabled ? "Enabled" : "Disabled");
        // peform the rendering operation.
        imagery->render(*this);

        // call base class
        StaticText::populateRenderCache();
    }

    Scrollbar* FalagardStaticText::createVertScrollbar(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(name));
    }

    Scrollbar* FalagardStaticText::createHorzScrollbar(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(name));
    }

    Rect FalagardStaticText::getTextRenderArea(void) const
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        bool v_visible = d_vertScrollbar->isVisible(true);
        bool h_visible = d_horzScrollbar->isVisible(true);

        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            String area_name("TextRenderArea");

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
                return wlf.getNamedArea(area_name).getArea().getPixelRect(*this);
            }
        }

        // default to plain TextRenderArea
        return wlf.getNamedArea("TextRenderArea").getArea().getPixelRect(*this);
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardStaticTextFactory::createWindow(const String& name)
    {
        return new FalagardStaticText(d_type, name);
    }

    void FalagardStaticTextFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section

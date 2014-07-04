/************************************************************************
    filename:   FalComboDropList.cpp
    created:    Mon Jul 4 2005
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
#include "FalComboDropList.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIScrollbar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardComboDropList::WidgetTypeName[] = "Falagard/ComboDropList";


    FalagardComboDropList::FalagardComboDropList(const String& type, const String& name) :
        ComboDropList(type, name)
    {
    }

    FalagardComboDropList::~FalagardComboDropList()
    {
    }

    Rect FalagardComboDropList::getListRenderArea(void) const
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        bool v_visible = d_vertScrollbar->isVisible(true);
        bool h_visible = d_horzScrollbar->isVisible(true);

        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            String area_name("ItemRenderingArea");

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

        // default to plain ItemRenderingArea
        return wlf.getNamedArea("ItemRenderingArea").getArea().getPixelRect(*this);
    }

    Scrollbar* FalagardComboDropList::createVertScrollbar(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(name));
    }

    Scrollbar* FalagardComboDropList::createHorzScrollbar(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(name));
    }

    void FalagardComboDropList::cacheListboxBaseImagery()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*this);
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardComboDropListFactory::createWindow(const String& name)
    {
        return new FalagardComboDropList(d_type, name);
    }

    void FalagardComboDropListFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section

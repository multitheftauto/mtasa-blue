/************************************************************************
    filename:   FalTooltip.cpp
    created:    Thu Jul 7 2005
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
#include "FalTooltip.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardTooltip::WidgetTypeName[] = "Falagard/Tooltip";

    FalagardTooltip::FalagardTooltip(const String& type, const String& name) :
        Tooltip(type, name)
    {
    }

    FalagardTooltip::~FalagardTooltip()
    {
    }

    void FalagardTooltip::populateRenderCache()
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for our current state
        const StateImagery* imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*this);
    }

    Size FalagardTooltip::getTextSize() const
    {
        Size sz(Tooltip::getTextSize());

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);

        Rect textArea(wlf.getNamedArea("TextArea").getArea().getPixelRect(*this));
        Rect wndArea(getAbsoluteRect());

        sz.d_width  += wndArea.getWidth() - textArea.getWidth();
        sz.d_height += wndArea.getHeight() - textArea.getHeight();
        return sz;
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardTooltipFactory::createWindow(const String& name)
    {
        return new FalagardTooltip(d_type, name);
    }

    void FalagardTooltipFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section

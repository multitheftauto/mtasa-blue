/************************************************************************
    filename:   FalPopupMenu.cpp
    created:    Fri Jul 8 2005
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
#include "FalPopupMenu.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardPopupMenu::WidgetTypeName[] = "Falagard/PopupMenu";

    FalagardPopupMenu::FalagardPopupMenu(const String& type, const String& name) :
        PopupMenu(type, name)
    {
    }

    FalagardPopupMenu::~FalagardPopupMenu()
    {
    }

    void FalagardPopupMenu::populateRenderCache()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*this);
    }

    void FalagardPopupMenu::sizeToContent_impl(void)
    {
        Rect renderArea(getItemRenderArea());
        Rect wndArea(getAbsoluteRect());

        // get size of content
        Size sz(getContentSize());

        // calculate the full size with the frame accounted for and resize the window to this
        sz.d_width  += wndArea.getWidth() - renderArea.getWidth();
        sz.d_height += wndArea.getHeight() - renderArea.getHeight();
        setSize(Absolute,sz);
    }

    Rect FalagardPopupMenu::getItemRenderArea(void) const
    {
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        return wlf.getNamedArea("ItemRenderArea").getArea().getPixelRect(*this);
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardPopupMenuFactory::createWindow(const String& name)
    {
        return new FalagardPopupMenu(d_type, name);
    }

    void FalagardPopupMenuFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section

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
#include "CEGUI/WindowRendererSets/Core/MenuItem.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/widgets/MenuItem.h"
#include "CEGUI/widgets/Menubar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String FalagardMenuItem::TypeName("Core/MenuItem");

//----------------------------------------------------------------------------//
FalagardMenuItem::FalagardMenuItem(const String& type) :
    ItemEntryWindowRenderer(type)
{
}

//----------------------------------------------------------------------------//
Sizef FalagardMenuItem::getItemPixelSize() const
{
    return getContentNamedArea().getArea().getPixelRect(*d_window).getSize();
}

//----------------------------------------------------------------------------//
const NamedArea& FalagardMenuItem::getContentNamedArea() const
{
    const WidgetLookFeel& wlf(getLookNFeel());

    if (static_cast<MenuItem*>(d_window)->getPopupMenu() && !parentIsMenubar() &&
        wlf.isNamedAreaDefined("HasPopupContentSize"))
    {
        return wlf.getNamedArea("HasPopupContentSize");
    }
    else
    {
        return wlf.getNamedArea("ContentSize");
    }
}

//----------------------------------------------------------------------------//
bool FalagardMenuItem::parentIsMenubar() const
{
    const Window* const parent = d_window->getParent();
    return parent && dynamic_cast<const Menubar*>(parent);
}

//----------------------------------------------------------------------------//
void FalagardMenuItem::render()
{
    MenuItem* w = (MenuItem*)d_window;
    // build name of state we're in
    String stateName(w->isEffectiveDisabled() ? "Disabled" : "Enabled");

    String suffix;

	// only show opened imagery if the menu items popup window is not closing
	// (otherwise it might look odd)
    if (w->isOpened() && !(w->hasAutoPopup() && w->isPopupClosing()))
        suffix = "PopupOpen";
    else if (w->isPushed())
        suffix = w->isHovering() ? "Pushed" : "PushedOff";
    else if (w->isHovering())
        suffix = "Hover";
    else
        suffix = "Normal";

    const StateImagery* imagery;
    // get WidgetLookFeel for the assigned look.
    const WidgetLookFeel& wlf = getLookNFeel();

    // try and get imagery for our current state
    if (wlf.isStateImageryPresent(stateName + suffix))
    {
        imagery = &wlf.getStateImagery(stateName + suffix);
    }
    else
    {
        imagery = &wlf.getStateImagery(stateName + "Normal");
    }

    // peform the rendering operation.
    imagery->render(*w);

    // only draw popup-open/closed-icon if we have a popup menu, and parent is not a menubar
    Window* parent_window = w->getParent();
    bool not_menubar = (!parent_window) ? true : !dynamic_cast<Menubar*>(parent_window);

    if (w->getPopupMenu() && not_menubar)
    {
        // get imagery for popup open/closed state
        imagery = &wlf.getStateImagery(w->isOpened() ? "PopupOpenIcon" : "PopupClosedIcon");
        // peform the rendering operation.
        imagery->render(*w);
    }
}

//----------------------------------------------------------------------------//
bool FalagardMenuItem::handleFontRenderSizeChange(const Font* const font)
{
    if (getContentNamedArea().handleFontRenderSizeChange(*d_window, font))
    {
        if (Window* const parent = d_window->getParent())
            parent->performChildWindowLayout();

        return true;
    }

    return false;
}

//----------------------------------------------------------------------------//

}


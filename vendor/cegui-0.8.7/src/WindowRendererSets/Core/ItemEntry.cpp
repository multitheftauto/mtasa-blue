/***********************************************************************
    created:    Thu Sep 22 2005
    author:     Tomas Lindquist Olsen
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
#include "CEGUI/WindowRendererSets/Core/ItemEntry.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardItemEntry::TypeName("Core/ItemEntry");

    FalagardItemEntry::FalagardItemEntry(const String& type) :
        ItemEntryWindowRenderer(type)
    {
    }

    void FalagardItemEntry::render()
    {
        ItemEntry* item = static_cast<ItemEntry*>(d_window);

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();

        const StateImagery* imagery;
        // render basic imagery
        String state = item->isEffectiveDisabled() ? "Disabled" : "Enabled";
        if (item->isSelectable() && item->isSelected())
        {
            imagery = &wlf.getStateImagery(item->isEffectiveDisabled() ? "SelectedDisabled" : "SelectedEnabled");
        }
        else
        {
            imagery = &wlf.getStateImagery(item->isEffectiveDisabled() ? "Disabled" : "Enabled");
        }
        imagery->render(*d_window);
    }

    Sizef FalagardItemEntry::getItemPixelSize() const
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        return wlf.getNamedArea("ContentSize").getArea().getPixelRect(*d_window).getSize();
    }

} // End of  CEGUI namespace section

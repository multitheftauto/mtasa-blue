/***********************************************************************
    created:    Tue Feb 28 2012
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/widgets/ToggleButton.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
const String ToggleButton::WidgetTypeName("CEGUI/ToggleButton");
const String ToggleButton::EventNamespace("ToggleButton");
const String ToggleButton::EventSelectStateChanged("SelectStateChanged");

//----------------------------------------------------------------------------//
ToggleButton::ToggleButton(const String& type, const String& name) :
    ButtonBase(type, name),
    d_selected(false)
{
    addToggleButtonProperties();
}

//----------------------------------------------------------------------------//
void ToggleButton::addToggleButtonProperties()
{
    const String& propertyOrigin(WidgetTypeName);
    
    CEGUI_DEFINE_PROPERTY(ToggleButton, bool,
        "Selected",
        "Property to access the selected state of the ToggleButton. "
        "Value is either \"true\" or \"false\".",
        &ToggleButton::setSelected, &ToggleButton::isSelected, false
    );
}

//----------------------------------------------------------------------------//
void ToggleButton::setSelected(bool select)
{
    if (d_selected == select)
        return;

    d_selected = select;
    invalidate();

    WindowEventArgs args(this);
    onSelectStateChange(args);
}

//----------------------------------------------------------------------------//
void ToggleButton::onSelectStateChange(WindowEventArgs& e)
{
    fireEvent(EventSelectStateChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void ToggleButton::onMouseButtonUp(MouseEventArgs& e)
{
    if (e.button == LeftButton && isPushed())
    {
        if (const Window* const sheet = getGUIContext().getRootWindow())
        {
            // was mouse released over this widget
            // (use mouse position, as e.position is already unprojected)
            if (this == sheet->getTargetChildAtPosition(
                    getGUIContext().getMouseCursor().getPosition()))
            {
                setSelected(getPostClickSelectState());
            }
        }

        ++e.handled;
    }

    ButtonBase::onMouseButtonUp(e);
}

//----------------------------------------------------------------------------//
bool ToggleButton::getPostClickSelectState() const
{
    return d_selected ^ true;
}

//----------------------------------------------------------------------------//

}


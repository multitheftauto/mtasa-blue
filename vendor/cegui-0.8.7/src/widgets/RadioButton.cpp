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
#include "CEGUI/widgets/RadioButton.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
const String RadioButton::WidgetTypeName("CEGUI/RadioButton");

//----------------------------------------------------------------------------//
RadioButton::RadioButton(const String& type, const String& name) :
    ToggleButton(type, name),
    d_groupID(0)
{
    addRadioButtonProperties();
}

//----------------------------------------------------------------------------//
void RadioButton::addRadioButtonProperties(void)
{
    const String& propertyOrigin(WidgetTypeName);

    CEGUI_DEFINE_PROPERTY(RadioButton, ulong,
        "GroupID",
        "Property to access the radio button group ID. "
        "Value is an unsigned integer number.",
        &RadioButton::setGroupID, &RadioButton::getGroupID, 0
    );
}

//----------------------------------------------------------------------------//
void RadioButton::setGroupID(ulong group)
{
    d_groupID = group;

    if (d_selected)
        deselectSiblingRadioButtonsInGroup();
}

//----------------------------------------------------------------------------//
void RadioButton::deselectSiblingRadioButtonsInGroup() const
{
    if (!d_parent)
        return;

    const size_t child_count = d_parent->getChildCount();
    for (size_t child = 0; child < child_count; ++child)
    {
        if (RadioButton* rb = dynamic_cast<RadioButton*>(
                getParent()->getChildAtIdx(child)))
        {
            if (rb->isSelected() && (rb != this) &&
                (rb->getGroupID() == d_groupID))
            {
                rb->setSelected(false);
            }
        }
    }
}

//----------------------------------------------------------------------------//
RadioButton* RadioButton::getSelectedButtonInGroup() const
{
    // Only search we we are a child window
    if (d_parent)
    {
        size_t child_count = d_parent->getChildCount();

        // scan all children
        for (size_t child = 0; child < child_count; ++child)
        {
            // is this child same type as we are?
            if (getParent()->getChildAtIdx(child)->getType() == getType())
            {
                RadioButton* rb = (RadioButton*)getParent()->getChildAtIdx(child);

                // is child same group and selected?
                if (rb->isSelected() && (rb->getGroupID() == d_groupID))
                {
                    // return the matching RadioButton pointer (may even be 'this').
                    return rb;
                }

            }

        }

    }

    // no selected button attached to this window is in same group
    return 0;
}

//----------------------------------------------------------------------------//
void RadioButton::onSelectStateChange(WindowEventArgs& e)
{
    if (d_selected)
        deselectSiblingRadioButtonsInGroup();

    ToggleButton::onSelectStateChange(e);
}

//----------------------------------------------------------------------------//
bool RadioButton::getPostClickSelectState() const
{
    return true;
}

//----------------------------------------------------------------------------//

}


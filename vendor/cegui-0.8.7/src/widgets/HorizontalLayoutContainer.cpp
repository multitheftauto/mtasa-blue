/***********************************************************************
    created:    29/7/2010
    author:     Martin Preisler
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/widgets/HorizontalLayoutContainer.h"
#include "CEGUI/CoordConverter.h"
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
    Constants
*************************************************************************/
// type name for this widget
const String HorizontalLayoutContainer::WidgetTypeName("HorizontalLayoutContainer");

/*************************************************************************
    Constructor
*************************************************************************/
HorizontalLayoutContainer::HorizontalLayoutContainer(const String& type,
                                                     const String& name) :
        SequentialLayoutContainer(type, name)
{}

//----------------------------------------------------------------------------//
HorizontalLayoutContainer::~HorizontalLayoutContainer(void)
{}

//----------------------------------------------------------------------------//
void HorizontalLayoutContainer::layout()
{
    // used to compare UDims
    const float absHeight = getChildContentArea().get().getHeight();

    // this is where we store the left offset
    // we continually increase this number as we go through the windows
    UDim leftOffset(0, 0);
    UDim layoutHeight(0, 0);

    for (ChildList::iterator it = d_children.begin(); it != d_children.end(); ++it)
    {
        Window* window = static_cast<Window*>(*it);

        const UVector2 offset = getOffsetForWindow(window);
        const UVector2 boundingSize = getBoundingSizeForWindow(window);

        // full child window width, including margins
        const UDim& childHeight = boundingSize.d_y;

        if (CoordConverter::asAbsolute(layoutHeight, absHeight) <
            CoordConverter::asAbsolute(childHeight, absHeight))
        {
            layoutHeight = childHeight;
        }

        window->setPosition(offset + UVector2(leftOffset, UDim(0, 0)));
        leftOffset += boundingSize.d_x;
    }

    setSize(USize(leftOffset, layoutHeight));
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section


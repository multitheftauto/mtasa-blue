/***********************************************************************
    created:    Tue Jul 5 2005
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
#include "CEGUI/WindowRendererSets/Core/ListHeaderSegment.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/widgets/ListHeaderSegment.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardListHeaderSegment::TypeName("Core/ListHeaderSegment");

    FalagardListHeaderSegment::FalagardListHeaderSegment(const String& type) :
        WindowRenderer(type)
    {
    }

    void FalagardListHeaderSegment::render()
    {
        ListHeaderSegment* w = (ListHeaderSegment*)d_window;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();

        const StateImagery* imagery;

        // get imagery for main state.
        if (w->isEffectiveDisabled())
        {
            imagery = &wlf.getStateImagery("Disabled");
        }
        else if ((w->isSegmentHovering() != w->isSegmentPushed()) && !w->isSplitterHovering() && w->isClickable())
        {
            imagery = &wlf.getStateImagery("Hover");
        }
        else if (w->isSplitterHovering())
        {
            imagery = &wlf.getStateImagery("SplitterHover");
        }
        else
        {
            imagery = &wlf.getStateImagery("Normal");
        }

        // do main rendering
        imagery->render(*w);

        // Render sorting icon as needed
        ListHeaderSegment::SortDirection sort_dir = w->getSortDirection();
        if (sort_dir == ListHeaderSegment::Ascending)
        {
            imagery = &wlf.getStateImagery("AscendingSortIcon");
            imagery->render(*w);
        }
        else if (sort_dir == ListHeaderSegment::Descending)
        {
            imagery = &wlf.getStateImagery("DescendingSortIcon");
            imagery->render(*w);
        }

        // draw ghost copy if the segment is being dragged.
        if (w->isBeingDragMoved())
        {
            Sizef pixel_size = w->getPixelSize();
            Rectf targetArea(0, 0, pixel_size.d_width, pixel_size.d_height);
            targetArea.offset(w->getDragMoveOffset());
            imagery = &wlf.getStateImagery("DragGhost");
            imagery->render(*w, targetArea);

            // Render sorting icon as needed
            if (sort_dir == ListHeaderSegment::Ascending)
            {
                imagery = &wlf.getStateImagery("GhostAscendingSortIcon");
                imagery->render(*w, targetArea);
            }
            else if (sort_dir == ListHeaderSegment::Descending)
            {
                imagery = &wlf.getStateImagery("GhostDescendingSortIcon");
                imagery->render(*w, targetArea);
            }
        }
    }

} // End of  CEGUI namespace section

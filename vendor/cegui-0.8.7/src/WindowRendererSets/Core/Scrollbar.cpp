/***********************************************************************
    created:    Mon Jul 4 2005
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
#include "CEGUI/WindowRendererSets/Core/Scrollbar.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/widgets/Thumb.h"
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/TplWindowRendererProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardScrollbar::TypeName("Core/Scrollbar");

    FalagardScrollbar::FalagardScrollbar(const String& type) :
        ScrollbarWindowRenderer(type),
        d_vertical(false)
    {
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardScrollbar, bool,
        "VerticalScrollbar", "Property to get/set whether the Scrollbar operates in the vertical direction."
        "  Value is either \"true\" or \"false\".",
        &FalagardScrollbar::setVertical, &FalagardScrollbar::isVertical,
        false);
    }

    void FalagardScrollbar::render()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*d_window);
    }

    void FalagardScrollbar::performChildWindowLayout()
    {
        updateThumb();
    }

    void FalagardScrollbar::updateThumb(void)
    {
        Scrollbar* w = (Scrollbar*)d_window;
        const WidgetLookFeel& wlf = getLookNFeel();
        Rectf area(wlf.getNamedArea("ThumbTrackArea").getArea().getPixelRect(*w));

        Thumb* theThumb = w->getThumb();

        float posExtent = w->getDocumentSize() - w->getPageSize();
        float slideExtent;

        if (d_vertical)
        {
            if(w->getPixelSize().d_height != 0.0f && posExtent != 0.0f)
            {
                slideExtent = area.getHeight() - theThumb->getPixelSize().d_height;
                theThumb->setVertRange(area.top() / w->getPixelSize().d_height, (area.top() + slideExtent) / w->getPixelSize().d_height);
                theThumb->setPosition(UVector2(cegui_absdim(area.left()),
                    cegui_reldim((area.top() + (w->getScrollPosition() * (slideExtent / posExtent))) / w->getPixelSize().d_height)));
            }
            else
            {
                theThumb->setVertRange(0.0f, 0.0f);
                theThumb->setPosition(UVector2(cegui_absdim(area.left()), cegui_reldim(0.0f)));
            }
        }
        else
        {
            if(w->getPixelSize().d_width != 0.0f && posExtent != 0.0f)
            {
                slideExtent = area.getWidth() - theThumb->getPixelSize().d_width;
                theThumb->setHorzRange(area.left() / w->getPixelSize().d_width, (area.left() + slideExtent)  / w->getPixelSize().d_width);
                theThumb->setPosition(UVector2(cegui_reldim((area.left() + (w->getScrollPosition() * (slideExtent / posExtent))) / w->getPixelSize().d_width),
                                                     cegui_absdim(area.top())));
            }
            else
            {
                theThumb->setHorzRange(0.0f, 0.0f);
                theThumb->setPosition(UVector2(cegui_reldim(0.0f), cegui_absdim(area.top())));
            }
        }
    }

    float FalagardScrollbar::getValueFromThumb(void) const
    {
        Scrollbar* w = (Scrollbar*)d_window;
        const WidgetLookFeel& wlf = getLookNFeel();
        const Rectf area(wlf.getNamedArea("ThumbTrackArea").getArea().getPixelRect(*w));

        Thumb* theThumb = w->getThumb();
        float posExtent = w->getDocumentSize() - w->getPageSize();

        if (d_vertical)
        {
            float slideExtent = area.getHeight() - theThumb->getPixelSize().d_height;
            return (CoordConverter::asAbsolute(theThumb->getYPosition(), w->getPixelSize().d_height) - area.top()) / (slideExtent / posExtent);
        }
        else
        {
            float slideExtent = area.getWidth() - theThumb->getPixelSize().d_width;
            return (CoordConverter::asAbsolute(theThumb->getXPosition(), w->getPixelSize().d_width) - area.left()) / (slideExtent / posExtent);
        }
    }

    float FalagardScrollbar::getAdjustDirectionFromPoint(const Vector2f& pt) const
    {
        Scrollbar* w = (Scrollbar*)d_window;
        const Rectf& absrect(w->getThumb()->getUnclippedOuterRect().get());

        if ((d_vertical && (pt.d_y > absrect.bottom())) ||
            (!d_vertical && (pt.d_x > absrect.right())))
        {
            return 1;
        }
        else if ((d_vertical && (pt.d_y < absrect.top())) ||
            (!d_vertical && (pt.d_x < absrect.left())))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    bool FalagardScrollbar::isVertical() const
    {
        return d_vertical;
    }

    void FalagardScrollbar::setVertical(bool setting)
    {
        d_vertical = setting;
    }

} // End of  CEGUI namespace section

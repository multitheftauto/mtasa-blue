/***********************************************************************
    created:    Sun Jul 3 2005
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
#include "CEGUI/WindowRendererSets/Core/Slider.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/widgets/Thumb.h"
#include "CEGUI/TplWindowRendererProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardSlider::TypeName("Core/Slider");

    FalagardSlider::FalagardSlider(const String& type) :
        SliderWindowRenderer(type),
        d_vertical(false),
        d_reversed(false)
    {
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardSlider,bool,
        "VerticalSlider", "Property to get/set whether the Slider operates in the vertical direction."
        "  Value is either \"true\" or \"false\".",
        &FalagardSlider::setVertical,&FalagardSlider::isVertical,
        false);
        CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(FalagardSlider,bool,
        "ReversedDirection", "Property to get/set whether the Slider operates in reversed direction."
        "  Value is either \"true\" or \"false\".",
        &FalagardSlider::setReversedDirection,&FalagardSlider::isReversedDirection,
        false);
    }

    void FalagardSlider::render()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(d_window->isEffectiveDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*d_window);
    }

    void FalagardSlider::performChildWindowLayout()
    {
        updateThumb();
    }

    void FalagardSlider::updateThumb(void)
    {
        Slider* w = (Slider*)d_window;
        // get area the thumb is supposed to use as it's area.
        const WidgetLookFeel& wlf = getLookNFeel();
        Rectf area(wlf.getNamedArea("ThumbTrackArea").getArea().getPixelRect(*w));
        // get accesss to the thumb
        Thumb* theThumb = w->getThumb();

        const Sizef w_pixel_size(w->getPixelSize());

        const float thumbRelXPos = w_pixel_size.d_width == 0.0f ? 0.0f : (area.left() / w_pixel_size.d_width);
        const float thumbRelYPos = w_pixel_size.d_height == 0.0f ? 0.0f : (area.top() / w_pixel_size.d_height);
        // get base location for thumb widget
        UVector2 thumbPosition(cegui_reldim(thumbRelXPos), cegui_reldim(thumbRelYPos));

        // Is this a vertical slider
        if (d_vertical)
        {
            // pixel extent of total available area the thumb moves in
            float slideExtent = area.getHeight() - theThumb->getPixelSize().d_height;

            // Set range of motion for the thumb widget
            if (w_pixel_size.d_height != 0.0f)
                theThumb->setVertRange(area.top()  / w_pixel_size.d_height,
                                      (area.top() + slideExtent) / w_pixel_size.d_height);
            else
                theThumb->setVertRange(0.0f, 0.0f);

            // calculate vertical positon for thumb
            float thumbOffset = w->getCurrentValue() * (slideExtent / w->getMaxValue());

            if (w_pixel_size.d_height != 0.0f)
                thumbPosition.d_y.d_scale +=
                    (d_reversed ? thumbOffset : slideExtent - thumbOffset) / w_pixel_size.d_height;
        }
        // Horizontal slider
        else
        {
            // pixel extent of total available area the thumb moves in
            float slideExtent = area.getWidth() - theThumb->getPixelSize().d_width;

            // Set range of motion for the thumb widget
            if (w_pixel_size.d_width != 0.0f)
                theThumb->setHorzRange(area.left() / w_pixel_size.d_width,
                                      (area.left() + slideExtent) / w_pixel_size.d_width);
            else
                theThumb->setHorzRange(0.0f, 0.0f);


            // calculate horizontal positon for thumb
            float thumbOffset = w->getCurrentValue() * (slideExtent / w->getMaxValue());

            if (w_pixel_size.d_width != 0.0f)
                thumbPosition.d_x.d_scale +=
                    (d_reversed ? slideExtent - thumbOffset : thumbOffset)  / w_pixel_size.d_width;
        }

        // set new position for thumb.
        theThumb->setPosition(thumbPosition);
    }

    float FalagardSlider::getValueFromThumb(void) const
    {
        Slider* w = (Slider*)d_window;
        // get area the thumb is supposed to use as it's area.
        const WidgetLookFeel& wlf = getLookNFeel();
        const Rectf area(wlf.getNamedArea("ThumbTrackArea").getArea().getPixelRect(*w));
        // get accesss to the thumb
        Thumb* theThumb = w->getThumb();

        // slider is vertical
        if (d_vertical)
        {
            // pixel extent of total available area the thumb moves in
            float slideExtent = area.getHeight() - theThumb->getPixelSize().d_height;
            // calculate value represented by current thumb position
            float thumbValue = (CoordConverter::asAbsolute(
                theThumb->getYPosition(), w->getPixelSize().d_height) - area.top()) / (slideExtent / w->getMaxValue());
            // return final thumb value according to slider settings
            return d_reversed ? thumbValue : w->getMaxValue() - thumbValue;
        }
        // slider is horizontal
        else
        {
            // pixel extent of total available area the thumb moves in
            float slideExtent = area.getWidth() - theThumb->getPixelSize().d_width;
            // calculate value represented by current thumb position
            float thumbValue = (CoordConverter::asAbsolute(
                theThumb->getXPosition(), w->getPixelSize().d_width) - area.left()) / (slideExtent / w->getMaxValue());
            // return final thumb value according to slider settings
            return d_reversed ? w->getMaxValue() - thumbValue : thumbValue;
        }
    }

    float FalagardSlider::getAdjustDirectionFromPoint(const Vector2f& pt) const
    {
        Slider* w = (Slider*)d_window;
        const Rectf absrect(w->getThumb()->getUnclippedOuterRect().get());

        if ((d_vertical && (pt.d_y < absrect.top())) ||
            (!d_vertical && (pt.d_x > absrect.right())))
        {
            return d_reversed ? -1.0f : 1.0f;
        }
        else if ((d_vertical && (pt.d_y > absrect.bottom())) ||
            (!d_vertical && (pt.d_x < absrect.left())))
        {
            return d_reversed ? 1.0f : -1.0f;
        }
        else
        {
            return 0;
        }
    }

    bool FalagardSlider::isVertical() const
    {
        return d_vertical;
    }

    void FalagardSlider::setVertical(bool setting)
    {
        d_vertical = setting;
    }

    bool FalagardSlider::isReversedDirection() const
    {
        return d_reversed;
    }

    void FalagardSlider::setReversedDirection(bool setting)
    {
        d_reversed = setting;
    }

} // End of  CEGUI namespace section

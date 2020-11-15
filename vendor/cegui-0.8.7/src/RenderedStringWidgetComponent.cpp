/***********************************************************************
    created:    27/05/2009
    author:     Paul Turner
 *************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/RenderedStringWidgetComponent.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Window.h"
#include "CEGUI/Image.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
RenderedStringWidgetComponent::RenderedStringWidgetComponent() :
    d_windowPtrSynched(true),
    d_window(0),
    d_selected(false)
{
}

//----------------------------------------------------------------------------//
RenderedStringWidgetComponent::RenderedStringWidgetComponent(
                                                    const String& widget_name) :
    d_windowName(widget_name),
    d_windowPtrSynched(false),
    d_window(0),
    d_selected(false)
{
}

//----------------------------------------------------------------------------//
RenderedStringWidgetComponent::RenderedStringWidgetComponent(Window* widget) :
    d_windowPtrSynched(true),
    d_window(widget),
    d_selected(false)
{
}

//----------------------------------------------------------------------------//
void RenderedStringWidgetComponent::setWindow(const String& widget_name)
{
    d_windowName = widget_name;
    d_windowPtrSynched = false;
    d_window = 0;
}

//----------------------------------------------------------------------------//
void RenderedStringWidgetComponent::setWindow(Window* widget)
{
    d_window = widget;
    d_windowPtrSynched = true;
}

//----------------------------------------------------------------------------//
const Window* RenderedStringWidgetComponent::getWindow() const
{
    return getEffectiveWindow(0); // FIXME: Perhaps?
}

//----------------------------------------------------------------------------//
void RenderedStringWidgetComponent::setSelection(const Window* /*ref_wnd*/,
                                                 const float start,
                                                 const float end)
{
    d_selected = (start != end);
}

//----------------------------------------------------------------------------//
void RenderedStringWidgetComponent::draw(const Window* ref_wnd,
                                         GeometryBuffer& buffer,
                                         const Vector2f& position,
                                         const CEGUI::ColourRect* /*mod_colours*/,
                                         const Rectf* clip_rect,
                                         const float vertical_space,
                                         const float /*space_extra*/) const
{
    Window* const window = getEffectiveWindow(ref_wnd);

    if (!window)
        return;

    // HACK: re-adjust for inner-rect of parent
    float x_adj = 0, y_adj = 0;
    Window* parent = window->getParent();
    
    if (parent)
    {
        const Rectf& outer(parent->getUnclippedOuterRect().get());
        const Rectf& inner(parent->getUnclippedInnerRect().get());
        x_adj = inner.d_min.d_x - outer.d_min.d_x;
        y_adj = inner.d_min.d_y - outer.d_min.d_y;
    }
    // HACK: re-adjust for inner-rect of parent (Ends)

    Vector2f final_pos(position);
    // handle formatting options
    switch (d_verticalFormatting)
    {
    case VF_BOTTOM_ALIGNED:
        final_pos.d_y += vertical_space - getPixelSize(ref_wnd).d_height;
        break;

    case VF_STRETCHED:
        Logger::getSingleton().logEvent("RenderedStringWidgetComponent::draw: "
            "VF_STRETCHED specified but is unsupported for Widget types; "
            "defaulting to VF_CENTRE_ALIGNED instead.");
        
        // intentional fall-through.
        
    case VF_CENTRE_ALIGNED:
        final_pos.d_y += (vertical_space - getPixelSize(ref_wnd).d_height) / 2 ;
        break;


    case VF_TOP_ALIGNED:
        // nothing additional to do for this formatting option.
        break;

    default:
        CEGUI_THROW(InvalidRequestException(
                "unknown VerticalFormatting option specified."));
    }

    // render the selection if needed
    if (d_selectionImage && d_selected)
    {
        const Rectf select_area(position, getPixelSize(ref_wnd));
        d_selectionImage->render(buffer, select_area, clip_rect, ColourRect(0xFF002FFF));
    }

    // we do not actually draw the widget, we just move it into position.
    const UVector2 wpos(UDim(0, final_pos.d_x + d_padding.d_min.d_x - x_adj),
                        UDim(0, final_pos.d_y + d_padding.d_min.d_y - y_adj));

    window->setPosition(wpos);
}

//----------------------------------------------------------------------------//
Window* RenderedStringWidgetComponent::getEffectiveWindow(
                                                const Window* ref_wnd) const
{
    if (!d_windowPtrSynched)
    {
        if (!ref_wnd)
            return 0;

        d_window = ref_wnd->getChild(d_windowName);
        d_windowPtrSynched = true;
    }
    
    return d_window;
}

//----------------------------------------------------------------------------//
Sizef RenderedStringWidgetComponent::getPixelSize(const Window* ref_wnd) const
{
    Sizef sz(0, 0);

    if (Window* const window = getEffectiveWindow(ref_wnd))
    {
        sz = window->getPixelSize();
        sz.d_width += (d_padding.d_min.d_x + d_padding.d_max.d_x);
        sz.d_height += (d_padding.d_min.d_y + d_padding.d_max.d_y);
    }

    return sz;
}

//----------------------------------------------------------------------------//
bool RenderedStringWidgetComponent::canSplit() const
{
    return false;
}

//----------------------------------------------------------------------------//
RenderedStringWidgetComponent* RenderedStringWidgetComponent::split(
    const Window* /*ref_wnd*/, float /*split_point*/, bool /*first_component*/)
{
    CEGUI_THROW(InvalidRequestException(
        "this component does not support being split."));
}

//----------------------------------------------------------------------------//
RenderedStringWidgetComponent* RenderedStringWidgetComponent::clone() const
{
    return CEGUI_NEW_AO RenderedStringWidgetComponent(*this);
}

//----------------------------------------------------------------------------//
size_t RenderedStringWidgetComponent::getSpaceCount() const
{
    // widgets do not have spaces
    return 0;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

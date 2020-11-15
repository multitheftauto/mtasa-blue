/***********************************************************************
    created:    24/05/2009
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
#include "CEGUI/RenderedStringImageComponent.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

//----------------------------------------------------------------------------//
RenderedStringImageComponent::RenderedStringImageComponent() :
    d_image(0),
    d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
    d_size(0, 0),
    d_selected(false)
{
}

//----------------------------------------------------------------------------//
RenderedStringImageComponent::RenderedStringImageComponent(const String& name) :
    d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
    d_size(0, 0),
    d_selected(false)
{
    setImage(name);
}

//----------------------------------------------------------------------------//
RenderedStringImageComponent::RenderedStringImageComponent(const Image* image) :
    d_image(image),
    d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
    d_size(0, 0),
    d_selected(false)
{
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setImage(const String& name)
{
    if (!name.empty())
    {
        d_image = &ImageManager::getSingleton().get(name);
    }
    else
    {
        d_image = 0;
    }
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setImage(const Image* image)
{
    d_image = image;
}

//----------------------------------------------------------------------------//
const Image* RenderedStringImageComponent::getImage() const
{
    return d_image;
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setColours(const ColourRect& cr)
{
    d_colours = cr;
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setColours(const Colour& c)
{
    d_colours.setColours(c);
}

//----------------------------------------------------------------------------//
const ColourRect& RenderedStringImageComponent::getColours() const
{
    return d_colours;
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setSelection(const Window* /*ref_wnd*/,
                                                const float start, const float end)
{
    d_selected = (start != end);
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::draw(const Window* ref_wnd,
                                        GeometryBuffer& buffer,
                                        const Vector2f& position,
                                        const ColourRect* mod_colours,
                                        const Rectf* clip_rect,
                                        const float vertical_space,
                                        const float /*space_extra*/) const
{
    if (!d_image)
        return;

    CEGUI::Rectf dest(position.d_x, position.d_y, 0, 0);
    float y_scale = 1.0f;

    // handle formatting options
    switch (d_verticalFormatting)
    {
    case VF_BOTTOM_ALIGNED:
        dest.d_min.d_y += vertical_space - getPixelSize(ref_wnd).d_height;
        break;

    case VF_CENTRE_ALIGNED:
        dest.d_min.d_y += (vertical_space - getPixelSize(ref_wnd).d_height) / 2 ;
        break;

    case VF_STRETCHED:
        y_scale = vertical_space / getPixelSize(ref_wnd).d_height;
        break;

    case VF_TOP_ALIGNED:
        // nothing additional to do for this formatting option.
        break;

    default:
        CEGUI_THROW(InvalidRequestException(
            "unknown VerticalFormatting option specified."));
    }

    Sizef sz(d_image->getRenderedSize());
    if (d_size.d_width != 0.0)
        sz.d_width = d_size.d_width;
    if (d_size.d_height != 0.0)
        sz.d_height = d_size.d_height;
    
    sz.d_height *= y_scale;
    dest.setSize(sz);

    // apply padding to position
    dest.offset(d_padding.getPosition());

    // render the selection if needed
    if (d_selectionImage && d_selected)
    {
        const Rectf select_area(position, getPixelSize(ref_wnd));
        d_selectionImage->render(buffer, select_area, clip_rect, ColourRect(0xFF002FFF));
    }

    // apply modulative colours if needed.
    ColourRect final_cols(d_colours);
    if (mod_colours)
        final_cols *= *mod_colours;

    // draw the image.
    d_image->render(buffer, dest, clip_rect, final_cols);
}

//----------------------------------------------------------------------------//
Sizef RenderedStringImageComponent::getPixelSize(const Window* /*ref_wnd*/) const
{
    Sizef sz(0, 0);

    if (d_image)
    {
        sz = d_image->getRenderedSize();
        if (d_size.d_width != 0.0)
            sz.d_width = d_size.d_width;
        if (d_size.d_height != 0.0)
            sz.d_height = d_size.d_height;
        sz.d_width += (d_padding.d_min.d_x + d_padding.d_max.d_x);
        sz.d_height += (d_padding.d_min.d_y + d_padding.d_max.d_y);
    }

    return sz;
}

//----------------------------------------------------------------------------//
bool RenderedStringImageComponent::canSplit() const
{
    return false;
}

//----------------------------------------------------------------------------//
RenderedStringImageComponent* RenderedStringImageComponent::split(
    const Window* /*ref_wnd*/ ,float /*split_point*/, bool /*first_component*/)
{
    CEGUI_THROW(InvalidRequestException(
        "this component does not support being split."));
}

//----------------------------------------------------------------------------//
RenderedStringImageComponent* RenderedStringImageComponent::clone() const
{
    return CEGUI_NEW_AO RenderedStringImageComponent(*this);
}

//----------------------------------------------------------------------------//
size_t RenderedStringImageComponent::getSpaceCount() const
{
    // images do not have spaces.
    return 0;
}

//----------------------------------------------------------------------------//
void RenderedStringImageComponent::setSize(const Sizef& sz)
{
    d_size = sz;
}

//----------------------------------------------------------------------------//
const Sizef& RenderedStringImageComponent::getSize() const
{
    return d_size;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

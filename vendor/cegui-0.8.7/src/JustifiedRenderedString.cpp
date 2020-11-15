/***********************************************************************
    created:    Mon Jul 6 2009
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/JustifiedRenderedString.h"
#include "CEGUI/RenderedString.h"
#include "CEGUI/Vector.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
JustifiedRenderedString::JustifiedRenderedString(const RenderedString& string) :
    FormattedRenderedString(string)
{
}

//----------------------------------------------------------------------------//
void JustifiedRenderedString::format(const Window* ref_wnd,
                                     const Sizef& area_size)
{
    d_spaceExtras.clear();

    for (size_t i = 0; i < d_renderedString->getLineCount(); ++i)
    {
        const size_t space_count = d_renderedString->getSpaceCount(i);
        const float string_width = d_renderedString->getPixelSize(
                                                        ref_wnd, i).d_width;

        if ((space_count == 0) || (string_width >= area_size.d_width))
            d_spaceExtras.push_back(0.0f);
        else
            d_spaceExtras.push_back(
                (area_size.d_width - string_width) / space_count);
    }
}

//----------------------------------------------------------------------------//
void JustifiedRenderedString::draw(const Window* ref_wnd, GeometryBuffer& buffer,
                                   const Vector2f& position,
                                   const ColourRect* mod_colours,
                                   const Rectf* clip_rect) const
{
    Vector2f draw_pos(position);

    for (size_t i = 0; i < d_renderedString->getLineCount(); ++i)
    {
        d_renderedString->draw(ref_wnd, i, buffer, draw_pos, mod_colours,
                               clip_rect, d_spaceExtras[i]);
        draw_pos.d_y += d_renderedString->getPixelSize(ref_wnd, i).d_height;
    }
}

//----------------------------------------------------------------------------//
size_t JustifiedRenderedString::getFormattedLineCount() const
{
    return d_renderedString->getLineCount();
}

//----------------------------------------------------------------------------//
float JustifiedRenderedString::getHorizontalExtent(const Window* ref_wnd) const
{
    float w = 0.0f;
    for (size_t i = 0; i < d_renderedString->getLineCount(); ++i)
    {
        const float this_width = d_renderedString->getPixelSize(ref_wnd, i).d_width +
            d_renderedString->getSpaceCount(i) * d_spaceExtras[i];

        if (this_width > w)
            w = this_width;
    }

    return w;
}

//----------------------------------------------------------------------------//
float JustifiedRenderedString::getVerticalExtent(const Window* ref_wnd) const
{
    float h = 0.0f;
    for (size_t i = 0; i < d_renderedString->getLineCount(); ++i)
        h += d_renderedString->getPixelSize(ref_wnd, i).d_height;

    return h;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

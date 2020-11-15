/***********************************************************************
    created:    06/07/2009
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
#include "CEGUI/RenderedStringWordWrapper.h"
#include "CEGUI/JustifiedRenderedString.h"
#include "CEGUI/LeftAlignedRenderedString.h"

// The purpose of this file is to define a specialised RenderedStringWordWrapper
// for Justified formatting so that the last line is handled correctly.

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
template <>
void RenderedStringWordWrapper<JustifiedRenderedString>::format(
                                                        const Window* ref_wnd,
                                                        const Sizef& area_size)
{
    deleteFormatters();

    RenderedString rstring, lstring;
    rstring = *d_renderedString;
    float rs_width;

    FormattedRenderedString* frs;

    for (size_t line = 0; line < rstring.getLineCount(); ++line)
    {
        while ((rs_width = rstring.getPixelSize(ref_wnd, line).d_width) > 0)
        {
            // skip line if no wrapping occurs
            if (rs_width <= area_size.d_width)
                break;

            // split rstring at width into lstring and remaining rstring
            rstring.split(ref_wnd, line, area_size.d_width, lstring);
            frs = CEGUI_NEW_AO JustifiedRenderedString(*CEGUI_NEW_AO RenderedString(lstring));
            frs->format(ref_wnd, area_size);
            d_lines.push_back(frs);
            line = 0;
        }
    }

    // last line (which we do not justify)
    frs = CEGUI_NEW_AO LeftAlignedRenderedString(*CEGUI_NEW_AO RenderedString(rstring));
    frs->format(ref_wnd, area_size);
    d_lines.push_back(frs);
}

//----------------------------------------------------------------------------//
    
} // End of  CEGUI namespace section

/***********************************************************************
    created:    Sat Jun 11 2011
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/Image.h"
#include "CEGUI/Logger.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{

//----------------------------------------------------------------------------//
Image::~Image()
{
}

//----------------------------------------------------------------------------//
void Image::computeScalingFactors(AutoScaledMode mode,
                                  const Sizef& display_size,
                                  const Sizef& native_display_size,
                                  float& x_scale,
                                  float& y_scale)
{
    if (mode == ASM_Disabled)
    {
        x_scale = 1.0f;
        y_scale = 1.0f;
    }
    else if (mode == ASM_Vertical)
    {
        x_scale = display_size.d_height / native_display_size.d_height;
        y_scale = x_scale;
    }
    else if (mode == ASM_Horizontal)
    {
        x_scale = display_size.d_width / native_display_size.d_width;
        y_scale = x_scale;
    }
    else if (mode == ASM_Min)
    {
        x_scale = std::min(display_size.d_width / native_display_size.d_width,
                           display_size.d_height / native_display_size.d_height);
        y_scale = x_scale;
    }
    else if (mode == ASM_Max)
    {
        x_scale = std::max(display_size.d_width / native_display_size.d_width,
                           display_size.d_height / native_display_size.d_height);
        y_scale = x_scale;
    }
    else if (mode == ASM_Both)
    {
        x_scale = display_size.d_width / native_display_size.d_width;
        y_scale = display_size.d_height / native_display_size.d_height;
    }
    else
    {
        assert(false && "Invalid AutoScaledMode");
    }
}

//----------------------------------------------------------------------------//
void Image::elementStartLocal(const String& element,
                              const XMLAttributes& /*attributes*/)
{
     Logger::getSingleton().logEvent(
        "    [Image] Unknown XML tag encountered: " + element);
}

//----------------------------------------------------------------------------//
void Image::elementEndLocal(const String& element)
{
    if (element == "Image")
        d_completed = true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section


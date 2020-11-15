/***********************************************************************
    created:    20th February 2010
    author:     Lukas E Meindl

    purpose:    Implementation of the ColourPicker colour type classes
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
#include "CEGUI/CommonDialogs/ColourPicker/Types.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

using namespace CEGUI;

//----------------------------------------------------------------------------//
RGB_Colour::RGB_Colour(const Lab_Colour& colour)
{
    *this = ColourPickerConversions::toRGB(colour);
}

//----------------------------------------------------------------------------//
RGB_Colour::RGB_Colour(const HSV_Colour& colour)
{
    *this = ColourPickerConversions::toRGB(colour);
}

//----------------------------------------------------------------------------//
RGB_Colour::RGB_Colour(const CEGUI::Colour& colour)
{
    *this = ColourPickerConversions::toRGB(colour);
}


//----------------------------------------------------------------------------//
Lab_Colour::Lab_Colour(const RGB_Colour& colour)
{
    *this = ColourPickerConversions::toLab(colour);
}

//----------------------------------------------------------------------------//
Lab_Colour::Lab_Colour(const HSV_Colour& colour)
{
    *this = ColourPickerConversions::toLab(ColourPickerConversions::toRGB(colour));
}


//----------------------------------------------------------------------------//
Lab_Colour::Lab_Colour(const CEGUI::Colour& colour)
{
    *this = ColourPickerConversions::toLab(ColourPickerConversions::toRGB(colour));
}

//----------------------------------------------------------------------------//
HSV_Colour::HSV_Colour(const RGB_Colour& colour)
{
    *this = ColourPickerConversions::toHSV(colour);
}

//----------------------------------------------------------------------------//
HSV_Colour::HSV_Colour(const Lab_Colour& colour)
{
    *this = ColourPickerConversions::toHSV(ColourPickerConversions::toRGB(colour));
}

//----------------------------------------------------------------------------//
HSV_Colour::HSV_Colour(const CEGUI::Colour& colour)
{
    *this = ColourPickerConversions::toHSV(ColourPickerConversions::toRGB(colour));
}



//----------------------------------------------------------------------------//
RGB_Colour RGB_Colour::operator*(const float& number) const
{
    RGB_Colour col;

    col.r = static_cast<unsigned char>(r * number);
    col.g = static_cast<unsigned char>(g * number);
    col.b = static_cast<unsigned char>(b * number);

    return col;
}

//----------------------------------------------------------------------------//
RGB_Colour RGB_Colour::operator+(const RGB_Colour& colour) const
{
    RGB_Colour col;

    col.r = static_cast<int>(r) + static_cast<int>(colour.r);
    col.g = static_cast<int>(g) + static_cast<int>(colour.g);
    col.b = static_cast<int>(b) + static_cast<int>(colour.b);

    return col;
}

//----------------------------------------------------------------------------//


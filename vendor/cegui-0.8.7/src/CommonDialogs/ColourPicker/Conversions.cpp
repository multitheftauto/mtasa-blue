/***********************************************************************
    created:    20th February 2010
    author:     Lukas E Meindl
    
    purpose:    Implementation of the class used to provide conversions
                between the ColourPicker colour types
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
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"
#include <math.h>
#include <algorithm>

namespace CEGUI
{

//----------------------------------------------------------------------------//
const float ColourPickerConversions::Xn(0.95047f);
const float ColourPickerConversions::Yn(1.00000f);
const float ColourPickerConversions::Zn(1.08883f);

const float ColourPickerConversions::LAB_COMPARE_VALUE_CONST(0.00885645167903563081717167575546f);

//----------------------------------------------------------------------------//
void ColourPickerConversions::clamp(float& value, float min_val, float max_val)
{
    value = value < min_val ? min_val : (value > max_val ? max_val : value);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toX(unsigned char R, unsigned char G, unsigned char B)
{
    return toX(R / 255.0f, G / 255.0f, B / 255.0f);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toY(unsigned char R, unsigned char G, unsigned char B)
{
    return toY(R / 255.0f, G / 255.0f, B / 255.0f);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toZ(unsigned char R, unsigned char G, unsigned char B)
{
    return toZ(R / 255.0f, G / 255.0f, B / 255.0f);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toX(float R, float G, float B)
{
    return (0.4124564f * R + 0.3575761f * G + 0.1804375f * B);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toY(float R, float G, float B)
{
    return (0.2126729f * R + 0.7151522f * G + 0.0721750f * B);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toZ(float R, float G, float B)
{
    return (0.0193339f * R + 0.1191920f * G + 0.9503041f * B);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toL(float Y)
{
    return (116.0f * YNormCalc(Y) - 16.0f);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toA(float X, float Y)
{
    return (500.0f * (XNormCalc(X) - YNormCalc(Y)));
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::toB(float Y, float Z)
{
    return (200.0f * (YNormCalc(Y) - ZNormCalc(Z)));
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::XNormCalc(float X)
{
    float div = (X / Xn);
    return normCalc(div);

}

//----------------------------------------------------------------------------//
float ColourPickerConversions::YNormCalc(float Y)
{
    float div = (Y / Yn);
    return normCalc(div);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::ZNormCalc(float Z)
{
    float div = (Z / Zn);
    return normCalc(div);
}

//----------------------------------------------------------------------------//
float ColourPickerConversions::normCalc(float div)
{
    if (div < LAB_COMPARE_VALUE_CONST)
        return ((24389.0f / 27.0f * div + 16.0f) / 116.0f);
    else
        return pow(div, 1.0f / 3.0f);
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerConversions::toRGB(const Lab_Colour& colour)
{
    return toRGB(colour.L, colour.a, colour.b);
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerConversions::toRGB(float L, float a, float b)
{
    float vy = (L + 16.0f) / 116.0f;
    float vx = a / 500.0f + vy;
    float vz = vy - b / 200.0f;

    {
        float vx3 = pow(vx, 3);
        float vy3 = pow(vy, 3);
        float vz3 = pow(vz, 3);

        if (vy3 > LAB_COMPARE_VALUE_CONST)
            vy = vy3;
        else
            vy = (vy - 16.0f / 116.0f) / 7.787f;

        if (vx3 > LAB_COMPARE_VALUE_CONST)
            vx = vx3;
        else
            vx = (vx - 16.0f / 116.0f) / 7.787f;

        if (vz3 > LAB_COMPARE_VALUE_CONST)
            vz = vz3;
        else
            vz = (vz - 16.0f / 116.0f) / 7.787f;
    }

    vx *= Xn;
    vz *= Zn;

    float vr = vx *  3.2406f + vy * -1.5372f + vz * -0.4986f;
    float vg = vx * -0.9689f + vy *  1.8758f + vz *  0.0415f;
    float vb = vx *  0.0557f + vy * -0.2040f + vz *  1.0570f;

    clamp(vr, 0.0f, 1.0f);
    clamp(vg, 0.0f, 1.0f);
    clamp(vb, 0.0f, 1.0f);

    return RGB_Colour(static_cast<unsigned char>(255.0f * vr),
                      static_cast<unsigned char>(255.0f * vg),
                      static_cast<unsigned char>(255.0f * vb));
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerConversions::toRGB(const CEGUI::Colour& colour)
{
    return RGB_Colour(static_cast<unsigned char>(colour.getRed() * 255.0f),
                      static_cast<unsigned char>(colour.getGreen() * 255.0f),
                      static_cast<unsigned char>(colour.getBlue() * 255.0f));
}

//----------------------------------------------------------------------------//
CEGUI::Colour ColourPickerConversions::toCeguiColour(const RGB_Colour& colourRGB)
{
    return CEGUI::Colour(colourRGB.r / 255.0f,
                         colourRGB.g / 255.0f,
                         colourRGB.b / 255.0f);
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerConversions::linearInterpolationRGB(float interPolBalance,
        const RGB_Colour& start,
        const RGB_Colour& end)
{
    RGB_Colour colour;

    clampInterpolationValue(interPolBalance);

    colour.r = static_cast<unsigned char>(
                   (1 - interPolBalance) * start.r + (interPolBalance * end.r));
    colour.g = static_cast<unsigned char>(
                   (1 - interPolBalance) * start.g + (interPolBalance * end.g));
    colour.b = static_cast<unsigned char>(
                   (1 - interPolBalance) * start.b + (interPolBalance * end.b));

    return colour;
}

//----------------------------------------------------------------------------//
Lab_Colour ColourPickerConversions::linearInterpolationLab(float interPolBalance,
        const Lab_Colour& start,
        const Lab_Colour& end)
{
    Lab_Colour colour;

    clampInterpolationValue(interPolBalance);

    colour.L = (1 - interPolBalance) * start.L + (interPolBalance * end.L);
    colour.a = (1 - interPolBalance) * start.a + (interPolBalance * end.a);
    colour.b = (1 - interPolBalance) * start.b + (interPolBalance * end.b);

    return colour;
}

//----------------------------------------------------------------------------//
HSV_Colour ColourPickerConversions::linearInterpolationHSV(float interPolBalance,
        const HSV_Colour& start,
        const HSV_Colour& end)
{
    HSV_Colour colour;

    clampInterpolationValue(interPolBalance);

    colour.H = (1 - interPolBalance) * start.H + (interPolBalance * end.H);
    colour.S = (1 - interPolBalance) * start.S + (interPolBalance * end.S);
    colour.V = (1 - interPolBalance) * start.V + (interPolBalance * end.V);

    return colour;
}

//----------------------------------------------------------------------------//
unsigned char ColourPickerConversions::linearInterpolationAlpha(float interPolBalance,
        unsigned char startAlpha,
        unsigned char endAlpha)
{
    clampInterpolationValue(interPolBalance);

    return static_cast<unsigned char>(
               (1 - interPolBalance) * startAlpha + (interPolBalance * endAlpha));
}

//----------------------------------------------------------------------------//
void ColourPickerConversions::clampInterpolationValue(float& interPolBalance)
{
    interPolBalance = std::max(0.0f, interPolBalance);
    interPolBalance = std::min(1.0f, interPolBalance);
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerConversions::toRGB(const HSV_Colour& colour)
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    int i = (int)(colour.H * 6.0f);
    float f = colour.H * 6 - i;
    float p = colour.V * (1 - colour.S);
    float q = colour.V * (1 - f * colour.S);
    float t = colour.V * (1 - (1 - f) * colour.S);

    switch (i % 6)
    {
    case 0:
        r = colour.V;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = colour.V;
        b = p;
        break;

    case 2:
        r = p;
        g = colour.V;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = colour.V;
        break;

    case 4:
        r = t;
        g = p;
        b = colour.V;
        break;

    case 5:
        r = colour.V;
        g = p;
        b = q;
        break;
    }

    return RGB_Colour(static_cast<unsigned char>(r * 255),
                      static_cast<unsigned char>(g * 255),
                      static_cast<unsigned char>(b * 255));
}
//----------------------------------------------------------------------------//

Lab_Colour ColourPickerConversions::toLab(RGB_Colour colour)
{
    float X = toX(colour.r, colour.g, colour.b);
    float Y = toY(colour.r, colour.g, colour.b);
    float Z = toZ(colour.r, colour.g, colour.b);

    float L = toL(Y);
    float a = toA(X, Y);
    float b = toB(Y, Z);

    return Lab_Colour(L, a, b);
}

//----------------------------------------------------------------------------//
HSV_Colour ColourPickerConversions::toHSV(RGB_Colour colour)
{
    float r = colour.r / 255.0f;
    float g = colour.g / 255.0f;
    float b = colour.b / 255.0f;

    bool maxCompRed = false;
    bool maxCompGreen = false;
    float max_comp = b;

    if(r > g && r > b)
    {
        maxCompRed = true;
        max_comp = r;
    }
    else if(g > b)
    {
        maxCompGreen = true;
        max_comp = g;
    }

    float min_comp = ceguimin(ceguimin(r, g), b);
    float h;
    float s;
    float v = max_comp;

    float diff = max_comp - min_comp;
    s = (max_comp == 0.0f ? 0.0f : diff / max_comp);

    if (max_comp == min_comp)
    {
        h = 0; // achromatic
    }
    else
    {
        if (maxCompRed)
            h = (g - b) / diff + (g < b ? 6.0f : 0.0f);

        if (maxCompGreen)
            h = (b - r) / diff + 2.0f;

        else
            h = (r - g) / diff + 4.0f;

        h /= 6.0f;
    }

    return HSV_Colour(h, s, v);
}

//----------------------------------------------------------------------------//

}


/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto: San Andreas
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CColor
{
public:
    CColor() { R = G = B = A = 255; }
    CColor(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A = 255)
    {
        R = _R;
        G = _G;
        B = _B;
        A = _A;
    }
    CColor(const CColor& other) { *this = other; }
    CColor(unsigned long ulColor) { *this = ulColor; }
    CColor& operator=(const CColor& color)
    {
        R = color.R;
        G = color.G;
        B = color.B;
        A = color.A;
        return *this;
    }
    CColor& operator=(unsigned long ulColor)
    {
        R = (ulColor >> 16) & 0xFF;
        G = (ulColor >> 8) & 0xFF;
        B = (ulColor)&0xFF;
        return *this;
    }
    bool operator==(const CColor& other) const { return R == other.R && G == other.G && B == other.B && A == other.A; }

    unsigned char R, G, B, A;
};

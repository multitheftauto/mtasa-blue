/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CRect.h
 *  PURPOSE:     This is an actual GTA SA class. MTA has its own version of
 *               CRect, and it's called CRect2D. CRect behaves similarly to
 *               CRect2D, except it's used by GTA:SA, and the initial values
 *               are NOT zero when constructed.
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector2D.h"

class CRect
{
public:
    float left;              // x1
    float bottom;            // y1
    float right;             // x2
    float top;               // y2

    CRect()
    {
        left = 1000000.0f;
        top = 1000000.0f;
        right = -1000000.0f;
        bottom = -1000000.0f;
    }

    CRect(float left, float top, float right, float bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    void Restrict(CRect* rect)
    {
        if (rect->left < left)
            left = rect->left;
        if (rect->right > right)
            right = rect->right;
        if (rect->top < top)
            top = rect->top;
        if (rect->bottom > bottom)
            bottom = rect->bottom;
    }

    void StretchToPoint(float x, float y)
    {
        if (x < left)
            left = x;
        if (x > right)
            right = x;
        if (y < top)
            top = y;
        if (y > bottom)
            bottom = y;
    }

    bool IsCircleInside(CVector2D* point, float radius)
    {
        return left - radius <= point->fX && radius + right >= point->fX && top - radius <= point->fY && radius + bottom >= point->fY;
    }

    void Reset()
    {
        left = 0.0f;
        bottom = 0.0f;
        right = 0.0f;
        top = 0.0f;
    }

    // Fix #1613: custom function. Used for fixing custom map collision crashes in CPhysical class (infinite loop)
    // The value must be greater than or equal to 1.0f because let 's say if the value of left is 15.5,
    // when it' s casted to int, it will be rounded off to 15, so we need to subtract at least 0.6 to make make it
    // 14 when it 's casted to int . It doesn't really matter which value we subtract because the bounding box
    // values are already corrupted at this point, and we're just trying to avert the crash by making sure
    // the top and left values are always less than the bottom and right values.
    void FixIncorrectTopLeft()
    {
        if (left > right) left = right - 1.0f;
        if (top > bottom) top = bottom - 1.0f;
    }
};

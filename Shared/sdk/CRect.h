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

    // custom function. Used for fixing custom map collision crashes in CPhysical class (infinite loop)
    void FixIncorrectTopLeft()
    {
        if (left > right) left = right - 1.0f;
        if (top > bottom) top = bottom - 1.0f;
    }
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"

class CColor;

class CGUILabel : public CGUIElement
{
public:
    virtual ~CGUILabel(){};

    virtual CVector2D GetTextSize() = 0;
    virtual float     GetTextExtent() = 0;

    virtual CColor GetTextColor() = 0;
    virtual void   SetTextColor(CColor color) = 0;
    virtual void   SetTextColor(int r, int g, int b, int a = 255) = 0;

    virtual CGUITextAlignHorizontal GetTextHorizontalAlign() = 0;
    virtual void                    SetTextHorizontalAlign(CGUITextAlignHorizontal align) = 0;

    virtual CGUITextAlignVertical GetTextVerticalAlign() = 0;
    virtual void                  SetTextVerticalAlign(CGUITextAlignVertical align) = 0;
};

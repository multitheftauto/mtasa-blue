/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUILabel.h>
#include "CGUIElement_Impl.h"

class CGUILabel_Impl : public CGUILabel, public CGUIElement_Impl
{
public:
    CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

    CVector2D GetTextSize();
    float     GetTextExtent();

    CColor GetTextColor();
    void   SetTextColor(CColor color);
    void   SetTextColor(int r, int g, int b, int a = 255);

    CGUITextAlignHorizontal GetTextHorizontalAlign();
    void                    SetTextHorizontalAlign(CGUITextAlignHorizontal align);

    CGUITextAlignVertical GetTextVerticalAlign();
    void                  SetTextVerticalAlign(CGUITextAlignVertical align);

#include "CGUIElement_Inc.h"
private:
    CColor m_color = {255, 255, 255, 255};

    std::pair<CGUITextAlignHorizontal, CGUITextAlignVertical> m_textAlign = {CGUITextAlignHorizontal::LEFT, CGUITextAlignVertical::TOP};
};

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

class CGUILabel : public CGUIElement
{
public:
    virtual ~CGUILabel(){};

    virtual CVector2D GetTextSize() = 0;
    virtual float       GetTextExtent() = 0;
};

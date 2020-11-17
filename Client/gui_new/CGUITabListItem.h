/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabListItem.h
 *  PURPOSE:     Tab-able Element Interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <gui/CGUI.h>

namespace GUINew{
    class CGUITabListItem;
}

class GUINew::CGUITabListItem : public CGUIElement
{
public:
    virtual bool ActivateOnTab() = 0;
};

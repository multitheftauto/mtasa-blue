/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabListItem.h
 *  PURPOSE:     Tab-able Element Interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <gui/CGUI.h>

class CGUITabListItem : public CGUIElement
{
public:
    virtual bool ActivateOnTab() = 0;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDebugView.h
 *  PURPOSE:     Header file for debug view class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUI.h"
#include <gui/CGUI.h>
#include "CChat.h"

#define DEBUGVIEW_WIDTH         576                             // Debugview default width
#define DEBUGVIEW_TEXT_COLOR    CColor( 235, 221, 178, 255 )    // Debugview default text color

class CDebugView : public CChat
{
public:
    CDebugView(CGUI* pManager, const CVector2D& vecPosition);

    void Draw(bool bUseCacheTexture, bool bAllowOutline);
    void Output(const char* szText, bool bColorCoded);

protected:
    // Debug view doesn't support position changes unlike chat box
    void UpdatePosition() override{};
};

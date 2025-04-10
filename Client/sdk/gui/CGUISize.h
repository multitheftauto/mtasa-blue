/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUISize.h
 *  PURPOSE:     Widget size interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CGUISize
{
public:
    CGUISize() {}
    CGUISize(float width, float height) : fWidth(width), fHeight(height) {}

    bool operator==(const CGUISize& other) const;
    bool operator!=(const CGUISize& other) const;

    float fWidth;
    float fHeight;
};

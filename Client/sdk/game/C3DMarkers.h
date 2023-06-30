/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/C3DMarkers.h
 *  PURPOSE:     3D marker manager interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class C3DMarker;
class CVector;

class C3DMarkers
{
public:
    virtual C3DMarker* CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector* vecPosition, float fSize, float fPulseFraction, uchar r, uchar g, uchar b,
                                    uchar a) = 0;
    virtual C3DMarker* FindFreeMarker() = 0;
};

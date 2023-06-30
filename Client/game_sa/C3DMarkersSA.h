/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/C3DMarkersSA.h
 *  PURPOSE:     Header file for 3D Marker entity manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/C3DMarkers.h>
#include "C3DMarkerSA.h"

#define MAX_3D_MARKERS          32

class C3DMarkersSA : public C3DMarkers
{
private:
    C3DMarkerSA* Markers[MAX_3D_MARKERS];

public:
    C3DMarkersSA();
    ~C3DMarkersSA();

    C3DMarker* CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector* vecPosition, float fSize, float fPulseFraction, uchar r, uchar g, uchar b,
                            uchar a);
    C3DMarker* FindFreeMarker();
    C3DMarker* FindMarker(DWORD Identifier);
};

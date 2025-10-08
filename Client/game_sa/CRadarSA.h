/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRadarSA.h
 *  PURPOSE:     Header file for game radar class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRadar.h>
#include "CMarkerSA.h"

#define ARRAY_CMarker                       0xBA86F0
#define MAX_MARKERS                         175

#define FUNC_DrawAreaOnRadar                0x5853d0
#define FUNC_SetCoordBlip                   0x583820

class CRadarSA : public CRadar
{
public:
    CRadarSA();
    ~CRadarSA();
    CMarker* CreateMarker(CVector* vecPosition);
    CMarker* GetFreeMarker();
    void     DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SharedUtil::SColor color);
};

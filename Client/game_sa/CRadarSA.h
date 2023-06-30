/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CRadarSA.h
 *  PURPOSE:     Header file for game radar class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRadar.h>
#include "CMarkerSA.h"

class CRadarSA : public CRadar
{
public:
    CRadarSA();
    ~CRadarSA();
    CMarker* CreateMarker(CVector* vecPosition);
    CMarker* GetFreeMarker();
    void     DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SharedUtil::SColor color);
};

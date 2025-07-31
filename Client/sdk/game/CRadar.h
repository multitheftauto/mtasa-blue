/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRadar.h
 *  PURPOSE:     Game radar interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CMarker;
class CVector;

class CRadar
{
public:
    virtual CMarker* CreateMarker(CVector* vecPosition) = 0;
    virtual CMarker* GetFreeMarker() = 0;
    virtual void     DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SharedUtil::SColor color) = 0;
};

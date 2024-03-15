/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeather.h
 *  PURPOSE:     Game weather interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CWeather
{
public:
    virtual std::uint8_t Get() = 0;
    virtual void          Set(std::uint8_t primary, std::uint8_t secondary) = 0;
    virtual void          Release() = 0;

    virtual float GetAmountOfRain() = 0;
    virtual void  SetAmountOfRain(float fAmount) = 0;
    virtual void  ResetAmountOfRain() = 0;
};

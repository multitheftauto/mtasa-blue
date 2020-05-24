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
    virtual unsigned char Get() = 0;
    virtual void          Set(unsigned char primary, unsigned char secondary) = 0;
    virtual void          SetPrimary(unsigned char weather) = 0;
    virtual void          SetSecondary(unsigned char weather) = 0;

    virtual void ForceWeather(unsigned char weather) = 0;
    virtual void ForceWeatherNow(unsigned char weather) = 0;
    virtual void Release() = 0;

    virtual bool  IsRaining() = 0;
    virtual float GetAmountOfRain() = 0;
    virtual void  SetAmountOfRain(float fAmount) = 0;
    virtual void  ResetAmountOfRain() = 0;
};

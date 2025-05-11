/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeather.h
 *  PURPOSE:     Game weather interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CWeather
{
public:
    virtual unsigned char Get() = 0;
    virtual void          Set(unsigned char primary, unsigned char secondary) = 0;
    virtual void          Release() = 0;

    virtual float GetAmountOfRain() = 0;
    virtual void  SetAmountOfRain(float fAmount) = 0;
    virtual void  ResetAmountOfRain() = 0;

    virtual float GetWetRoads() const = 0;
    virtual bool  SetWetRoads(float fAmount) = 0;
    virtual bool  ResetWetRoads() = 0;

    virtual float GetFoggyness() const = 0;
    virtual bool  SetFoggyness(float fAmount) = 0;
    virtual bool  ResetFoggyness() = 0;

    virtual float GetFog() const = 0;
    virtual bool  SetFog(float fAmount) = 0;
    virtual bool  ResetFog() = 0;

    virtual float GetRainFog() const = 0;
    virtual bool  SetRainFog(float fAmount) = 0;
    virtual bool  ResetRainFog() = 0;

    virtual float GetWaterFog() const = 0;
    virtual bool  SetWaterFog(float fAmount) = 0;
    virtual bool  ResetWaterFog() = 0;

    virtual float GetSandstorm() const = 0;
    virtual bool  SetSandstorm(float fAmount) = 0;
    virtual bool  ResetSandstorm() = 0;

    virtual float GetRainbow() const = 0;
    virtual bool  SetRainbow(float fAmount) = 0;
    virtual bool  ResetRainbow() = 0;
};

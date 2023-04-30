/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeatherSA.h
 *  PURPOSE:     Header file for weather class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWeather.h>

class CWeatherSA : public CWeather
{
    friend class COffsets;

public:
    unsigned char Get();
    void          Set(unsigned char primary, unsigned char secondary);

    void Release();

    float GetAmountOfRain();
    void  SetAmountOfRain(float fAmount);
    void  ResetAmountOfRain();

    float GetWetRoads();
    bool  SetWetRoads(float fAmount);
    bool  ResetWetRoads();

    float GetFoggyness();
    bool  SetFoggyness(float fAmount);
    bool  ResetFoggyness();

    float GetFog();
    bool  SetFog(float fAmount);
    bool  ResetFog();

    float GetRainFog();
    bool  SetRainFog(float fAmount);
    bool  ResetRainFog();

    float GetWaterFog();
    bool  SetWaterFog(float fAmount);
    bool  ResetWaterFog();

    float GetSandstorm();
    bool  SetSandstorm(float fAmount);
    bool  ResetSandstorm();

    float GetRainbow();
    bool  SetRainbow(float fAmount);
    bool  ResetRainbow();

private:
    static unsigned char* VAR_CWeather__ForcedWeatherType;
    static unsigned char* VAR_CWeather__OldWeatherType;
    static unsigned char* VAR_CWeather__NewWeatherType;
    static float*         VAR_CWeather__Rain;
};

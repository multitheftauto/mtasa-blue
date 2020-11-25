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
#include "Common.h"

class CWeatherSA : public CWeather
{
    friend class COffsets;

public:
    unsigned char Get();
    void          Set(unsigned char primary, unsigned char secondary);
    void          SetPrimary(unsigned char weather);
    void          SetSecondary(unsigned char weather);

    void ForceWeather(unsigned char weather);
    void ForceWeatherNow(unsigned char weather);
    void Release();

    bool  IsRaining();
    float GetAmountOfRain();
    void  SetAmountOfRain(float fAmount);
    void  ResetAmountOfRain();

private:
    static unsigned char* VAR_CurrentWeather;
    static unsigned char* VAR_CurrentWeather_b;
    static unsigned char* VAR_CurrentWeather_c;
    static float*         VAR_AmountOfRain;

    static unsigned long FUNC_IsRaining;
};

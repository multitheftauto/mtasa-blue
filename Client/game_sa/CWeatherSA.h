/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CWeatherSA.h
 *  PURPOSE:     Header file for weather class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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

private:
    static unsigned char* VAR_CWeather__ForcedWeatherType;
    static unsigned char* VAR_CWeather__OldWeatherType;
    static unsigned char* VAR_CWeather__NewWeatherType;
    static float*         VAR_CWeather__Rain;
};

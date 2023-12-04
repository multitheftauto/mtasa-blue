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
};

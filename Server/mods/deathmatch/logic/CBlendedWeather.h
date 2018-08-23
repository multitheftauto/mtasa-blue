/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBlendedWeather.h
 *  PURPOSE:     Weather blend manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClock.h"

class CBlendedWeather
{
public:
    CBlendedWeather(CClock* pClock);

    void DoPulse(void);

    void SetWeather(unsigned char ucWeather);
    void SetWeatherBlended(unsigned char ucWeather, unsigned char ucHour);

    unsigned char GetWeather(void);
    unsigned char GetWeatherBlendingTo(void) { return m_ucSecondaryBlendedWeather; };
    unsigned char GetBlendedStartHour(void) { return m_ucBlendStartHour; };
    unsigned char GetBlendedStopHour(void) { return m_ucBlendStopHour; };

private:
    void Update(void);

    unsigned char m_ucPrimaryWeather;
    unsigned char m_ucSecondaryWeather;
    unsigned char m_ucPrimaryBlendedWeather;
    unsigned char m_ucSecondaryBlendedWeather;

    unsigned char m_ucBlendStartHour;
    unsigned char m_ucBlendStopHour;

    CClock* m_pClock;
};

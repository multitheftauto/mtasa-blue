/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBlendedWeather.h
 *  PURPOSE:     Header file for blended weather class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CBlendedWeather
{
public:
    CBlendedWeather();

    void DoPulse();

    void SetWeather(std::uint8_t ucWeather);
    void SetWeatherBlended(std::uint8_t ucWeather, std::uint8_t ucHour);

    std::uint8_t GetWeather();
    std::uint8_t GetWeatherBlendingTo() { return m_ucSecondaryBlendedWeather; };
    std::uint8_t GetBlendedStartHour() { return m_ucBlendStartHour; };
    std::uint8_t GetBlendedStopHour() { return m_ucBlendStopHour; };

private:
    std::uint8_t m_ucPrimaryWeather;
    std::uint8_t m_ucSecondaryWeather;
    std::uint8_t m_ucPrimaryBlendedWeather;
    std::uint8_t m_ucSecondaryBlendedWeather;

    std::uint8_t m_ucBlendStartHour;
    std::uint8_t m_ucBlendStopHour;

    CClock*   m_pClock;
    CWeather* m_pWeather;
};

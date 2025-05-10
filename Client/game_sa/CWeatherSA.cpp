/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeatherSA.cpp
 *  PURPOSE:     Weather logic
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWeatherSA.h"

static float WEATHER_FAKE_ACCUMULATOR;

unsigned char CWeatherSA::Get()
{
    return *(unsigned char*)0xC81318; // CWeather::ForcedWeatherType
}

void CWeatherSA::Set(unsigned char primary, unsigned char secondary)
{
    MemPutFast<unsigned char>(0xC81320, primary); // CWeather::OldWeatherType
    MemPutFast<unsigned char>(0xC8131C, secondary); // CWeather::NewWeatherType
}

void CWeatherSA::Release()
{
    MemPutFast<unsigned char>(0xC81318, 0xFF); // CWeather::ForcedWeatherType
}

float CWeatherSA::GetAmountOfRain()
{
    return *(float*)0xC81324; // CWeather::Rain
}

void CWeatherSA::SetAmountOfRain(float fAmount)
{
    // Patch all the places inside of CWeather::Update that would overwrite CWeather::Rain
    MemPut<DWORD>((LPVOID)(0x72C686 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BC92 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemSet((void*)0x72BC72, 0x90, 5);

    // Set the amount of rain
    MemPutFast<float>(0xC81324, fAmount); // CWeather::Rain
}

void CWeatherSA::ResetAmountOfRain()
{
    BYTE originalMov[5] = {0xA3, 0x24, 0x13, 0xC8, 0x00};                    // 0x72BC72
    MemCpy((LPVOID)0x72BC72, &originalMov, 5);

    static constexpr DWORD originalAddr = 0x00C81324;
    MemPut<DWORD>((LPVOID)(0x72C686 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BC92 + 2), originalAddr);
}

float CWeatherSA::GetWetRoads() const
{
    return *(float*)0xC81308;
}

bool CWeatherSA::SetWetRoads(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72BB9F + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BBB7 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BBD0 + 1), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BBD7 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC81308, fAmount);
    return true;
}

bool CWeatherSA::ResetWetRoads()
{
    static constexpr DWORD originalAddr = 0x00C81308;
    MemPut<DWORD>((LPVOID)(0x72BB9F + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BBB7 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BBD0 + 1), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BBD7 + 2), originalAddr);
    return true;
}

float CWeatherSA::GetFoggyness() const
{
    return *(float*)0xC81300;
}

bool CWeatherSA::SetFoggyness(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72BDF5 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BDDD + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BE13 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC81300, fAmount);
    return true;
}

bool CWeatherSA::ResetFoggyness()
{
    static constexpr DWORD originalAddr = 0x00C81300;
    MemPut<DWORD>((LPVOID)(0x72BDF5 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BDDD + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BE13 + 2), originalAddr);
    return true;
}

float CWeatherSA::GetFog() const
{
    return *(float*)0xC812FC;
}

bool CWeatherSA::SetFog(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72BE37 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BE1F + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BE4F + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC812FC, fAmount);
    return true;
}

bool CWeatherSA::ResetFog()
{
    static constexpr DWORD originalAddr = 0x00C812FC;
    MemPut<DWORD>((LPVOID)(0x72BE37 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BE1F + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BE4F + 2), originalAddr);
    return true;
}

float CWeatherSA::GetRainFog() const
{
    return *(float*)0xC81410;
}

bool CWeatherSA::SetRainFog(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72ADD8 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72ADE4 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC81410, fAmount);
    return true;
}

bool CWeatherSA::ResetRainFog()
{
    static constexpr DWORD originalAddr = 0x00C81410;
    MemPut<DWORD>((LPVOID)(0x72ADD8 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72ADE4 + 2), originalAddr);
    return true;
}

float CWeatherSA::GetWaterFog() const
{
    return *(float*)0xC81338;
}

bool CWeatherSA::SetWaterFog(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72C35C + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72C38E + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72C36F + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC81338, fAmount);
    return true;
}

bool CWeatherSA::ResetWaterFog()
{
    static constexpr DWORD originalAddr = 0x00C81338;
    MemPut<DWORD>((LPVOID)(0x72C35C + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72C38E + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72C36F + 2), originalAddr);
    return true;
}

float CWeatherSA::GetSandstorm() const
{
    return *(float*)0xC812F4;
}

bool CWeatherSA::SetSandstorm(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72A4B6 + 1), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BCEB + 1), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BD0B + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC812F4, fAmount);
    return true;
}

bool CWeatherSA::ResetSandstorm()
{
    static constexpr DWORD originalAddr = 0x00C812F4;
    MemPut<DWORD>((LPVOID)(0x72A4B6 + 1), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BCEB + 1), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BD0B + 2), originalAddr);
    return true;
}

float CWeatherSA::GetRainbow() const
{
    return *(float*)0xC812E4;
}

bool CWeatherSA::SetRainbow(float fAmount)
{
    MemPut<DWORD>((LPVOID)(0x72BF51 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);
    MemPut<DWORD>((LPVOID)(0x72BF59 + 2), (DWORD)&WEATHER_FAKE_ACCUMULATOR);

    MemPutFast<float>(0xC812E4, fAmount);
    return true;
}

bool CWeatherSA::ResetRainbow()
{
    static constexpr DWORD originalAddr = 0x00C812E4;
    MemPut<DWORD>((LPVOID)(0x72BF51 + 2), originalAddr);
    MemPut<DWORD>((LPVOID)(0x72BF59 + 2), originalAddr);
    return true;
}

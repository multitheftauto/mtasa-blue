/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeatherSA.cpp
 *  PURPOSE:     Weather logic
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWeatherSA.h"

unsigned char* CWeatherSA::VAR_CWeather__ForcedWeatherType;
unsigned char* CWeatherSA::VAR_CWeather__OldWeatherType;
unsigned char* CWeatherSA::VAR_CWeather__NewWeatherType;
float*         CWeatherSA::VAR_CWeather__Rain;

unsigned char CWeatherSA::Get()
{
    return (unsigned char)*VAR_CWeather__ForcedWeatherType;
}

void CWeatherSA::Set(unsigned char primary, unsigned char secondary)
{
    *VAR_CWeather__OldWeatherType = static_cast<unsigned char>(primary);
    *VAR_CWeather__NewWeatherType = static_cast<unsigned char>(secondary);
}

void CWeatherSA::Release()
{
    *VAR_CWeather__ForcedWeatherType = 0xFF;
}

float CWeatherSA::GetAmountOfRain()
{
    return *VAR_CWeather__Rain;
}

void CWeatherSA::SetAmountOfRain(float fAmount)
{
    // Nop the functions which don't like us take care of the rain
    MemPut<BYTE>(0x72C686, 0xDD);
    MemPut<BYTE>(0x72C687, 0xD8);

    MemSet((void*)0x72C688, 0x90, 4);

    MemPut<BYTE>(0x72BC92, 0xDD);
    MemPut<BYTE>(0x72BC93, 0xD8);

    MemSet((void*)0x72BC94, 0x90, 4);

    MemSet((void*)0x72BC72, 0x90, 5);

    // Set the amount of rain
    *VAR_CWeather__Rain = fAmount;
}

void CWeatherSA::ResetAmountOfRain()
{
    BYTE originalMov[5] = {0xA3, 0x24, 0x13, 0xC8, 0x00};                    // 0x72BC72
    BYTE originalFstp1[6] = {0xD9, 0x1D, 0x24, 0x13, 0xC8, 0x00};            // 0x72BC92
    BYTE originalFstp2[6] = {0xD9, 0x1D, 0x24, 0x13, 0xC8, 0x00};            // 0x72C686

    MemCpy((LPVOID)0x72BC72, &originalMov, 5);
    MemCpy((LPVOID)0x72BC92, &originalFstp1, 6);
    MemCpy((LPVOID)0x72C686, &originalFstp2, 6);
}

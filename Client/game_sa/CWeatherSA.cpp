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
    MemPut<BYTE>(0x72C686, 0xDD);
    MemPut<BYTE>(0x72C687, 0xD8);

    MemSet((void*)0x72C688, 0x90, 4);

    MemPut<BYTE>(0x72BC92, 0xDD);
    MemPut<BYTE>(0x72BC93, 0xD8);

    MemSet((void*)0x72BC94, 0x90, 4);

    MemSet((void*)0x72BC72, 0x90, 5);

    // Set the amount of rain
    MemPutFast<float>(0xC81324, fAmount); // CWeather::Rain
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

float CWeatherSA::GetWetRoads() const
{
    return *(float*)0xC81308;
}

bool CWeatherSA::SetWetRoads(float fAmount)
{
    MemSet((LPVOID)(0x72BB9F + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BBB7 + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BBD0 + 1), 0x90, 3);
    MemSet((LPVOID)(0x72BBD7 + 2), 0x90, 3);

    MemPutFast<float>(0xC81308, fAmount);
    return true;
}

bool CWeatherSA::ResetWetRoads()
{
    BYTE originalCodes[3] = {0x08, 0x13, 0xC8};
    MemCpy((LPVOID)(0x72BB9F + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BBB7 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BBD0 + 1), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BBD7 + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetFoggyness() const
{
    return *(float*)0xC81300;
}

bool CWeatherSA::SetFoggyness(float fAmount)
{
    MemSet((LPVOID)(0x72BDF5 + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BDDD + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BE13 + 2), 0x90, 3);

    MemPutFast<float>(0xC81300, fAmount);
    return true;
}

bool CWeatherSA::ResetFoggyness()
{
    BYTE originalCodes[3] = {0x00, 0x13, 0xC8};
    MemCpy((LPVOID)(0x72BDF5 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BDDD + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BE13 + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetFog() const
{
    return *(float*)0xC812FC;
}

bool CWeatherSA::SetFog(float fAmount)
{
    MemSet((LPVOID)(0x72BE37 + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BE1F + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BE4F + 2), 0x90, 3);

    MemPutFast<float>(0xC812FC, fAmount);
    return true;
}

bool CWeatherSA::ResetFog()
{
    BYTE originalCodes[3] = {0xFC, 0x12, 0xC8};
    MemCpy((LPVOID)(0x72BE37 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BE1F + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BE4F + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetRainFog() const
{
    return *(float*)0xC81410;
}

bool CWeatherSA::SetRainFog(float fAmount)
{
    MemSet((LPVOID)(0x72ADD8 + 2), 0x90, 3);
    MemSet((LPVOID)(0x72ADE4 + 2), 0x90, 3);

    MemPutFast<float>(0xC81410, fAmount);
    return true;
}

bool CWeatherSA::ResetRainFog()
{
    BYTE originalCodes[3] = {0x10, 0x14, 0xC8};
    MemCpy((LPVOID)(0x72ADD8 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72ADE4 + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetWaterFog() const
{
    return *(float*)0xC81338;
}

bool CWeatherSA::SetWaterFog(float fAmount)
{
    MemSet((LPVOID)(0x72C35C + 2), 0x90, 3);
    MemSet((LPVOID)(0x72C38E + 2), 0x90, 3);
    MemSet((LPVOID)(0x72C36F + 2), 0x90, 3);

    MemPutFast<float>(0xC81338, fAmount);
    return true;
}

bool CWeatherSA::ResetWaterFog()
{
    BYTE originalCodes[3] = {0x38, 0x13, 0xC8};
    MemCpy((LPVOID)(0x72C35C + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72C38E + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72C36F + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetSandstorm() const
{
    return *(float*)0xC812F4;
}

bool CWeatherSA::SetSandstorm(float fAmount)
{
    MemSet((LPVOID)(0x72A4B6 + 1), 0x90, 3);
    MemSet((LPVOID)(0x72BCEB + 1), 0x90, 3);
    MemSet((LPVOID)(0x72BD0B + 2), 0x90, 3);

    MemPutFast<float>(0xC812F4, fAmount);
    return true;
}

bool CWeatherSA::ResetSandstorm()
{
    BYTE originalCodes[3] = {0xF4, 0x12, 0xC8};
    MemCpy((LPVOID)(0x72A4B6 + 1), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BCEB + 1), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BD0B + 2), &originalCodes, 3);
    return true;
}

float CWeatherSA::GetRainbow() const
{
    return *(float*)0xC812E4;
}

bool CWeatherSA::SetRainbow(float fAmount)
{
    MemSet((LPVOID)(0x72BF51 + 2), 0x90, 3);
    MemSet((LPVOID)(0x72BF59 + 2), 0x90, 3);

    MemPutFast<float>(0xC812E4, fAmount);
    return true;
}

bool CWeatherSA::ResetRainbow()
{
    BYTE originalCodes[3] = {0xE4, 0x12, 0xC8};
    MemCpy((LPVOID)(0x72BF51 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x72BF59 + 2), &originalCodes, 3);
    return true;
}

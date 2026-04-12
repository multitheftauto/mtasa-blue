/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Audio entity radio track manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAERadioTrackManagerSA.h"

BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DWORD dwFunc = FUNC_GetCurrentRadioStationID;
    BYTE  bReturn = 0;
    // clang-format off
    using func_t = decltype(bReturn) (__thiscall*)(decltype(0x8CB6F8) );
bReturn =     reinterpret_cast<func_t>(dwFunc)(0x8CB6F8);
    // clang-format on

    return bReturn;
}

BYTE CAERadioTrackManagerSA::IsVehicleRadioActive()
{
    DWORD dwFunc = FUNC_IsVehicleRadioActive;
    BYTE  bReturn = 0;
    // clang-format off
    using func_t = decltype(bReturn) (__thiscall*)(decltype(0x8CB6F8) );
bReturn =     reinterpret_cast<func_t>(dwFunc)(0x8CB6F8);
    // clang-format on

    return bReturn;
}

char* CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)
{
    DWORD dwFunc = FUNC_GetRadioStationName;
    char* cReturn = 0;
    DWORD dwStationID = bStationID;
    // clang-format off
    using func_t = decltype(cReturn) (__thiscall*)(decltype(0x8CB6F8), decltype(dwStationID));
cReturn =     reinterpret_cast<func_t>(dwFunc)(0x8CB6F8, dwStationID);
    // clang-format on

    return cReturn;
}

bool CAERadioTrackManagerSA::IsRadioOn()
{
    DWORD dwFunc = FUNC_IsRadioOn;
    bool  bReturn = false;
    // clang-format off
    using func_t = decltype(bReturn) (__thiscall*)(decltype(0x8CB6F8) );
bReturn =     reinterpret_cast<func_t>(dwFunc)(0x8CB6F8);
    // clang-format on

    return bReturn;
}

void CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)
{
    DWORD dwFunc = FUNC_SetBassSetting;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(0x8CB6F8), decltype(dwBass), decltype(0x3F800000));
    reinterpret_cast<func_t>(dwFunc)(0x8CB6F8, dwBass, 0x3F800000);
    // clang-format on
}

void CAERadioTrackManagerSA::Reset()
{
    DWORD dwFunc = FUNC_Reset;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(0x8CB6F8) );
    reinterpret_cast<func_t>(dwFunc)(0x8CB6F8);
    // clang-format on
}

void CAERadioTrackManagerSA::StartRadio(BYTE bStationID, BYTE bUnknown)
{
    DWORD dwFunc = FUNC_StartRadio;
    DWORD dwStationID = bStationID;
    DWORD dwUnknown = bUnknown;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(0x8CB6F8), decltype(dwStationID), decltype(dwUnknown), decltype(0), decltype(0));
    reinterpret_cast<func_t>(dwFunc)(0x8CB6F8, dwStationID, dwUnknown, 0, 0);
    // clang-format on
}

bool CAERadioTrackManagerSA::IsStationLoading() const
{
    CAERadioTrackManagerSAInterface* trackInterface = GetInterface();
    return (trackInterface->stationsListed || trackInterface->stationsListDown);
}

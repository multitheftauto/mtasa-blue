/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Audio entity radio track manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DEBUG_TRACE("BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()");

    // CAERadioTrackManager::GetCurrentRadioStationID
    return ((BYTE(__thiscall*)(int))FUNC_GetCurrentRadioStationID)(CLASS_CAERadioTrackManager);
}

BYTE CAERadioTrackManagerSA::IsVehicleRadioActive()
{
    DEBUG_TRACE("BYTE CAERadioTrackManagerSA::IsVehicleRadioActive()");

    // CAERadioTrackManager::IsVehicleRadioActive
    return ((bool(__thiscall*)(int))FUNC_IsVehicleRadioActive)(CLASS_CAERadioTrackManager);
}

char* CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)
{
    DEBUG_TRACE("char * CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)");

    // CAERadioTrackManager::GetRadioStationName
    return ((char*(__thiscall*)(int, char))FUNC_GetRadioStationName)(CLASS_CAERadioTrackManager, bStationID);
}

BOOL CAERadioTrackManagerSA::IsRadioOn()
{
    DEBUG_TRACE("BOOL CAERadioTrackManagerSA::IsRadioOn()");

    // CAERadioTrackManager::IsRadioOn
    return ((bool(__thiscall*)(int))FUNC_IsRadioOn)(CLASS_CAERadioTrackManager);
}

VOID CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)");

    // CAERadioTrackManager::SetBassSetting
    ((void(__thiscall*)(int, char, float))FUNC_SetBassSetting)(CLASS_CAERadioTrackManager, dwBass, 1.0f);
}

VOID CAERadioTrackManagerSA::Reset()
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::Reset()");

    // CAERadioTrackManager::Reset
    ((void(__thiscall*)(int))FUNC_Reset)(CLASS_CAERadioTrackManager);
}

VOID CAERadioTrackManagerSA::StartRadio(BYTE bStationID, BYTE bUnknown)
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::StartRadio(BYTE bStationID, BYTE bUnknown)");

    // CAERadioTrackManager::StartRadio
    ((void(__thiscall*)(int, char, char, float, unsigned char))FUNC_StartRadio)(CLASS_CAERadioTrackManager, bStationID, bUnknown, 0, 0);
}

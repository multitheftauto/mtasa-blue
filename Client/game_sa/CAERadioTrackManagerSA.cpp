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
#include "CAERadioTrackManagerSA.h"

BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DWORD dwFunc = FUNC_GetCurrentRadioStationID;
    BYTE  bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

BYTE CAERadioTrackManagerSA::IsVehicleRadioActive()
{
    DWORD dwFunc = FUNC_IsVehicleRadioActive;
    BYTE  bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

char* CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)
{
    DWORD dwFunc = FUNC_GetRadioStationName;
    char* cReturn = 0;
    DWORD dwStationID = bStationID;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    dwStationID
        call    dwFunc
        mov     cReturn, eax
    }

    return cReturn;
}

bool CAERadioTrackManagerSA::IsRadioOn()
{
    DWORD dwFunc = FUNC_IsRadioOn;
    bool  bReturn = false;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

void CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)
{
    DWORD dwFunc = FUNC_SetBassSetting;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    0x3F800000 // 1.0f
        push    dwBass
        call    dwFunc
    }
}

void CAERadioTrackManagerSA::Reset()
{
    DWORD dwFunc = FUNC_Reset;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
    }
}

void CAERadioTrackManagerSA::StartRadio(BYTE bStationID, BYTE bUnknown)
{
    DWORD dwFunc = FUNC_StartRadio;
    DWORD dwStationID = bStationID;
    DWORD dwUnknown = bUnknown;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    0
        push    0
        push    dwUnknown
        push    dwStationID
        call    dwFunc
    }
}

bool CAERadioTrackManagerSA::IsStationLoading() const
{
    CAERadioTrackManagerSAInterface* trackInterface = GetInterface();
    return (trackInterface->stationsListed || trackInterface->stationsListDown);
}

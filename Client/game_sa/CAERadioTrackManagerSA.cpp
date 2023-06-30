/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Audio entity radio track manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAERadioTrackManagerSA.h"

#define CLASS_CAERadioTrackManager 0x8CB6F8

uchar CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DWORD dwFunc = 0x4E83F0;
    uchar bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

uchar CAERadioTrackManagerSA::IsVehicleRadioActive()
{
    DWORD dwFunc = 0x4E9800;
    uchar bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

char* CAERadioTrackManagerSA::GetRadioStationName(uchar bStationID)
{
    DWORD dwFunc = 0x4E9E10;
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
    DWORD dwFunc = 0x4E8350;
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
    DWORD dwFunc = 0x4E82F0;
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
    DWORD dwFunc = 0x4E7F80;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
    }
}

void CAERadioTrackManagerSA::StartRadio(uchar bStationID, uchar bUnknown)
{
    DWORD dwFunc = 0x4EB3C0;
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

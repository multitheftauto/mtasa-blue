/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAERadioTrackManagerSA.cpp
*  PURPOSE:     Audio entity radio track manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DEBUG_TRACE("BYTE CAERadioTrackManagerSA::GetCurrentRadioStationID()");
    DWORD dwFunc = FUNC_GetCurrentRadioStationID;
    BYTE bReturn = 0;
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
    DEBUG_TRACE("BYTE CAERadioTrackManagerSA::IsVehicleRadioActive()");
    DWORD dwFunc = FUNC_IsVehicleRadioActive;
    BYTE bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

char * CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)
{
    DEBUG_TRACE("char * CAERadioTrackManagerSA::GetRadioStationName(BYTE bStationID)");
    DWORD dwFunc = FUNC_GetRadioStationName;
    char * cReturn = 0;
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

BOOL CAERadioTrackManagerSA::IsRadioOn()
{
    DEBUG_TRACE("BOOL CAERadioTrackManagerSA::IsRadioOn()");
    DWORD dwFunc = FUNC_IsRadioOn;
    BYTE bReturn = 0;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

VOID CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::SetBassSetting(DWORD dwBass)");
    DWORD dwFunc = FUNC_SetBassSetting;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    0x3F800000 // 1.0f
        push    dwBass
        call    dwFunc
    }
}

VOID CAERadioTrackManagerSA::Reset()
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::Reset()");
    DWORD dwFunc = FUNC_Reset;
    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
    }
}

VOID CAERadioTrackManagerSA::RetuneRadio ( BYTE bStationID )
{
    DEBUG_TRACE("VOID CAERadioTrackManagerSA::RetuneRadio(BYTE bStationID)");
    DWORD dwFunc = FUNC_RetuneRadio;
    DWORD dwStationID = bStationID;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    dwStationID
        call    dwFunc
    }
}

VOID CAERadioTrackManagerSA::StartRadio ( BYTE bStationID )
{
    DEBUG_TRACE ( "VOID CAERadioTrackManagerSA::StartRadio(BYTE bStationID)" );
    DWORD dwFunc = FUNC_StartRadio2;
    DWORD dwStationID = bStationID;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    1
        push    dwStationID
        call    dwFunc
    }
}


/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Header file for audio entity radio track manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAERadioTrackManager.h>

#define FUNC_GetCurrentRadioStationID   0x4E83F0
#define FUNC_IsVehicleRadioActive       0x4E9800
#define FUNC_GetRadioStationName        0x4E9E10
#define FUNC_IsRadioOn                  0x4E8350
#define FUNC_SetBassSetting             0x4E82F0
#define FUNC_Reset                      0x4E7F80
#define FUNC_StartRadio                 0x4EB3C0

#define CLASS_CAERadioTrackManager      0x8CB6F8

class CAERadioTrackManagerSA : public CAERadioTrackManager
{
public:
    BYTE  GetCurrentRadioStationID();
    BYTE  IsVehicleRadioActive();
    char* GetRadioStationName(BYTE bStationID);
    bool  IsRadioOn();
    void  SetBassSetting(DWORD dwBass);
    void  Reset();
    void  StartRadio(BYTE bStationID, BYTE bUnknown);
};

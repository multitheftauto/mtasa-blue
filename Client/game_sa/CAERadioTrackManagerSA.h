/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Header file for audio entity radio track manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAERadioTrackManager.h>

class CAERadioTrackManagerSA : public CAERadioTrackManager
{
public:
    uchar GetCurrentRadioStationID();
    uchar IsVehicleRadioActive();
    char* GetRadioStationName(uchar bStationID);
    bool  IsRadioOn();
    void  SetBassSetting(DWORD dwBass);
    void  Reset();
    void  StartRadio(uchar bStationID, uchar bUnknown);
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/CAERadioTrackManager.h
 *  PURPOSE:     Radio track audio entity interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAERadioTrackManager
{
public:
    virtual uchar GetCurrentRadioStationID() = 0;
    virtual uchar IsVehicleRadioActive() = 0;
    virtual char* GetRadioStationName(uchar bStationID) = 0;
    virtual bool  IsRadioOn() = 0;
    virtual void  SetBassSetting(DWORD dwBass) = 0;
    virtual void  Reset() = 0;
    virtual void  StartRadio(uchar bStationID, uchar bUnknown) = 0;
};

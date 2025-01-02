/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAERadioTrackManager.h
 *  PURPOSE:     Radio track audio entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAERadioTrackManager
{
public:
    virtual BYTE  GetCurrentRadioStationID() = 0;
    virtual BYTE  IsVehicleRadioActive() = 0;
    virtual char* GetRadioStationName(BYTE bStationID) = 0;
    virtual bool  IsRadioOn() = 0;
    virtual void  SetBassSetting(DWORD dwBass) = 0;
    virtual void  Reset() = 0;
    virtual void  StartRadio(BYTE bStationID, BYTE bUnknown) = 0;
    virtual bool  IsStationLoading() const = 0;
};

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

#ifndef __CGAME_CAERADIOTRACKMANAGER
#define __CGAME_CAERADIOTRACKMANAGER

#include <windows.h>

class CAERadioTrackManager
{
public:
    virtual BYTE        GetCurrentRadioStationID        ( void ) = 0;
    virtual BYTE        IsVehicleRadioActive            ( void ) = 0;
    virtual char *      GetRadioStationName             ( BYTE bStationID ) = 0;
    virtual BOOL        IsRadioOn                       ( void ) = 0;
    virtual void        SetBassSetting                  ( DWORD dwBass ) = 0;
    virtual void        Reset                           ( void ) = 0;
    virtual void        RetuneRadio                     ( BYTE bStationID ) = 0;
    virtual void        StartRadio                      ( BYTE bStationID ) = 0;
};

#endif

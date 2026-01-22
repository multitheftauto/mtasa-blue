/*****************************************************************************
 *
 *  PROJECT:        Multi Theft Auto v1.0
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:        sdk/game/CAEVehicleAudioEntity.h
 *  PURPOSE:        Vehicle audio interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum eVehicleAudioType
{
    VEHICLE_AUDIO_CAR = 0,
    VEHICLE_AUDIO_BIKE,
    VEHICLE_AUDIO_BICYCLE,
    VEHICLE_AUDIO_BOAT,
    VEHICLE_AUDIO_HELI,
    VEHICLE_AUDIO_PLANE,
    VEHICLE_AUDIO_SEAPLANE,            // unused?

    VEHICLE_AUDIO_TRAIN = 8,
    VEHICLE_AUDIO_SPECIAL,            // RC vehicles, vortex, caddy, few trailers
    VEHICLE_AUDIO_SILENT
};

class CAEVehicleAudioEntity
{
public:
    virtual void JustGotInVehicleAsDriver() = 0;
    virtual void JustGotOutOfVehicleAsDriver() = 0;
    virtual void TurnOnRadioForVehicle() = 0;
};
